#pragma once
#include <eigen3/Eigen/Core>
const int WINDOW_WIDTH = 1280;
const int WINDOW_HEIGHT = 960;
const int VIEWPORT_WIDTH = 1280;
const int VIEWPORT_HEIGHT = 960;
const Eigen::Vector4f CLEAR_COLOR{0.2f, 0.2f, 0.2f, 1.0f};
const float ZNEAR = -0.1f;
const float ZFAR = -50.0f;
const float FOV = M_PI / 3.0f;
const float CAMERA_SPEED = 1.5f;
const float MOUSE_SENSITIVITY = 0.0005f;
const float ZOOM_SPEED = 0.1f;

const Eigen::Vector3f CAMERA_START_E{0.0f, 0.0f, 1.0f};
const Eigen::Vector3f CAMERA_START_G{0.0f, 0.0f, -1.0f};
const Eigen::Vector3f CAMERA_START_T{0.0f, 1.0f, 0.0f};