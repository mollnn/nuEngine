#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool key_status[512];
float mouse_x, mouse_y;
bool mouse_button_status[3];

void framesizeCallback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (action == GLFW_PRESS)
    {
        key_status[key] = true;
    }
    if (action == GLFW_RELEASE)
    {
        key_status[key] = false;
    }
}

void cursorCallback(GLFWwindow *window, double mx, double my)
{
    mouse_x = mx;
    mouse_y = my;
}

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_button_status[0] = 1;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mouse_button_status[1] = 1;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_button_status[2] = 1;
            break;
        default:
            return;
        }
    }
    if (action == GLFW_RELEASE)
    {
        switch (button)
        {
        case GLFW_MOUSE_BUTTON_LEFT:
            mouse_button_status[0] = 0;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            mouse_button_status[1] = 0;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            mouse_button_status[2] = 0;
            break;
        default:
            return;
        }
    }
    return;
}

GLuint loadShader(const std::string &vs_name, const std::string &fs_name)
{
    std::ifstream vs_ifs(vs_name), fs_ifs(fs_name);
    std::stringstream vs_ss, fs_ss;
    vs_ss << vs_ifs.rdbuf();
    fs_ss << fs_ifs.rdbuf();
    std::string vs_src = vs_ss.str(), fs_src = fs_ss.str();
    const GLchar *vs_src_ptr = vs_src.c_str(), *fs_src_ptr = fs_src.c_str();

    GLuint vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vs_src_ptr, nullptr);
    glCompileShader(vs);

    GLint vs_stat;
    char vs_info[512];
    glGetShaderiv(vs, GL_COMPILE_STATUS, &vs_stat);
    if (!vs_stat)
    {
        glGetShaderInfoLog(vs, 512, nullptr, vs_info);
        std::cerr << "VS Compiler error: " << vs_name << ": " << vs_info << std::endl;
        return 0;
    }

    GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fs_src_ptr, nullptr);
    glCompileShader(fs);

    GLint fs_stat;
    char fs_info[512];
    glGetShaderiv(fs, GL_COMPILE_STATUS, &fs_stat);
    if (!fs_stat)
    {
        glGetShaderInfoLog(fs, 512, nullptr, fs_info);
        std::cerr << "FS Compiler error: " << fs_name << ": " << fs_info << std::endl;
        return 0;
    }

    GLuint sp = glCreateProgram();
    glAttachShader(sp, vs);
    glAttachShader(sp, fs);
    glLinkProgram(sp);

    GLint sp_stat;
    char sp_info[512];
    glGetProgramiv(sp, GL_LINK_STATUS, &sp_stat);
    if (!sp_stat)
    {
        glGetProgramInfoLog(sp, 512, nullptr, sp_info);
        std::cerr << "Shader Link error: " << vs_name << ": " << sp_info << std::endl;
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return sp;
}

class Shader
{
    int sp_;

public:
    Shader(const std::string &vs_name, const std::string &fs_name)
    {
        sp_ = loadShader(vs_name, fs_name);
    }

    void use()
    {
        glUseProgram(sp_);
    }

    void setUniform(const std::string &name, int x)
    {
        glUniform1i(glGetUniformLocation(sp_, name.c_str()), x);
    }

    void setUniform(const std::string &name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(sp_, name.c_str()), x, y, z);
    }
    void setUniform(const std::string &name, const glm::mat4 &value)
    {
        glUniformMatrix4fv(glGetUniformLocation(sp_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }
};

GLuint loadTexture(const std::string &tex_name)
{
    int sx, sy;
    unsigned char *tex_data = stbi_load(tex_name.c_str(), &sx, &sy, nullptr, 3);

    GLuint tex;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, sx, sy, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);

    return tex;
}

class Texture
{
    int handle_;

public:
    Texture(const std::string &tex_name)
    {
        handle_ = loadTexture(tex_name);
    }

    void use(int unit_id)
    {
        glActiveTexture(unit_id);
        glBindTexture(GL_TEXTURE0 + unit_id, handle_);
    }
};

class Mesh
{
    GLuint vao_;
    GLuint vbo_;
    int n_;

public:
    Mesh(const std::vector<GLfloat> &vertices)
    {
        n_ = vertices.size();
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

    void draw()
    {
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glDrawArrays(GL_TRIANGLES, 0, n_);
    }
};

int main()
{
    std::cout << "Nightup Engine v0.1" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(400, 400, "Nightup Engine v0.1", nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Failed at glfwCreateWindow" << std::endl;
        glfwTerminate();
        return 1;
    }

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framesizeCallback);
    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, cursorCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::vector<GLfloat> vertices = {-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                                     1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                     0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f};
    std::vector<GLfloat> vertices2 = {-0.5f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                                      0.5f, 0.1f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                      0.0f, 0.1f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f};
    Mesh mesh(vertices);
    Mesh mesh2(vertices2);
    Shader shader("../shader.vs", "../shader.fs");
    Texture texture("tex1.jpg");

    glm::vec3 cam_pos(0.0, 0.5, 1.0);
    glm::vec3 cam_dir(0.0, 0.0, -1.0);
    glm::vec3 cam_up(0.0, 1.0, 0.0);

    float pitch = 0.0f, yaw = 0.0f;

    GLfloat cam_speed = 0.5f;
    GLfloat cam_rotate_speed = 1.0f;

    GLfloat last_frame_time = glfwGetTime();
    GLfloat last_cursor_x = mouse_x, last_cursor_y = mouse_y;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        GLfloat current_time = glfwGetTime();

        GLfloat delta_cursor_x = mouse_x - last_cursor_x;
        GLfloat delta_cursor_y = mouse_y - last_cursor_y;

        last_cursor_x = mouse_x;
        last_cursor_y = mouse_y;

        if (key_status[GLFW_KEY_LEFT_ALT] && mouse_button_status[0])
        {
            pitch += -89.0f * delta_cursor_y * cam_rotate_speed / 400;
            yaw += 360.0f * delta_cursor_x * cam_rotate_speed / 400;

            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        cam_dir.x = glm::cos(glm::radians(pitch)) * glm::cos(glm::radians(yaw));
        cam_dir.y = glm::sin(glm::radians(pitch));
        cam_dir.z = glm::cos(glm::radians(pitch)) * glm::sin(glm::radians(yaw));
        cam_up = glm::vec3(0.0f, 1.0f, 0.0f);
        glm::vec3 cam_hand = glm::cross(cam_dir, cam_up);
        cam_up = glm::cross(cam_hand, cam_dir);
        cam_up = glm::normalize(cam_up);
        cam_hand = glm::cross(cam_dir, cam_up);

        if (key_status[GLFW_KEY_UP])
        {
            cam_pos += cam_dir * cam_speed * (current_time - last_frame_time);
        }
        if (key_status[GLFW_KEY_DOWN])
        {
            cam_pos -= cam_dir * cam_speed * (current_time - last_frame_time);
        }
        if (key_status[GLFW_KEY_LEFT])
        {
            cam_pos -= cam_hand * cam_speed * (current_time - last_frame_time);
        }
        if (key_status[GLFW_KEY_RIGHT])
        {
            cam_pos += cam_hand * cam_speed * (current_time - last_frame_time);
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();
        texture.use(0);
        shader.setUniform("tex", 0);

        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(-0.1, 0.0, 0.0));

        glm::mat4 view(1.0);

        view = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);

        glm::mat4 projection(1.0);
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);

        shader.setUniform("n_point_light", 3);
        shader.setUniform("point_light[0].val", 0.1f, 0.1f, 0.1f);
        shader.setUniform("point_light[0].pos", 0.5f, 0.3f, 0.0f);
        shader.setUniform("point_light[1].val", 0.001f, 0.0f, 0.0f);
        shader.setUniform("point_light[1].pos", 0.0f, 0.01f, 0.0f);
        shader.setUniform("point_light[2].val", 1500000.0f, 1300000.0f, 1000000.0f);
        shader.setUniform("point_light[2].pos", 0.0f, 1000.0f, 0.0f);

        shader.setUniform("model", model);
        shader.setUniform("view", view);
        shader.setUniform("projection", projection);

        mesh.draw();
        mesh2.draw();

        glfwSwapBuffers(window);

        last_frame_time = current_time;
    }

    glfwTerminate();
    return 0;
}