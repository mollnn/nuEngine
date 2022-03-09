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

void mouseCallback(GLFWwindow *window, double mx, double my)
{
    mouse_x = mx;
    mouse_y = my;
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
    glfwSetCursorPosCallback(window, mouseCallback);

    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    std::vector<GLfloat> vertices = {-1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
                                     1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
                                     0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.5f};

    GLuint vao, vbo;
    glGenVertexArrays(1, &vao);
    glGenBuffers(1, &vbo);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(GLfloat), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
    glEnableVertexAttribArray(2);

    GLuint shader = loadShader("../shader.vs", "../shader.fs");
    GLuint tex = loadTexture("tex1.jpg");

    glm::vec3 cam_pos(0.0, 0.5, 1.0);
    glm::vec3 cam_dir(0.0, 0.0, -1.0);
    glm::vec3 cam_up(0.0, 1.0, 0.0);

    float pitch = 0.0f, yaw = 0.0f;

    GLfloat cam_speed = 0.001f;

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();

        pitch = -89.0f * (mouse_y - 200) / 200;
        yaw = 360.0f * 2 * mouse_x / 400;

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
            cam_pos += cam_dir * cam_speed;
        }
        if (key_status[GLFW_KEY_DOWN])
        {
            cam_pos -= cam_dir * cam_speed;
        }
        if (key_status[GLFW_KEY_LEFT])
        {
            cam_pos -= cam_hand * cam_speed;
        }
        if (key_status[GLFW_KEY_RIGHT])
        {
            cam_pos += cam_hand * cam_speed;
        }

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);

        glActiveTexture(0);
        glBindTexture(GL_TEXTURE0, tex);
        glUniform1i(glGetUniformLocation(shader, "tex"), 0);
        glUniform3f(glGetUniformLocation(shader, "light_int"), 1.0f, 0.0f, 0.0f);
        glUniform3f(glGetUniformLocation(shader, "light_pos"), 0.0f, 1.0f, 0.0f);

        glm::mat4 model(1.0);
        model = glm::translate(model, glm::vec3(-0.1, 0.0, 0.0));

        glm::mat4 view(1.0);

        view = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);

        glm::mat4 projection(1.0);
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);

        glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glDrawArrays(GL_TRIANGLES, 0, vertices.size());

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}