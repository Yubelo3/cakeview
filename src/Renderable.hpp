#pragma once
#include "Buffers.hpp"
#include "Shader.hpp"
#include "Utils.hpp"
#include "Texture.hpp"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "Time.hpp"
#include "OBJ_Loader.h"
#include "Light.hpp"
#include <memory>

class Renderable
{
protected:
    VertexArray _va;
    ShaderProgram _shader;
    std::vector<Texture> _textures;
    Eigen::Matrix4f _transform = Eigen::Matrix4f::Identity();

protected:
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
    virtual void setLights(const std::vector<std::shared_ptr<Light>> &lights)
    {
        // Do nothing by default
    }
    virtual void draw(const Eigen::Matrix4f &) = 0;
};

class Point : public Renderable
{
public:
    Point(const Eigen::Vector3f &p, const Eigen::Vector3f &color)
    {
        glPointSize(10.0f);
        _va.bind();
        float data[] = {p[0], p[1], p[2], color[0], color[1], color[2]};
        VertexBuffer vb(p.data(), sizeof(float) * 6);
        vb.bind();
        VertexBufferLayout layout;
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.bind();
        Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/Point.vs");
        Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/Point.fs");
        _shader = ShaderProgram(vertexShader, fragmentShader);

        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &_OPV)
    {
        _va.bind();
        _shader.bind();
        glUniformMatrix4fv(_shader.uniform("uni_transform"), 1, GL_FALSE, _OPV.data());
        glDrawArrays(GL_POINTS, 0, 1);
    }
};

class UntexturedMesh : public Renderable
{
private:
    unsigned int _count = 0;
    Eigen::Vector3f _color;
    const std::vector<std::shared_ptr<Light>> *_lights = nullptr;

public:
    UntexturedMesh(const char *filepath, const Eigen::Vector3f &color)
    {
        _color = color;
        objl::Loader loader;
        if (!loader.LoadFile(filepath))
        {
            std::cerr << "Failed to load mesh file" << std::endl;
            exit(-1);
        }

        std::vector<float> data;
        for (const objl::Mesh &m : loader.LoadedMeshes)
        {
            for (int i = 0; i < m.Vertices.size(); i += 3)
            {
                static auto cvt = [](const objl::Vector3 &v) -> Eigen::Vector3f
                {
                    float x = v.X, y = v.Y, z = v.Z;
                    return Eigen::Vector3f{x, y, z};
                };
                static auto psh = [&data](const Eigen::Vector3f &v, const Eigen::Vector3f &n)
                {
                    data.push_back(v[0]);
                    data.push_back(v[1]);
                    data.push_back(v[2]);
                    data.push_back(n[0]);
                    data.push_back(n[1]);
                    data.push_back(n[2]);
                };
                Eigen::Vector3f v0 = cvt(m.Vertices[i].Position);
                Eigen::Vector3f v1 = cvt(m.Vertices[i + 1].Position);
                Eigen::Vector3f v2 = cvt(m.Vertices[i + 2].Position);
                Eigen::Vector3f normal = ((v1 - v0).cross(v2 - v0)).normalized();
                psh(v0, normal);
                psh(v1, normal);
                psh(v2, normal);
            }
        }
        _count = data.size() / 6;

        _va.bind();
        VertexBuffer vb(data.data(), data.size() * sizeof(float));
        vb.bind();
        VertexBufferLayout layout;
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.bind();

        Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/Phong.vs");
        Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/Phong.fs");
        _shader = ShaderProgram(vertexShader, fragmentShader);
        _shader.bind();

        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &OPV)
    {
        _va.bind();
        _shader.bind();
        Eigen::Matrix4f transform = OPV * _transform;
        glUniformMatrix4fv(_shader.uniform("uni_transform"), 1, GL_FALSE, transform.data());
        glUniformMatrix4fv(_shader.uniform("uni_modelTransform"), 1, GL_FALSE, _transform.data());
        if (_lights)
        {
            glUniform1i(_shader.uniform("uni_nlights"), _lights->size());
            for (unsigned int i = 0; i < _lights->size(); i++)
            {
                const std::shared_ptr<Light> &l = (*_lights)[i];
                float I = l->intensity();
                std::string lightName = "uni_lights[";
                lightName += '0' + i;
                lightName += ']';
                glUniform3f(_shader.uniform(lightName + ".pos"), l->pos()[0], l->pos()[1], l->pos()[2]);
                glUniform3f(_shader.uniform(lightName + ".color"), l->color()[0] * I, l->color()[1] * I, l->color()[2] * I);
            }
        }
        glDrawArrays(GL_TRIANGLES, 0, _count);
    }
    void setLights(const std::vector<std::shared_ptr<Light>> &lights)
    {
        _lights = &lights;
    }
};

class Mesh : public Renderable
{
private:
    unsigned int _count = 0;
    const std::vector<std::shared_ptr<Light>> *_lights = nullptr;

public:
    Mesh(const char *filepath)
    {
        objl::Loader loader;
        if (!loader.LoadFile(filepath))
        {
            std::cerr << "Failed to load mesh file" << std::endl;
            exit(-1);
        }

        std::vector<float> data;
        for (const objl::Mesh &m : loader.LoadedMeshes)
        {
            for (int i = 0; i < m.Vertices.size(); i += 3)
            {
                static auto cvt = [](const objl::Vector3 &v) -> Eigen::Vector3f
                {
                    float x = v.X, y = v.Y, z = v.Z;
                    return Eigen::Vector3f{x, y, z};
                };
                static auto psh = [&data](const Eigen::Vector3f &v, const Eigen::Vector3f &n)
                {
                    data.push_back(v[0]);
                    data.push_back(v[1]);
                    data.push_back(v[2]);
                    data.push_back(n[0]);
                    data.push_back(n[1]);
                    data.push_back(n[2]);
                };
                Eigen::Vector3f v0 = cvt(m.Vertices[i].Position);
                Eigen::Vector3f v1 = cvt(m.Vertices[i + 1].Position);
                Eigen::Vector3f v2 = cvt(m.Vertices[i + 2].Position);
                Eigen::Vector3f normal = ((v1 - v0).cross(v2 - v0)).normalized();
                psh(v0, normal);
                psh(v1, normal);
                psh(v2, normal);
            }
        }
        _count = data.size() / 6;

        _va.bind();
        VertexBuffer vb(data.data(), data.size() * sizeof(float));
        vb.bind();
        VertexBufferLayout layout;
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.bind();

        Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/Phong.vs");
        Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/Phong.fs");
        _shader = ShaderProgram(vertexShader, fragmentShader);
        _shader.bind();

        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &OPV)
    {
        _va.bind();
        _shader.bind();
        Eigen::Matrix4f transform = OPV * _transform;
        glUniformMatrix4fv(_shader.uniform("uni_transform"), 1, GL_FALSE, transform.data());
        glUniformMatrix4fv(_shader.uniform("uni_modelTransform"), 1, GL_FALSE, _transform.data());
        if (_lights)
        {
            glUniform1i(_shader.uniform("uni_nlights"), _lights->size());
            for (unsigned int i = 0; i < _lights->size(); i++)
            {
                const std::shared_ptr<Light> &l = (*_lights)[i];
                float I = l->intensity();
                std::string lightName = "uni_lights[";
                lightName += '0' + i;
                lightName += ']';
                glUniform3f(_shader.uniform(lightName + ".pos"), l->pos()[0], l->pos()[1], l->pos()[2]);
                glUniform3f(_shader.uniform(lightName + ".color"), l->color()[0] * I, l->color()[1] * I, l->color()[2] * I);
            }
        }
        glDrawArrays(GL_TRIANGLES, 0, _count);
    }
    void setLights(const std::vector<std::shared_ptr<Light>> &lights)
    {
        _lights = &lights;
    }
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

        glLineWidth(3.0f);
        _va.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &OPV)
    {
        _va.bind();
        _shader.bind();
        Eigen::Matrix4f transform = OPV * _transform;
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
        // 配置texture
        _textures.push_back(Texture("../res/img/container.jpg", GL_RGB));
        _textures.push_back(Texture("../res/img/awesomeface.png", GL_RGBA));

        // 解绑
        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &OPV)
    {
        _va.bind();
        _shader.bind();

        float timeValue = MyTimer::now();
        float alpha = sin(timeValue) / 2.0f + 0.5f;
        glUniform1f(_shader.uniform("alpha"), alpha);
        Eigen::Matrix4f transform = OPV * _transform;
        glUniformMatrix4fv(_shader.uniform("transform"), 1, GL_FALSE, transform.data());

        _bindTexture();

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 后两个参数分别为索引类型和IBO偏移量
    }
};

class UncoloredPlane : public Renderable
{
private:
    unsigned int _count = 0;

public:
    UncoloredPlane(const std::vector<Eigen::Vector3f> &vertices)
    {
        _count = (vertices.size() - 2) * 3;
        _va.bind();
        std::vector<float> data;
        data.reserve(vertices.size() * 3);
        for (const auto &v : vertices)
            for (int i : {0, 1, 2})
                data.push_back(v[i]);
        VertexBuffer vb(data.data(), data.size() * sizeof(float));
        vb.bind();
        std::vector<unsigned int> index;
        for (unsigned int i = 1; i < vertices.size() - 1; i++)
        {
            index.push_back(0);
            index.push_back(i);
            index.push_back(i + 1);
        }
        IndexBuffer ib(index.data(), index.size() * sizeof(unsigned int));
        ib.bind();
        VertexBufferLayout layout;
        layout.addElem(GL_FLOAT, 3, GL_FALSE);
        layout.bind();

        Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/Plane.vs");
        Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/UncoloredPlane.fs");
        _shader = ShaderProgram(vertexShader, fragmentShader);

        _va.unbind();
        _shader.unbind();
    }

public:
    void draw(const Eigen::Matrix4f &OPV)
    {
        _va.bind();
        _shader.bind();
        Eigen::Matrix4f transform = _transform * OPV;
        glUniformMatrix4fv(_shader.uniform("transform"), 1, GL_FALSE, transform.data());
        glDrawElements(GL_TRIANGLES, _count, GL_UNSIGNED_INT, 0);
    }
};