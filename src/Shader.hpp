#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <map>

class Shader
{
private:
    GLuint _id;
    std::string _source;

private:
    void _loadShaderSource(const char *filepath)
    {
        std::ifstream ifs(filepath);
        if (!ifs.is_open())
        {
            std::cerr << "Failed to open file " << filepath << std::endl;
            exit(-1);
        }
        std::stringstream ss;
        ss << ifs.rdbuf();
        ifs.close();
        _source = ss.str();
    }

public:
    Shader(GLenum shaderType, const char *filepath)
    {
        _id = glCreateShader(shaderType);
        _loadShaderSource(filepath);
        const char *source = _source.c_str();
        glShaderSource(_id, 1, &source, nullptr);
        glCompileShader(_id);

        int success;
        glGetShaderiv(_id, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(_id, 512, nullptr, infoLog);
            std::cerr << "Failed to compile shader\n";
            std::cerr << infoLog << std::endl;
            exit(-1);
        }
    }

public:
    inline unsigned int id() const
    {
        return _id;
    }
    void release()
    {
        glDeleteShader(_id);
        _id = 0;
    }
};

class ShaderProgram
{
private:
    unsigned int _id;
    std::map<std::string, int> _uniforms;

public:
    ShaderProgram(){};
    ShaderProgram(Shader vertexShader, Shader fragmentShader)
    {
        _id = glCreateProgram();
        glAttachShader(_id, vertexShader.id());
        glAttachShader(_id, fragmentShader.id());
        glLinkProgram(_id);

        int success;
        glGetProgramiv(_id, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(_id, 512, nullptr, infoLog);
            std::cerr << "Failed to link program\n";
            std::cerr << infoLog << std::endl;
            exit(-1);
        }
        vertexShader.release();
        fragmentShader.release();
    }

public:
    void bind() const
    {
        glUseProgram(_id);
    }
    void unbind() const
    {
        glUseProgram(0);
    }
    inline unsigned int id() const
    {
        return _id;
    }
    int uniform(const std::string &name)
    {
        return glGetUniformLocation(id(),name.c_str());
    }
};