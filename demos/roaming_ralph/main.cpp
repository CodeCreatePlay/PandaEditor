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

#include "demon.h"

#include "characterController.cpp"
#include "characterCollisionHandler.cpp"
#include "cameraController.cpp"
#include "cameraCollisionHandler.cpp"


static const std::string ASSETS_PATH = "demos/_assets";

class RoamingRalphDemo {
public:
    RoamingRalphDemo() 
        : demon( Demon::get_instance() ),
          cam_controller(ralph, camera),
          cam_collision_handler(ralph, c_trav),
          character_controller(ralph),
          character_collision_handler(ralph, c_trav)
	{
        camera = demon.game.main_cam;

        load_world();
        load_actor();
        
		character_controller.load_anims(ASSETS_PATH);
		character_collision_handler.init();
        cam_controller.init();
        cam_collision_handler.init();
		
		// create a key map and register keys to their corresponding events
        key_map = 
		{
			{"left",      false},
			{"right",     false},
			{"forward",   false},
			{"cam-left",  false},
			{"cam-right", false}
		};

        register_keys();

		// add an event hook to capture events sent by panda3D
        demon.engine.add_event_hook(
            1,
            [this](const Event* evt, const std::vector<void*>& params)
			{ this->on_evt(evt, params); }
        );

		// create an update task
        PT(GenericAsyncTask) my_task = new GenericAsyncTask(
			"RoamingRalphDemoUpdate",
            [](GenericAsyncTask* task, void* user_data)
			{
                static_cast<RoamingRalphDemo*>(user_data)->update_task(task);
                return AsyncTask::DS_cont;
            },
            this
        );

        auto task_mgr = AsyncTaskManager::get_global_ptr();
        task_mgr->add(my_task);
		
		// finalize
		// update at least once before the first 'RoamingRalphDemoUpdate' task update
		c_trav.traverse(demon.engine.render);
		
        character_controller.update(key_map);
        character_collision_handler.update();
        cam_controller.update(key_map);
    }

    void start() { 
		demon.start(); 
	}

private:
	CharacterController       character_controller;
    CharacterCollisionHandler character_collision_handler;
    CameraController          cam_controller;
    CameraCollisionHandler    cam_collision_handler;

	// environment and character models
    NodePath environment;
    NodePath ralph;
	
	LPoint3 start_pos; // start pos of character
	
	// global
	NodePath camera;
	CollisionTraverser c_trav;
	
	// other
	Demon &demon;
    std::unordered_map<std::string, bool> key_map;
	std::unordered_map<std::string, std::pair<std::string, bool>> event_map;


    void update_task(GenericAsyncTask*)
	{
        if (!demon.engine.mouse_watcher->has_mouse()) return;

        c_trav.traverse(demon.engine.render);

        character_controller.update(key_map);
        character_collision_handler.update();
        cam_controller.update(key_map);
    }

	void load_world()
	{
		environment = demon.engine.resourceManager.load_model(ASSETS_PATH + "/Level.egg");		
		environment.reparent_to(demon.game.render);
		environment.set_pos(LPoint3(0.0f, 0.0f, 0.0f));
		environment.ls();
	}

	void load_actor()
	{
		// load character model
		ralph = demon.engine.resourceManager.load_model(ASSETS_PATH + "/ralph.egg.pz");
		ralph.reparent_to(demon.engine.render);
		start_pos = environment.find("**/start_point").get_pos();
		ralph.set_pos(start_pos);
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

    void on_evt(const Event* evt, const std::vector<void*>& params)
	{
        if (event_map.find(evt->get_name()) != event_map.end())
		{
            key_map[event_map[evt->get_name()].first] = event_map[evt->get_name()].second;
        }
    }
};


int main(int argc, char* argv[])
{
	RoamingRalphDemo ralphDemo;
	ralphDemo.start();
    return 0;
}
