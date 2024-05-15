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
#include "Renderable.hpp"

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

int main(void)
{
    GLFWwindow *window = init();
    setting(window);

    // 配置
    BoxFace boxFace;
    Axis axis;

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
        boxFace.draw(transform);
        axis.draw(transform);
        //==================================================

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
