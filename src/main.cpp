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
#include "Scene.hpp"
#include "Camera.hpp"

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

void setCamera(Camera &camera)
{
    static const Eigen::Vector3f startE{2, 2, 2};
    static const Eigen::Vector3f target{0, 0, 0};

    float time = MyTimer::now();

    Eigen::Vector3f e = rotateAroundY(time).block<3, 3>(0, 0) * startE;
    Eigen::Vector3f g = (target - e).normalized();
    Eigen::Vector3f t{0, 1, 0};

    camera.set(e, g, t);
}

// TODO: 重心坐标插值修正
int main(void)
{
    GLFWwindow *window = init();
    setting(window);

    // 配置场景
    Scene scene = Camera();
    scene.add(std::make_shared<BoxFace>());
    scene.add(std::make_shared<Axis>());
    scene.add(std::make_shared<UncoloredPlane>(
        std::vector<Eigen::Vector3f>{
        Eigen::Vector3f{-3, -1, -3},
        Eigen::Vector3f{-3, -1, 3},
        Eigen::Vector3f{3, -1, 3},
        Eigen::Vector3f{3, -1, -3},
    }));

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // 深度较小时通过测试

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

        //=================Rendering loop===================
        MyTimer::update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setCamera(scene.camera());

        scene.drawAll();
        scene.object(0)->setTransform(translate(1.0, 1.0, 0.0));
        scene.draw(0);
        scene.object(0)->setTransform(translate(0.0, 1.0, 0.0));
        //==================================================

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
