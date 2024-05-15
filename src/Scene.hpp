#pragma once
#include "Camera.hpp"
#include "Renderable.hpp"
#include <memory>

class Scene
{
    using ObjPtr = std::shared_ptr<Renderable>;

private:
    std::vector<ObjPtr> _objects;
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
    void static draw(unsigned int i)
    {
        get()._objects[i]->draw(get()._camera.OPV());
    }
    void static drawAll()
    {
        for (const auto &p : get()._objects)
            p->draw(get()._camera.OPV());
    }
};

Scene Scene::_instance;