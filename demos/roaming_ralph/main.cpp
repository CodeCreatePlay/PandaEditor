#include <string>
#include <unordered_map>

#include <lpoint3.h>
#include <clockObject.h>
#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <nodePath.h>
#include <texturePool.h>
#include <collisionRay.h>
#include <collisionNode.h>
#include <collisionTraverser.h>
#include <collisionHandlerQueue.h>
#include <collideMask.h>

#include "demon.h"
#include "animUtils.h"


static const std::string ASSETS_PATH = "demos/assets/models";

class RoamingRalphDemo {
public:
	RoamingRalphDemo(Demon *demon) : demon(demon) {
		
		global_clock = ClockObject::get_global_clock();
		
		load_world ();
		load_actor ();
		setup_character_collisions();
		
		// define a key map
		key_map = {
            {"left",      false},
            {"right",     false},
            {"forward",   false},
            {"cam-left",  false},
            {"cam-right", false}
        };
		
		// on any event
		demon->engine.add_event_hook(1,
		[this](const Event* evt, const std::vector<void*>& params)
		{ this->on_evt(evt, params); }
		);

		register_keys();
		
		auto task_mgr = AsyncTaskManager::get_global_ptr();
        PT(GenericAsyncTask) my_task = new GenericAsyncTask("member_task",
            [](GenericAsyncTask* task, void* user_data) {
                static_cast<RoamingRalphDemo*>(user_data)->update_task(task);
                return AsyncTask::DS_cont;  // Task continues running
            },
            this
        );
		task_mgr->add(my_task);
	}
	
private:
	Demon *demon;
	PT(ClockObject) global_clock;
	
	std::unordered_map<std::string, bool> key_map;
	std::unordered_map<std::string, std::pair<std::string, bool>> event_map;

	// environment models
	NodePath environment;
	PT(Texture) tex;
	
	// character models
	NodePath ralph;
	AnimControlCollection animator;  // AnimControlCollection to manage the animations
	
	// collision setup
	PT(CollisionRay) ralph_ground_ray;
    PT(CollisionNode) ralph_ground_ray_node;
	NodePath ralph_ground_col_np;

    PT(CollisionHandlerQueue) ralph_ground_handler;
    CollisionTraverser c_trav;
	
	// character controller setup
	LPoint3 start_pos;
	bool is_moving;
	

	void update_task(GenericAsyncTask*) {

		if(!demon->engine.mouse_watcher->has_mouse())
			return;
		
		if (key_map["left"])
            ralph.set_h(ralph.get_h() + 300 * global_clock->get_dt());
        if (key_map["right"])
            ralph.set_h(ralph.get_h() - 300 * global_clock->get_dt());
        if (key_map["forward"])
            ralph.set_y(ralph, -25 * global_clock->get_dt());

        if (key_map["forward"] || key_map["left"] || key_map["right"]) {
            if (!is_moving) {
				animator.get_anim(0)->loop(true);
                is_moving = true;
			}
		}
        else {
            if (is_moving) {
				animator.stop_all();
                animator.pose("walk", 5);
                is_moving = false;
			}
		}
		
		// 
		c_trav.traverse(demon->engine.render);
		
		// Step 1: Get the number of collision entries in the queue
		int numEntries = ralph_ground_handler->get_num_entries();
		
		// Step 2: Create a vector to store CollisionEntry pointers
		std::vector<CollisionEntry*> entries;

		// Step 3: Populate the entries vector
		for (int i = 0; i < numEntries; ++i) {
			entries.push_back(ralph_ground_handler->get_entry(i));
		}
		
		// Step 4: Sort the entries based on the Z-coordinate of the surface point
		std::sort(entries.begin(), entries.end(), [this](CollisionEntry* a, CollisionEntry* b) {
			return a->get_surface_point(demon->engine.render).get_z() < b->get_surface_point(demon->engine.render).get_z();
		});

		// Step 5: If there are any entries, check if the first entry's into node is "terrain"
		// std::cout << entries[0]->get_into_node()->get_name() << std::endl;
		if (!entries.empty() && entries[0]->get_into_node()->get_name() == "Collision") {
			// Set ralph's Z-coordinate to the Z of the collision point
			LPoint3 surfacePoint = entries[0]->get_surface_point(demon->engine.render);
			ralph.set_z(surfacePoint.get_z());
			// std::cout << surfacePoint.get_z() << std::endl;
		} else {
			// Set ralph's position to startpos if no valid entry was found
			ralph.set_pos(start_pos);
			// std::cout << ralph.get_pos() << std::endl;
		}
	}
	
	void load_world() {
		environment = demon->engine.resourceManager.load_model(ASSETS_PATH + "/LevelTemp.egg");
		demon->game.render.attach_new_node(environment.node());
		// demon->engine.render.attach_new_node(environment.node());
		
		environment.reparent_to(demon->game.render);
		environment.set_pos(LPoint3(0.0f, 0.0f, 0.0f));
		environment.ls();
		
		// load the texture
		// tex =  demon->engine.resourceManager.load_texture("demos/roaming-ralph/assets/models/tex/Albedo.jpg");
		// environment.set_texture(tex, 1);
	}
	
	void load_actor() {
		// load character model
		ralph = demon->engine.resourceManager.load_model(ASSETS_PATH + "/ralph.egg.pz");
		ralph.reparent_to(demon->engine.render);
		start_pos = environment.find("**/start_point").get_pos();
		ralph.set_pos(start_pos);
		
		NodePath walk_anim = demon->engine.resourceManager.load_model(ASSETS_PATH + "/ralph-run.egg.pz");
		AnimUtils::bind_anims(ralph, walk_anim, animator);  // Bind the animation to the character
	}
	
	void load_extras() {
	}
	
    void setup_character_collisions() {
        // c_trav = new CollisionTraverser();

        // Collision ray setup
        ralph_ground_ray = new CollisionRay();
        ralph_ground_ray->set_origin(0, 0, 9);
        ralph_ground_ray->set_direction(0, 0, -1);

        // Collision node
        ralph_ground_ray_node = new CollisionNode("RalphRay");
        ralph_ground_ray_node->add_solid(ralph_ground_ray);
        ralph_ground_ray_node->set_from_collide_mask(CollideMask::bit(0));
        ralph_ground_ray_node->set_into_collide_mask(CollideMask::all_off());

        // Attach to root node
        ralph_ground_col_np = ralph.attach_new_node(ralph_ground_ray_node);

        // Collision handler
        ralph_ground_handler = new CollisionHandlerQueue();

        // Add collider
        c_trav.add_collider(ralph_ground_col_np, ralph_ground_handler);
    }
	
	void setup_cam_controls() {
	}
	
    void register_keys() {
		// Add entries corresponding to the Panda3D Python events
		event_map["a"] = {"left", true};
		event_map["d"] = {"right", true};
		event_map["w"] = {"forward", true};
		event_map["e"] = {"cam-left", true};
		event_map["q"] = {"cam-right", true};

		event_map["a-up"] = {"left", false};
		event_map["d-up"] = {"right", false};
		event_map["w-up"] = {"forward", false};
		event_map["e-up"] = {"cam-left", false};
		event_map["q-up"] = {"cam-right", false};
    }
	
	void on_key(const std::string evt, const std::pair<std::string, bool> key_value) {
		key_map[key_value.first] = key_value.second;
	}
	
    void on_evt(const Event* evt, const std::vector<void*> params) {
		if(event_map.find(evt->get_name()) != event_map.end()) {
			on_key(evt->get_name(), event_map[evt->get_name()]);
		}
	}
};


int main(int argc, char* argv[]) {
	
    Demon demon;
	RoamingRalphDemo ralph(&demon);
	
	demon.start();
    return 0;
}
