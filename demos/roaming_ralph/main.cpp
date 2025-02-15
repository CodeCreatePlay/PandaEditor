#include <string>
#include <unordered_map>

#include <clockObject.h>
#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <lpoint3.h>
#include <nodePath.h>
#include <texturePool.h>
#include <collisionRay.h>
#include <collisionNode.h>
#include <collisionTraverser.h>
#include <collisionHandlerQueue.h>
#include <collideMask.h>

#include "demon.hpp"
#include "taskUtils.hpp"
#include "characterController.cpp"
#include "characterCollisionHandler.cpp"
#include "cameraController.cpp"
#include "cameraCollisionHandler.cpp"


static const std::string ASSETS_PATH = "demos/_assets";

class RoamingRalphDemo {
public:
    RoamingRalphDemo() 
        : demon(Demon::get_instance()),
          camera_controller(ralph, camera),
          camera_collision_handler(ralph, c_trav),
          character_controller(ralph),
          character_collision_handler(ralph, c_trav)
    {
        camera = demon.game.main_cam;

        // load stuff
        load_world();
        load_actor();
        std::vector<NodePath> anims = load_actor_anims();
        LPoint3 start_pos = environment.find("**/start_point").get_pos();

        // Take ralph to the starting position
        ralph.set_pos(start_pos);

        // Initialize
        character_controller.init(anims);
        character_collision_handler.init(start_pos);
        camera_controller.init();
        camera_collision_handler.init();
        
        // Create a key map and register keys to their corresponding events
        key_map = 
        {
            {"left",      false},
            {"right",     false},
            {"forward",   false},
            {"cam-left",  false},
            {"cam-right", false}
        };

        register_keys();

        // Add an event hook to capture events sent by Panda3D
        demon.engine.accept([this](const std::string& event_name) { this->on_evt(event_name); });

        // Create an update task
        PT(AsyncTask) update_task = make_task([this](const PT(AsyncTask)& task) -> AsyncTask::DoneStatus {
            update(task);
            return AsyncTask::DS_cont;
        }, "RoamingRalphDemoUpdate");

        auto task_mgr = AsyncTaskManager::get_global_ptr();
        task_mgr->add(update_task);
        
        // Finalize
        // Update at least once before the first 'RoamingRalphDemoUpdate' task update
        const float dt = ClockObject::get_global_clock()->get_dt();
        
        c_trav.traverse(demon.engine.render);
        character_controller.update(dt, key_map);
        character_collision_handler.update();
        camera_controller.update(dt, key_map);
    }

    void start() { 
        demon.start(); 
    }

private:
    CharacterController       character_controller;
    CharacterCollisionHandler character_collision_handler;
    CameraController          camera_controller;
    CameraCollisionHandler    camera_collision_handler;

    // Environment and character models
    NodePath environment;
    NodePath ralph;
        
    // Global
    NodePath           camera;
    CollisionTraverser c_trav;
    
    // Input handling
    std::unordered_map<std::string, bool> key_map;
    std::unordered_map<std::string, std::pair<std::string, bool>> event_map;
    
    // Other
    Demon& demon;

    void update(const PT(AsyncTask)& async_task)
    {
        if (!demon.engine.mouse_watcher->has_mouse()) return;

        c_trav.traverse(demon.engine.render);
        character_controller.update(async_task->get_dt(), key_map);
        character_collision_handler.update();
        camera_controller.update(async_task->get_dt(), key_map);
    }

    void load_world()
    {
        environment = demon.engine.resourceManager.load_model(ASSETS_PATH + "/Level.egg");
        environment.reparent_to(demon.game.render);
        environment.set_pos(LPoint3(0.0f, 0.0f, 0.0f));
    }

    void load_actor()
    {
        // load character model
        ralph = demon.engine.resourceManager.load_model(ASSETS_PATH + "/ralph.egg.pz");
        ralph.reparent_to(demon.game.render);
    }
    
    std::vector<NodePath> load_actor_anims() const
    {
        NodePath walk_anim = demon.engine.resourceManager.load_model(ASSETS_PATH + "/ralph-run.egg.pz");
        return {walk_anim};
    }

    void register_keys()
    {
        event_map["a"] = {"left",      true};
        event_map["d"] = {"right",     true};
        event_map["w"] = {"forward",   true};
        event_map["e"] = {"cam-left",  true};
        event_map["q"] = {"cam-right", true};

        event_map["a-up"] = {"left",      false};
        event_map["d-up"] = {"right",     false};
        event_map["w-up"] = {"forward",   false};
        event_map["e-up"] = {"cam-left",  false};
        event_map["q-up"] = {"cam-right", false};
    }

    void on_evt(const std::string& event_name)
    {
        auto it = event_map.find(event_name);
        if (it != event_map.end())
        {
			key_map.at(it->second.first) = it->second.second; 
        }
    }
};

int main(int argc, char* argv[])
{
    RoamingRalphDemo ralphDemo;
    ralphDemo.start();
    return 0;
}
