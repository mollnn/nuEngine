#include "shader.h"

GLuint Shader::loadShader(const std::string &vs_name, const std::string &fs_name)
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

Shader::Shader(const std::string &vs_name, const std::string &fs_name)
{
    sp_ = loadShader(vs_name, fs_name);
}

void Shader::use()
{
    glUseProgram(sp_);
}

void Shader::setUniformi(const std::string &name, int x)
{
    glUniform1i(glGetUniformLocation(sp_, name.c_str()), x);
}

void Shader::setUniform(const std::string &name, float x)
{
    glUniform1f(glGetUniformLocation(sp_, name.c_str()), x);
}

void Shader::setUniform(const std::string &name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(sp_, name.c_str()), x, y, z);
}

void Shader::setUniform(const std::string &name, const glm::vec3 &value)
{
    glUniform3fv(glGetUniformLocation(sp_, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setUniform(const std::string &name, const glm::mat4 &value)
{
    glUniformMatrix4fv(glGetUniformLocation(sp_, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setLights(const std::vector<PointLight> &lights)
{
    setUniformi("n_point_light", lights.size());
    for (int i = 0; i < lights.size(); i++)
    {
        auto &light = lights[i];
        setUniform("point_light[" + std::to_string(i) + "].pos", light.position);
        setUniform("point_light[" + std::to_string(i) + "].val", light.intensity);
    }
}

void Shader::setMVP(const glm::mat4 &view, const glm::mat4 &projection)
{
    setUniform("view", view);
    setUniform("projection", projection);
}

void Shader::setCamera(const Camera &camera)
{
    setMVP(camera.view, camera.projection);
    setUniform("camera_pos", camera.cam_pos);
    setUniform("near", camera.near);
    setUniform("far", camera.far);
}

GLuint Shader::id()
{
    return sp_;
}

void Shader::setTexture(const std::string &name, Texture2D *texture)
{
    textures[name] = texture;
}

void Shader::solveTextures()
{
    int idx = 0;
    for (auto &[k, v] : textures)
    {
        glActiveTexture(GL_TEXTURE0 + idx);
        glBindTexture(GL_TEXTURE_2D, v->id());
        setUniformi(k, idx);
        idx++;
    }
}