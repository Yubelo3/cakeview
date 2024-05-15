#pragma once
#include <GLFW/glfw3.h>

class MyTimer
{
private:
    float _lastTime = 0;
    float _nowTime = glfwGetTime();
    static MyTimer _instance;

private:
    MyTimer(){};
public:
    MyTimer(const MyTimer &) = delete;

public:
    inline static MyTimer& get()
    {
        return _instance;
    }
    static float deltaTime()
    {
        return get()._lastTime-get()._nowTime;
    }
    static void update()
    {
        get()._lastTime=get()._nowTime;
        get()._nowTime=glfwGetTime();
    }
    static float now()
    {
        return get()._nowTime;
    }
};

MyTimer MyTimer::_instance;