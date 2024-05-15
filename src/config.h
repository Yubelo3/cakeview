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