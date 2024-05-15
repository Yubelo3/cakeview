#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include <iostream>

Eigen::Matrix4f translate(float x, float y, float z)
{
    Eigen::Matrix4f T;
    T << 1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1;
    return T;
}

Eigen::Matrix4f rotateAroundX(float radian)
{
    float c = cos(radian), s = sin(radian);
    Eigen::Matrix4f R;
    R << 1, 0, 0, 0,
        0, c, -s, 0,
        0, s, c, 0,
        0, 0, 0, 1;
    return R;
}

Eigen::Matrix4f rotateAroundY(float radian)
{
    float c = cos(radian), s = sin(radian);
    Eigen::Matrix4f R;
    R << c, 0, s, 0,
        0, 1, 0, 0,
        -s, 0, c, 0,
        0, 0, 0, 1;
    return R;
}

Eigen::Matrix4f rotateAroundZ(float radian)
{
    float c = cos(radian), s = sin(radian);
    Eigen::Matrix4f R;
    R << c, -s, 0, 0,
        s, c, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1;
    return R;
}

// world to camera
Eigen::Matrix4f viewingTransform(Eigen::Vector3f e, Eigen::Vector3f g, Eigen::Vector3f t)
{
    g.normalize();
    t.normalize();
    Eigen::Vector3f gCrossT = g.cross(t);

    Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
    T.block<3, 1>(0, 3) = -e;

    Eigen::Matrix4f R = Eigen::Matrix4f::Identity();
    R.block<1, 3>(0, 0) = gCrossT.transpose();
    R.block<1, 3>(1, 0) = t.transpose();
    R.block<1, 3>(2, 0) = (-g).transpose();

    return R * T;
}

// viewing frustum to cuboid
// n and f should be negative
Eigen::Matrix4f perspectiveTransform(float n, float f)
{
    Eigen::Matrix4f P;
    P << n, 0, 0, 0,
        0, n, 0, 0,
        0, 0, n + f, -n * f,
        0, 0, 1, 0;
    return P;
}

// cuboid coord to NDC
Eigen::Matrix4f orthogonalTransform(float n, float f, float fov, float aspect_ratio)
{
    float t = (-n) * tan(fov / 2);
    float r = aspect_ratio * t;

    Eigen::Matrix4f T = Eigen::Matrix4f::Identity();
    T(2, 3) = -(n + f) / 2;

    Eigen::Matrix4f S = Eigen::Matrix4f::Identity();
    S(0, 0) = 1 / r;
    S(1, 1) = 1 / t;
    S(2, 2) = 2 / (n - f);

    // 现在的投影矩阵将[-50,-0.1]映射到[-1,1]
    // OpenGL的深度是[0,1]，且越小越近，所以需要把[-1,1]映射到[1,0]
    // 即先反转再缩放再平移
    Eigen::Matrix4f R = Eigen::Matrix4f::Identity();
    R(2, 2) = -0.5; // 翻转+缩放
    R(2, 3) = 0.5;  // 平移

    return R * S * T;
}
