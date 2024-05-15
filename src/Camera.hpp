#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>
#include "Math.hpp"
#include "config.h"
#include "Time.hpp"

class Camera
{
    using V3f = Eigen::Vector3f;
    using M4f = Eigen::Matrix4f;

private:
    V3f _e;  // pos
    V3f _g;  // look at
    V3f _t;  // top
    M4f _P;  // 透视变换基本不变，可以存储
    M4f _VP; // 每次移动相机时计算

private:
    void _computeP()
    {
        Eigen::Matrix4f P = perspectiveTransform(ZNEAR, ZFAR);
        Eigen::Matrix4f O = orthogonalTransform(ZNEAR, ZFAR, FOV, (float)VIEWPORT_WIDTH / VIEWPORT_HEIGHT);
        _P = O * P;
    }

public:
    Camera()
    {
        _computeP();
    }
    Camera(const V3f &e, const V3f &g, const V3f &t)
    {
        _computeP();
        set(e, g, t);
    }

public:
    inline const V3f &e() const
    {
        return _e;
    }
    inline const V3f &g() const
    {
        return _g;
    }
    inline const V3f &t() const
    {
        return _t;
    }
    void set(const V3f &e, const V3f &g, const V3f &t)
    {
        _e = e;
        _g = g.normalized();
        _t = t - t.dot(_g) * _g;
        _t.normalize();
        _VP = _P * viewingTransform(_e, _g, _t);
    }
    inline const M4f &VP() const
    {
        return _VP;
    }
};