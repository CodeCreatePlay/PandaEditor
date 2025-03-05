#pragma once

#include <string>
#include <unordered_map>

#include <collisionRay.h>
#include <collisionNode.h>
#include <collisionEntry.h>
#include <collisionHandlerQueue.h>
#include <collisionTraverser.h>

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

    NodePath _node;

    PT(CollisionRay) _picker_ray;
	PT(CollisionEntry) _coll_entry;
    PT(CollisionHandlerQueue) _coll_handler;
    CollisionTraverser _traverser;
	
    float _last_x;
    float _last_y;
};
