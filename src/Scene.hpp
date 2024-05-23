#pragma once
#include "Camera.hpp"
#include "Renderable.hpp"
#include "Light.hpp"
#include <memory>

class Scene
{
    using ObjPtr = std::shared_ptr<Renderable>;
    using LightPtr = std::shared_ptr<Light>;

private:
    std::vector<ObjPtr> _objects;
    std::vector<LightPtr> _lights;
    Camera _camera;
    static Scene _instance;
    Scene(){};

public:
    Scene(const Scene &) = delete;

public:
    inline static Scene &get()
    {
        return _instance;
    }
    static void setCamera(const Camera &camera)
    {
        get()._camera = camera;
    }
    inline static Camera &camera()
    {
        return get()._camera;
    }
    inline static unsigned int count()
    {
        return get()._objects.size();
    }
    inline static const ObjPtr object(unsigned int i)
    {
        return get()._objects[i];
    }
    void static add(ObjPtr object)
    {
        get()._objects.push_back(object);
    }
    void static add(LightPtr light)
    {
        get()._lights.push_back(light);
    }
    void static draw(unsigned int i)
    {
        get()._objects[i]->setLights(get()._lights);
        get()._objects[i]->draw(get()._camera.OPV());
    }
    void static drawAll()
    {
        for (int i = 0; i < get()._objects.size(); i++)
            draw(i);
    }
};

Scene Scene::_instance;