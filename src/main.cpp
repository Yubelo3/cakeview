#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <assert.h>
#include <cmath>
#include "config.h"
#include "Buffers.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "Math.hpp"
#include "Time.hpp"

GLFWwindow *init()
{
    if (!glfwInit())
        exit(-1);
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        exit(-1);
    }
    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    // glewInit() must be called after get a valid context!
    if (glewInit() != GLEW_OK)
        std::cout << "GLEW NOT OK!" << std::endl;
    std::cout << glGetString(GL_VERSION) << std::endl;
    return window;
}

void setting(GLFWwindow *window)
{
    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); // 视口比窗口小的话，窗口其余部分可以用来显示其他元素
    glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], CLEAR_COLOR[3]);
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });
    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
}

template <typename T>
std::vector<T> loadArray(const char *filepath)
{
    std::ifstream ifs(filepath);
    if (!ifs.is_open())
    {
        std::cerr << "Failed to open " << filepath << std::endl;
        exit(-1);
    }
    std::vector<T> data;

    T tmp;
    while (ifs >> tmp)
        data.push_back(tmp);
    return data;
}

Eigen::Matrix4f getCameraTransform()
{
    static const Eigen::Vector3f startE{1, 2, 1};
    static const Eigen::Vector3f target{0, 0, 0};

    float time = MyTimer::now();

    Eigen::Vector3f e = rotateAroundY(time).block<3, 3>(0, 0) * startE;
    Eigen::Vector3f g = (target - e).normalized();
    Eigen::Vector3f t{0, 1, 0};
    t -= t.dot(g.normalized()) * g;

    Eigen::Matrix4f V = viewingTransform(e, g, t);
    return V;
}

Eigen::Matrix4f getPerspectiveTransform()
{
    Eigen::Matrix4f P = perspectiveTransform(-0.1, -50);
    Eigen::Matrix4f O = orthogonalTransform(-0.1, -50, M_PI / 3.0, (float)VIEWPORT_WIDTH / VIEWPORT_HEIGHT);
    return O * P;
}

Eigen::Matrix4f getTransform()
{
    static const Eigen::Matrix4f P = getPerspectiveTransform();
    Eigen::Matrix4f V = getCameraTransform();
    return P * V;
}

std::pair<VertexArray,ShaderProgram> setObject()
{
    // 配置VAO
    std::vector<float> vertices = loadArray<float>("../res/vertex_attributes/vert.data");
    std::vector<unsigned int> indices = loadArray<unsigned int>("../res/vertex_attributes/index.data");

    VertexArray va;
    va.bind();
    VertexBuffer vb((void *)vertices.data(), vertices.size() * sizeof(float));
    vb.bind();
    IndexBuffer ib(indices.data(), indices.size() * sizeof(unsigned int));
    ib.bind();
    VertexBufferLayout layout;
    layout.addElem(GL_FLOAT, 3, GL_FALSE); // pos
    layout.addElem(GL_FLOAT, 3, GL_FALSE); // color
    layout.addElem(GL_FLOAT, 2, GL_FALSE); // uv
    layout.bind();
    va.unbind();

    // 配置shader
    Shader vertexShader(GL_VERTEX_SHADER, "../res/vertex_shader/vertex_shader.vs");
    Shader fragmentShader(GL_FRAGMENT_SHADER, "../res/fragment_shader/fragment_shader.fs");
    ShaderProgram shaderProgram(vertexShader, fragmentShader);
    shaderProgram.unbind();

    return {va,shaderProgram};
}

std::pair<VertexArray,ShaderProgram> setAxis()
{
    // 配置坐标轴VAO和shader
    float axisPts[] = {
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        0.0,
        2.0,
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        0.0,
        2.0,
        0.0,
        0.0,
        1.0,
        0.0,
        0.0,
        0.0,
        2.0,
        0.0,
        0.0,
        1.0,
    };
    unsigned int axisIdx[] = {
        0, 1, 0, 2, 0, 3};
    VertexArray axisVa;
    axisVa.bind();
    VertexBuffer axisVb((void *)axisPts, sizeof(axisPts));
    axisVb.bind();
    IndexBuffer axisIb(axisIdx, sizeof(axisIdx));
    axisIb.bind();
    VertexBufferLayout axisLayout;
    axisLayout.addElem(GL_FLOAT, 3, false);
    axisLayout.addElem(GL_FLOAT, 3, false);
    axisLayout.bind();
    axisVa.unbind();
    Shader vertexShaderLine(GL_VERTEX_SHADER, "../res/vertex_shader/line.vs");
    Shader fragmentShaderLine(GL_FRAGMENT_SHADER, "../res/fragment_shader/line.fs");
    ShaderProgram shaderProgramLine(vertexShaderLine, fragmentShaderLine);
    shaderProgramLine.unbind();

    return {axisVa,shaderProgramLine};
}

int main(void)
{
    GLFWwindow *window = init();
    setting(window);

    // 配置
    auto[va,shaderProgram]=setObject();
    auto[axisVa,shaderProgramLine]=setAxis();

    // 配置纹理
    Texture texture0("../res/container.jpg", GL_RGB);
    Texture texture1("../res/awesomeface.png", GL_RGBA);

    // 准备绘制
    va.bind();
    texture0.bind(0);
    texture1.bind(1);
    shaderProgram.bind();

    // 配置uniform变量，这个必须在绑定shader之后
    shaderProgram.bind();
    shaderProgram.addUniform("alpha");
    shaderProgram.addUniform("texture0");
    glUniform1i(shaderProgram.uniform("texture0"), 0); // 指定纹理slot
    shaderProgram.addUniform("texture1");
    glUniform1i(shaderProgram.uniform("texture1"), 1);
    shaderProgram.addUniform("transform");

    // 配置坐标轴绘制
    shaderProgramLine.bind();
    shaderProgramLine.addUniform("transform");
    shaderProgramLine.unbind();
    glLineWidth(10.0f);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //=================Rendering loop===================
        MyTimer::update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Eigen::Matrix4f transform = getTransform();

        va.bind();
        shaderProgram.bind();
        float timeValue = MyTimer::now();
        float alpha = sin(timeValue) / 2.0f + 0.5f;
        glUniform1f(shaderProgram.uniform("alpha"), alpha);
        glUniformMatrix4fv(shaderProgram.uniform("transform"), 1, GL_FALSE, transform.data());
        // glDrawArrays(GL_TRIANGLES,0,3);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 后两个参数分别为索引类型和IBO偏移量

        axisVa.bind();
        shaderProgramLine.bind();
        glUniformMatrix4fv(shaderProgramLine.uniform("transform"), 1, GL_FALSE, transform.data());
        glDrawElements(GL_LINES, 6, GL_UNSIGNED_INT, 0);
        //==================================================

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
