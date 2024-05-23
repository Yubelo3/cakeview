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
#include "Input.hpp"

GLFWwindow *init()
{
    if (!glfwInit())
        exit(-1);
    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "cakeview", NULL, NULL);
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
    // 输入
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                   { glViewport(0, 0, width, height); });
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetScrollCallback(window, scrollCallback);

    // glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    glViewport(0, 0, VIEWPORT_WIDTH, VIEWPORT_HEIGHT); // 视口比窗口小的话，窗口其余部分可以用来显示其他元素
    glClearColor(CLEAR_COLOR[0], CLEAR_COLOR[1], CLEAR_COLOR[2], CLEAR_COLOR[3]);

    // 开启深度测试
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS); // 深度较小时通过测试

    Scene::camera().set(CAMERA_START_E, CAMERA_START_G, CAMERA_START_T);
    Scene::camera().setFov(FOV);
}

// TODO: 重心坐标插值修正
int main(void)
{
    GLFWwindow *window = init();
    setting(window);

    // 配置场景
    // Scene::add(std::make_shared<BoxFace>());
    Scene::add(std::make_shared<Axis>());
    // Scene::add(std::make_shared<UncoloredPlane>(
    //     std::vector<Eigen::Vector3f>{
    //         Eigen::Vector3f{-0.25, -0.25, -0.25},
    //         Eigen::Vector3f{-0.25, -0.25, 0.25},
    //         Eigen::Vector3f{0.25, -0.25, 0.25},
    //         Eigen::Vector3f{0.25, -0.25, -0.25},
    //     }));
    Scene::add(std::make_shared<UntexturedMesh>("../res/models/bunny/bunny.obj", Eigen::Vector3f{0.7, 0.8, 0.9}));
    Scene::add(std::make_shared<Point>(Eigen::Vector3f{0, 0.4, 0.3}, Eigen::Vector3f{0.8, 0.7, 0.6}));
    Scene::add(std::make_shared<Point>(Eigen::Vector3f{0, 0.4, -0.3}, Eigen::Vector3f{0.4, 0.5, 0.8}));
    Scene::add(std::make_shared<PointLight>(Eigen::Vector3f{0, 0.4, 0.3}, Eigen::Vector3f{0.8, 0.7, 0.6}, 0.2f));
    Scene::add(std::make_shared<PointLight>(Eigen::Vector3f{0, 0.4, -0.3}, Eigen::Vector3f{0.4, 0.5, 0.8}, 0.2f));

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        processKeyboard(window);
        MyTimer::update();
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        //=================Rendering loop===================
        Scene::drawAll();
        // Scene::object(0)->setTransform(translate(1.0, 1.0, 0.0));
        // Scene::draw(0);
        // Scene::object(0)->setTransform(translate(0.0, 1.0, 0.0));
        //==================================================

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
