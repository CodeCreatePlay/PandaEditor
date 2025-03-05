#include <cmath>
#include <algorithm>
#include "runtimeScript.hpp"

static const std::string ASSETS_PATH = "demos/_assets/_third_person_cam";
const std::string LEVEL_PATH         =  ASSETS_PATH + "/Level.egg";

class ThirdPersonCam : public RuntimeScript {
public:
    ThirdPersonCam() : camera(demon.game.main_cam) {
		
        load_environment();
		
        this->accept("wheel_up",   [this]() { this->zoom(1);  });
        this->accept("wheel_down", [this]() { this->zoom(-1); });
		
        reset();
    }

protected:
    void on_update(const PT(AsyncTask)&) {
        move_to_target();
        orbit();
		look_at_target();
    }
	
    void on_event(const std::string& event_name) {
		// call base method as well
		RuntimeScript::on_event(event_name);
		
		// user defined keys
        if (event_name == "r-up")
			reset();
		else if(event_name == "l-up")
			lock_x_orbit = !lock_x_orbit;
    }

private:
	// references
    NodePath camera;
    NodePath target;
	
	// position settings
    LVecBase3f target_pos_offset = LVecBase3f(0, 0, 5);
    int look_smooth = 100;
    int zoom_smooth = 100;
    int min_zoom = 10;
    int max_zoom = 100;
	
	// orbit settings
	int orbit_smooth = 150;
	
    float max_rotation_x =  30.0f;
    float min_rotation_x = -30.0f;

    bool lock_x_orbit = false;
    bool lock_y_orbit = false;
	
	// cache
    LVecBase3f target_pos = LVecBase3f(0, 0, 0);
    float x_rotation = 0.0f;
    float y_rotation = 0.0f;
    float distance_to_target = 30.0f;
	
	// methods
    void load_environment() {
        NodePath level = demon.engine.resource_manager.load_model(LEVEL_PATH);
        level.reparent_to(demon.game.render);

        NodePath smiley = demon.engine.resource_manager.load_model("smiley.egg");
        smiley.reparent_to(camera);

        target = level.find("Player");
    }

    void reset() {
        distance_to_target = 30.0f;
		
        y_rotation = 0.0f;
		x_rotation = 0.0f;
		
		LPoint3f newPos = target.get_pos() + target_pos_offset;
		newPos.set_y(-distance_to_target);
		camera.set_pos(newPos);
		
        move_to_target();
		orbit();
        look_at_target();
    }

	void move_to_target() {		
		// Convert degrees to radians
		float yawRad   = y_rotation * (M_PI / 180.0f);
		float pitchRad = x_rotation * (M_PI / 180.0f);

		// Spherical coordinates to Cartesian conversion
		float x =  distance_to_target * std::cos(pitchRad) * std::sin(yawRad);
		float y = -distance_to_target * std::cos(pitchRad) * std::cos(yawRad);
		float z =  distance_to_target * std::sin(pitchRad);

		// Set new camera position
		LPoint3f newPos      = target.get_pos() + target_pos_offset + LPoint3f(x, y, z);
		LPoint3f currentPos  = camera.get_pos();
		LPoint3f smoothedPos = lerp(currentPos, newPos, 0.5f);
		camera.set_pos(smoothedPos);
	}

    void look_at_target() {
        camera.look_at(target.get_pos());
    }

	void orbit() {
		float abs_dx = std::abs(mouse.get_dx());  // Absolute horizontal movement
		float abs_dy = std::abs(mouse.get_dy());  // Absolute vertical movement

		if (abs_dx > abs_dy) {  // Horizontal movement is dominant
			if (!lock_y_orbit) {
				y_rotation += mouse.get_horizontal() * orbit_smooth * dt;
				y_rotation = fmod(y_rotation, 360.0f);
			}
		} else if (abs_dy > abs_dx) {  // Vertical movement is dominant
			if (!lock_x_orbit) {
				x_rotation += mouse.get_vertical() * orbit_smooth * dt;
				x_rotation = clamp(x_rotation, min_rotation_x, max_rotation_x);
			}
		}
	}

    void zoom(int zoom) {
		distance_to_target += zoom * zoom_smooth * dt;
        distance_to_target  = clamp(distance_to_target, min_zoom, max_zoom);
    }
};


int main(int argc, char* argv[]) {
    ThirdPersonCam thirdpersoncam;
    thirdpersoncam.start();
    return 0;
}
