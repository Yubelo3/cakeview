#pragma once
#include <eigen3/Eigen/Core>
#include <eigen3/Eigen/Dense>

class Light
{
    using V3f=Eigen::Vector3f;

public:
    enum class LightType
    {
        POINT_LIGHT=0,
        PARALLEL_LIGHT=1,
    };    

protected:
    LightType _type;
    V3f _pos;
    V3f _color;
    V3f _dir;
    float _intensity;

public:
    inline LightType type() const
    {
        return _type;
    }
    inline const V3f& pos() const
    {
        return _pos;
    }
    inline const V3f& color() const
    {
        return _color;
    }
    inline float intensity() const
    {
        return _intensity;
    }
    inline void setPos(const V3f& pos)
    {
        _pos=pos;
    }
    inline void setColor(const V3f& color)
    {
        _color=color;
    }
    inline void setIntensity(float intensity)
    {
        _intensity=intensity;
    }
};

class PointLight:public Light
{
    using V3f=Eigen::Vector3f;

public:
    PointLight(const V3f& pos,const V3f& color,float intensity)
    {
        _type=Light::LightType::POINT_LIGHT;
        setPos(pos);
        setColor(color);
        setIntensity(intensity);
    }
};

class ParallelLight:public Light
{
    using V3f=Eigen::Vector3f;

public:
    ParallelLight(const V3f& color,float intensity)
    {
        _type=Light::LightType::PARALLEL_LIGHT;
        setColor(color);
        setIntensity(intensity);
    }
};