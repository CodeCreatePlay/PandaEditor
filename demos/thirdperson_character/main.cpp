#include <cmath>
#include <algorithm>
#include "runtimeScript.hpp"
#include "cameraController.cpp"

static const std::string ASSETS_PATH = "demos/_assets";
const std::string LEVEL_PATH         = ASSETS_PATH + "/Level.egg";
const std::string CHARACTER_PATH     = ASSETS_PATH + "/ralph.egg.pz";

class ThirdPersonCharacter : public RuntimeScript {
public:
    ThirdPersonCharacter() : 
		camera(demon.game.main_cam),
		camController(mouse, camera, target) {
			
        load_environment();

        this->accept("wheel_up", [this]() { camController.zoom(1); });
        this->accept("wheel_down", [this]() { camController.zoom(-1); });
		
        reset();
    }

protected:
    void on_update(const PT(AsyncTask)&) {
        c_trav.traverse(game.render);		
        camController.update(dt, mouse3_down);		
    }
    
    void on_event(const std::string& event_name) {
		// call base method as well
        RuntimeScript::on_event(event_name);

		// user defined keys
        if (event_name == "r-up") reset();
		if (event_name == "l-up") camController.toggle_y_orbit_lock();
		if (event_name == "mouse3") mouse3_down = true;
		if (event_name == "mouse3-up") mouse3_down = false;
    }

private:
    NodePath camera;
    NodePath target;
    CameraController camController;
	
	CollisionTraverser c_trav;
	
	bool mouse3_down = false;
    

    void load_environment() {
		// Load environment / level
        NodePath level = resource_manager.load_model(LEVEL_PATH);
        level.reparent_to(game.render);
		
		// Load a character / target for camera to orbit around
        target = resource_manager.load_model(CHARACTER_PATH);
        target.reparent_to(game.render);
        target.set_pos(level.find("**/Start_Pos").get_pos());
				
		// ---------------------------------------------------------- // 
		// Place character / target on ground using collision detection		
		PT(CollisionRay) collision_ray = new CollisionRay();
        collision_ray->set_origin(0, 0, 30);
        collision_ray->set_direction(0, 0, -1);

        PT(CollisionNode) collision_ray_node = new CollisionNode("CharacterCollisionRay");
        collision_ray_node->add_solid(collision_ray);
        collision_ray_node->set_from_collide_mask(CollideMask::bit(0));
        collision_ray_node->set_into_collide_mask(CollideMask::all_off());

        NodePath collision_np = target.attach_new_node(collision_ray_node);

        PT(CollisionHandlerQueue) collision_handler_queue = new CollisionHandlerQueue();
		
        CollisionTraverser c_trav;
		c_trav.add_collider(collision_np, collision_handler_queue);
		
		c_trav.traverse(game.render);
		
		int numEntries = collision_handler_queue->get_num_entries();
        if (numEntries == 0) return;

        // Panda3D's built-in sorting
        collision_handler_queue->sort_entries();

        // Get the lowest collision entry directly
        CollisionEntry* entry = collision_handler_queue->get_entry(0);
        if (entry->get_into_node()->get_name() == "Collider") {
            target.set_z(entry->get_surface_point(NodePath()).get_z());
		}

		//
		// ---------------------------------------------------------- // 
    }
    
    void reset() {
        camController.reset();
    }
};

int main(int argc, char* argv[]) {
    ThirdPersonCharacter thirdPersonCharacter;
    thirdPersonCharacter.start();
    return 0;
}
