#pragma once

#include <string>
#include <unordered_map>

#include <collisionTraverser.h>
#include <collisionEntry.h>
#include <collisionNode.h>
#include <collisionRay.h>
#include <collisionHandlerQueue.h>


class Camera;
class NodePath;
class Engine;

class MousePicker {
public:
    MousePicker(const std::string& name, Engine& engine);
	
    void init();
    AsyncTask::DoneStatus update(GenericAsyncTask* task = nullptr, float x = -1.0f, float y = -1.0f);
    void fire_event(const std::string& event);
    NodePath get_first_np();

private:
    std::string _name;
    Engine& _engine;

    PT(CollisionRay) _picker_ray;
    PT(CollisionHandlerQueue) _coll_handler;
    PT(CollisionEntry) _coll_entry;
    CollisionTraverser _traverser;
    NodePath _node;

    float _last_x;
    float _last_y;
};
