#pragma once
#include "Buffers.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "Texture.hpp"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "Time.hpp"

class Renderable
{
protected:
    VertexArray _va;
    ShaderProgram _shader;
    std::vector<Texture> _textures;
    Eigen::Matrix4f _transform = Eigen::Matrix4f::Identity();

protected:
    void _addTextureUniform()
    {
        _shader.bind();
        for (unsigned int i = 0; i < _textures.size(); i++)
        {
            std::string varName = "texture";
            varName += '0' + i;
            _shader.addUniform(varName);
        }
    }
    void _bindTexture()
    {
        _shader.bind();
        for (unsigned int i = 0; i < _textures.size(); i++)
        {
            _textures[i].bind(i);
            std::string varName = "texture";
            varName += '0' + i;
            glUniform1i(_shader.uniform(varName), i);
        }
    }

public:
    void setTransform(const Eigen::Matrix4f &transform)
    {
        _transform = transform;
    }
    virtual void draw(const Eigen::Matrix4f &) = 0;
};

class Axis : public Renderable
{
public:
    Axis()
    {
        std::vector<float> vertices = loadArray<float>("../res/vertex_attributes/Axis.data");
        std::vector<unsigned int> indices = loadArray<unsigned int>("../res/vertex_attributes/AxisIndex.data");

        // VAO
        _va.bind();
        VertexBuffer vb((void *)vertices.data(), vertices.size() * sizeof(float));
        vb.bind();
        IndexBuffer ib(indices.data(), indices.size() * sizeof(unsigned int));
        ib.bind();
        VertexBufferLayout layout;
        layout.addElem(GL_FLOAT, 3, GL_FALSE); // pos
        layout.addElem(GL_FLOAT, 3, GL_FALSE); // color
        layout.bind();

        // Shader
        Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/Axis.vs");
        Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/Axis.fs");
        _shader = ShaderProgram(vertexShader, fragmentShader);
        _shader.bind();
        _shader.addUniform("transform");

        glLineWidth(10.0f);
        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &VP)
    {
        _va.bind();
        _shader.bind();
        Eigen::Matrix4f transform = VP * _transform;
        glUniformMatrix4fv(_shader.uniform("transform"), 1, GL_FALSE, transform.data());
        glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
    }
};

class BoxFace : public Renderable
{
public:
    BoxFace()
    {
        std::vector<float> vertices = loadArray<float>("../res/vertex_attributes/BoxFace.data");
        std::vector<unsigned int> indices = loadArray<unsigned int>("../res/vertex_attributes/BoxFaceIndex.data");

        // 配置va
        _va.bind();
        VertexBuffer vb((void *)vertices.data(), vertices.size() * sizeof(float));
        vb.bind();
        IndexBuffer ib(indices.data(), indices.size() * sizeof(unsigned int));
        ib.bind();
        VertexBufferLayout layout;
        layout.addElem(GL_FLOAT, 3, GL_FALSE); // pos
        layout.addElem(GL_FLOAT, 3, GL_FALSE); // color
        layout.addElem(GL_FLOAT, 2, GL_FALSE); // uv
        layout.bind();

        // 配置shader
        Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/BoxFace.vs");
        Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/BoxFace.fs");
        _shader = ShaderProgram(vertexShader, fragmentShader);
        _shader.bind();
        _shader.addUniform("transform");
        _shader.addUniform("alpha");

        // 配置texture
        _textures.push_back(Texture("../res/img/container.jpg", GL_RGB));
        _textures.push_back(Texture("../res/img/awesomeface.png", GL_RGBA));
        _addTextureUniform();

        // 解绑
        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &VP)
    {
        _va.bind();
        _shader.bind();

        float timeValue = MyTimer::now();
        float alpha = sin(timeValue) / 2.0f + 0.5f;
        glUniform1f(_shader.uniform("alpha"), alpha);
        Eigen::Matrix4f transform = VP * _transform;
        glUniformMatrix4fv(_shader.uniform("transform"), 1, GL_FALSE, transform.data());

        _bindTexture();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 后两个参数分别为索引类型和IBO偏移量
    }
};