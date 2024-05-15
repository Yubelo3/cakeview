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
    V3f _e; // pos
    V3f _g; // look at
    V3f _t; // top
    float _fov;
    M4f _O;   // 每次fov变化时计算
    M4f _P;   // 透视变换基本不变，可以存储
    M4f _V;   // 每次相机移动时计算
    M4f _OPV; // 每次相机移动/fov变化时计算

public:
    Camera()
    {
        _P = perspectiveTransform(ZNEAR, ZFAR);
    };

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
        _t = t.normalized();
        _V = viewingTransform(_e, _g, _t);
        _OPV = _O * _P * _V;
    }
    void setFov(float fov)
    {
        _fov = fov;
        _O = orthogonalTransform(ZNEAR, ZFAR, fov, (float)VIEWPORT_WIDTH / VIEWPORT_HEIGHT);
        _OPV = _O * _P * _V;
    }
    inline const M4f &OPV() const
    {
        return _OPV;
    }
};