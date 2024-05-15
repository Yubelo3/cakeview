#pragma once
#include <GLFW/glfw3.h>
#include "eigen3/Eigen/Core"
#include "eigen3/Eigen/Dense"
#include "config.h"
#include "Math.hpp"
#include "Scene.hpp"

void mouseCallback(GLFWwindow *window, double x, double y)
{
    // x左小右大，y上小下大
    static bool firstEnter = true;
    static float lastX, lastY;
    static float yaw = 0.0, pitch = 0.0; // 初始面朝-Z

    if (firstEnter)
    {
        firstEnter = false;
        lastX = x;
        lastY = y;
    }

    float offsetX = (x - lastX), offsetY = y - lastY;
    lastX = x, lastY = y;
    offsetX *= MOUSE_SENSITIVITY;
    offsetY *= MOUSE_SENSITIVITY;

    // 规定：xyz旋转顺序，均为逆时针，欧拉角(0,0,0)时朝向(0,0,-1)，上方向为(0,1,0)
    // 即朝向为Rz(Roll)Ry(Yaw)Rx(Pitch)[0,0,-1]
    // 鼠标向左时yaw增加，鼠标向上时pitch增加

    yaw -= offsetX;
    pitch -= offsetY;
    pitch = std::clamp((double)pitch, -M_PI / 2.0 + 0.001, M_PI / 2.0 - 0.001);
    Eigen::Matrix3f transform = rotateAroundY(yaw).block<3, 3>(0, 0) * rotateAroundX(pitch).block<3, 3>(0, 0);
    Eigen::Vector3f newG = transform * CAMERA_START_G;
    Eigen::Vector3f newT = transform * CAMERA_START_T;

    Camera &cam = Scene::camera();
    cam.set(cam.e(), newG, newT);
}

void scrollCallback(GLFWwindow *window, double offsetX, double offsetY)
{
    // 应该只有offsetY有用，滚轮上滚是1，下滚是-1
    static float fov = FOV;

    fov += offsetY * ZOOM_SPEED * -1;
    fov = std::clamp((double)fov, M_PI / 9.0, M_PI / 1.5);
    Scene::camera().setFov(fov);
}

void processKeyboard(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    Camera &cam = Scene::camera();
    Eigen::Vector3f dir = Eigen::Vector3f::Zero();
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        dir = cam.g();
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        dir = -cam.g();
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        dir = cam.t().cross(cam.g());
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        dir = cam.g().cross(cam.t());
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        dir = {0, 1, 0};
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        dir = {0, -1, 0};
    Eigen::Vector3f newPos = cam.e() + dir * CAMERA_SPEED * MyTimer::deltaTime();
    cam.set(newPos, cam.g(), cam.t());
}