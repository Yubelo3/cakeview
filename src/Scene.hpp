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

public:
    Scene(const Camera &camera) : _camera(camera){};

public:
    inline Camera &camera()
    {
        return _camera;
    }
    inline unsigned int count() const
    {
        return _objects.size();
    }
    inline const ObjPtr object(unsigned int i) const
    {
        return _objects[i];
    }
    void add(ObjPtr object)
    {
        _objects.push_back(object);
    }
    void draw(unsigned int i)
    {
        _objects[i]->draw(_camera.VP());
    }
    void drawAll()
    {
        for(const auto& p:_objects)
            p->draw(_camera.VP());
    }
};