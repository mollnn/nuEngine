#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

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

struct PointLight
{
    glm::vec3 intensity;
    glm::vec3 position;

    PointLight(const glm::vec3 &i, const glm::vec3 &p) : intensity(i), position(p)
    {
    }
};

struct Camera
{
    glm::vec3 cam_pos;
    glm::vec3 cam_dir;
    glm::vec3 cam_up;

    float pitch = 0.0f, yaw = 0.0f;

    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;

    Camera()
    {
        cam_pos.x = 2;
        onEvents();
    }

    void onEvents()
    {
        static GLfloat last_frame_time = glfwGetTime();
        static GLfloat last_cursor_x = mouse_x, last_cursor_y = mouse_y;

        GLfloat current_time = glfwGetTime();

        GLfloat delta_cursor_x = mouse_x - last_cursor_x;
        GLfloat delta_cursor_y = mouse_y - last_cursor_y;

        last_cursor_x = mouse_x;
        last_cursor_y = mouse_y;

        GLfloat cam_speed = 0.5f;
        GLfloat cam_pan_speed = 0.005f;
        GLfloat cam_rotate_speed = 1.0f;

        if (key_status[GLFW_KEY_LEFT_ALT] && mouse_button_status[0])
        {
            pitch += -89.0f * delta_cursor_y * cam_rotate_speed / 512;
            yaw += 360.0f * delta_cursor_x * cam_rotate_speed / 512;

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

        if (key_status[GLFW_KEY_LEFT_CONTROL] && mouse_button_status[0])
        {
            cam_pos += cam_hand * delta_cursor_x * cam_pan_speed;
            cam_pos -= cam_up * delta_cursor_y * cam_pan_speed;
        }

        if (key_status[GLFW_KEY_LEFT_SHIFT] && mouse_button_status[0])
        {
            cam_pos += cam_hand * delta_cursor_x * cam_pan_speed;
            cam_pos -= cam_dir * delta_cursor_y * cam_pan_speed;
        }

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
        last_frame_time = current_time;

        view = glm::lookAt(cam_pos, cam_pos + cam_dir, cam_up);
        projection = glm::perspective(glm::radians(45.0f), 1.0f, 0.1f, 1000.0f);
    }
};

class Shader
{
    int sp_;

    static GLuint loadShader(const std::string &vs_name, const std::string &fs_name)
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

public:
    Shader(const std::string &vs_name, const std::string &fs_name)
    {
        sp_ = loadShader(vs_name, fs_name);
    }

    void use()
    {
        glUseProgram(sp_);
    }

    void setUniformi(const std::string &name, int x)
    {
        glUniform1i(glGetUniformLocation(sp_, name.c_str()), x);
    }

    void setUniform(const std::string &name, float x)
    {
        glUniform1f(glGetUniformLocation(sp_, name.c_str()), x);
    }

    void setUniform(const std::string &name, float x, float y, float z)
    {
        glUniform3f(glGetUniformLocation(sp_, name.c_str()), x, y, z);
    }

    void setUniform(const std::string &name, const glm::vec3 &value)
    {
        glUniform3fv(glGetUniformLocation(sp_, name.c_str()), 1, glm::value_ptr(value));
    }

    void setUniform(const std::string &name, const glm::mat4 &value)
    {
        glUniformMatrix4fv(glGetUniformLocation(sp_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
    }

    void setLights(const std::vector<PointLight> &lights)
    {
        setUniformi("n_point_light", lights.size());
        for (int i = 0; i < lights.size(); i++)
        {
            auto &light = lights[i];
            setUniform("point_light[" + std::to_string(i) + "].pos", light.position);
            setUniform("point_light[" + std::to_string(i) + "].val", light.intensity);
        }
    }

    void setMVP(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection)
    {
        setUniform("model", model);
        setUniform("view", view);
        setUniform("projection", projection);
    }

    void setCamera(const Camera &camera)
    {
        setMVP(camera.model, camera.view, camera.projection);
    }

    GLuint id()
    {
        return sp_;
    }
};

class Texture
{
    int handle_;
    std::string filename_;

    static GLuint loadTexture(const std::string &tex_name)
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

public:
    Texture(const std::string &tex_name)
    {
        handle_ = loadTexture(tex_name);
        filename_ = tex_name;
    }

    void use(int unit_id)
    {
        glActiveTexture(GL_TEXTURE0 + unit_id);
        glBindTexture(GL_TEXTURE_2D, handle_);
    }
};

class Material
{
    std::map<std::string, Texture *> textures;
    std::map<std::string, int> properties_i;
    std::map<std::string, float> properties_f;
    std::map<std::string, glm::vec3> properties_v3;

    void loadTexturesAssimpType(aiTextureType type, const std::string &type_name, aiMaterial *mat, const std::string &dir = "")
    {
        for (int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString filename;
            mat->GetTexture(type, i, &filename);
            Texture *texture = new Texture(filename.C_Str());
            textures[type_name + std::to_string(i + 1)] = texture;
            std::string flag_name = "usetex" + type_name.substr(7);
            properties_i[flag_name] = 1;
        }
    }

    void loadTexturesAssimp(aiMaterial *mat, const std::string &dir = "")
    {
        float t;
        if (AI_SUCCESS != mat->Get(AI_MATKEY_SHININESS, t))
            t = 1.0f;
        properties_f["shininess"] = t;

        glm::vec3 tv3;
        aiColor3D aitv3;
        if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_DIFFUSE, aitv3))
            tv3 = glm::vec3(0.5f);
        else
            tv3.r = aitv3.r, tv3.g = aitv3.g, tv3.b = aitv3.b;
        properties_v3["color_diffuse"] = tv3;
        properties_i["usetex_diffuse"] = 0;
        if (AI_SUCCESS != mat->Get(AI_MATKEY_COLOR_SPECULAR, aitv3))
            tv3 = glm::vec3(0.5f);
        else
            tv3.r = aitv3.r, tv3.g = aitv3.g, tv3.b = aitv3.b;
        properties_v3["color_specular"] = tv3;
        properties_i["usetex_specular"] = 0;

        loadTexturesAssimpType(aiTextureType_AMBIENT, "texture_ambient", mat, dir);
        loadTexturesAssimpType(aiTextureType_DIFFUSE, "texture_diffuse", mat, dir);
        loadTexturesAssimpType(aiTextureType_NORMALS, "texture_normals", mat, dir);
        loadTexturesAssimpType(aiTextureType_SPECULAR, "texture_specular", mat, dir);
    }

public:
    Material()
    {
    }

    Material(aiMaterial *mat, const std::string &dir = "")
    {
        // load from material file, directly load textures
        loadTexturesAssimp(mat, dir);
    }

    void load(aiMaterial *mat, const std::string &dir = "")
    {
        // load from material file, directly load textures
        loadTexturesAssimp(mat, dir);
    }

    void use(Shader &shader)
    {
        // put textures into shader
        GLuint shader_id = shader.id();
        GLuint texture_unit_id = 4; // first 4 units are reserved
        for (GLuint i = texture_unit_id; i < texture_unit_id + 8; i++)
        {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
        for (auto &[k, v] : textures)
        {
            shader.setUniformi(k, texture_unit_id);
            v->use(texture_unit_id);
            texture_unit_id++;
            std::cout << k << " " << v << std::endl;
        }
        for (auto &[k, v] : properties_f)
        {
            shader.setUniform(k, v);
            std::cout << k << " " << v << std::endl;
        }
        for (auto &[k, v] : properties_i)
        {
            shader.setUniformi(k, v);
            std::cout << k << " " << v << std::endl;
        }
        for (auto &[k, v] : properties_v3)
        {
            shader.setUniform(k, v);
            std::cout << k << " " << v.x << "," << v.y << "," << v.z << std::endl;
        }
        std::cout << std::endl;
    }
};

class Mesh
{
    std::vector<GLfloat> vertices_;
    std::vector<GLuint> indices_;
    Material material_;

    GLuint vao_;
    GLuint vbo_;
    GLuint ebo_;
    int n_;

    void loadMeshAssimp(aiMesh *mesh, const aiScene *scene)
    {
        for (int j = 0; j < mesh->mNumVertices; j++)
        {
            vertices_.push_back(mesh->mVertices[j].x);
            vertices_.push_back(mesh->mVertices[j].y);
            vertices_.push_back(mesh->mVertices[j].z);
            vertices_.push_back(mesh->mNormals[j].x);
            vertices_.push_back(mesh->mNormals[j].y);
            vertices_.push_back(mesh->mNormals[j].z);
            if (mesh->mTextureCoords[0])
            {
                vertices_.push_back(mesh->mTextureCoords[0][j].x);
                vertices_.push_back(mesh->mTextureCoords[0][j].y);
            }
            else
            {
                vertices_.push_back(0);
                vertices_.push_back(0);
            }
        }

        for (int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (int j = 0; j < face.mNumIndices; j++)
            {
                indices_.push_back(face.mIndices[j]);
            }
        }

        material_.load(scene->mMaterials[mesh->mMaterialIndex]);

        n_ = vertices_.size();
        glGenVertexArrays(1, &vao_);
        glGenBuffers(1, &vbo_);
        glGenBuffers(1, &ebo_);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glBufferData(GL_ARRAY_BUFFER, vertices_.size() * sizeof(GLfloat), vertices_.data(), GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_.size() * sizeof(GLuint), indices_.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *)(6 * sizeof(GLfloat)));
        glEnableVertexAttribArray(2);
    }

public:
    Mesh()
    {
    }

    Mesh(aiMesh *mesh, const aiScene *scene)
    {
        loadMeshAssimp(mesh, scene);
    }

    void draw(Shader &shader)
    {
        material_.use(shader);
        glBindVertexArray(vao_);
        glBindBuffer(GL_ARRAY_BUFFER, vbo_);
        glDrawElements(GL_TRIANGLES, n_, GL_UNSIGNED_INT, 0);
    }
};

class Model
{
    std::vector<Mesh> meshes;

    void loadModelAssimp(const std::string &filename)
    {
        Assimp::Importer importer;
        auto scene = importer.ReadFile(filename, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        std::function<void(aiNode *, const aiScene *)> solve = [&](aiNode *node, const aiScene *scene)
        {
            for (int i = 0; i < node->mNumMeshes; i++)
            {
                meshes.push_back(Mesh(scene->mMeshes[node->mMeshes[i]], scene));
            }
            for (int i = 0; i < node->mNumChildren; i++)
            {
                aiNode *child = node->mChildren[i];
                solve(child, scene);
            }
        };
        solve(scene->mRootNode, scene);
    }

public:
    Model()
    {
    }

    Model(const std::string &filename)
    {
        loadModelAssimp(filename);
    }

    void draw(Shader &shader)
    {
        for (auto &i : meshes)
        {
            i.draw(shader);
        }
    }
};

int main()
{
    std::cout << "Nightup Engine v0.1" << std::endl;

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(512, 512, "Nightup Engine v0.1", nullptr, nullptr);
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

    glEnable(GL_DEPTH_TEST);

    Model m0("mitsuba.obj");
    Shader shader("../shader.vs", "../shader.fs");
    Shader shadow_shader("../shadow.vs", "../shadow.fs");
    Camera camera;

    const GLuint SHADOW_MAP_WIDTH = 1024, SHADOW_MAP_HEIGHT = 1024;
    GLuint shadow_map_fbo;
    glGenFramebuffers(1, &shadow_map_fbo);
    GLuint shadow_map_texture;
    glGenTextures(1, &shadow_map_texture);
    glBindTexture(GL_TEXTURE_2D, shadow_map_texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat texture_border_color[] = {1.0, 1.0, 1.0, 1.0};
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, texture_border_color);

    glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadow_map_texture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        camera.onEvents();

        shadow_shader.use();
        glm::mat4 light_view, light_proj, light_vp;
        light_view = glm::lookAt(glm::vec3(0.0f, 10.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0, 0.0, 0.0));
        light_proj = glm::perspective(glm::radians(30.0), 1.0, 1.0, 20.0);
        light_vp = light_proj * light_view;

        shadow_shader.setUniform("lvp", (light_vp));
        shadow_shader.setUniform("model", (camera.model));

        glViewport(0, 0, SHADOW_MAP_WIDTH, SHADOW_MAP_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, shadow_map_fbo);
        glClear(GL_DEPTH_BUFFER_BIT);
        m0.draw(shadow_shader);

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, 512, 512);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setLights({PointLight(glm::vec3(400.0f, 400.0f, 400.0f), glm::vec3(0.0f, 20.0f, 0.0f)),
                          PointLight(glm::vec3(50.0f, 30.1f, 0.1f), glm::vec3(0.5f, 0.3f, 10.0f))});
        shader.setCamera(camera);
        shader.setUniform("lvp", (light_vp));
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, shadow_map_texture);
        shader.setUniformi("cast_shadow", rand() % 2);

        m0.draw(shader);

        glfwSwapBuffers(window);
    }

    glfwTerminate();
    return 0;
}