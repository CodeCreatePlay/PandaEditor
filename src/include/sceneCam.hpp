#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <nodePath.h>

// Forward declarations
class LVecBase3f;
class LVecBase2f;
class Engine;

class SceneCam : public NodePath {
public:
    SceneCam(Engine& engine, float speed = 20.0f, const LVecBase3f& default_pos = LVecBase3f(300, 400, 350));
    
    void initialize();
    void update();
    void reset();
    void enable();
    void disable();
    void on_resize_event(float aspect_ratio);

    void move(const LVecBase3f& move_vec);
    void orbit(const LVecBase2f& delta);
    NodePath create_axes(float thickness = 1.0f, float length = 25.0f);
    void update_axes();

private:
    Engine& engine;

    NodePath axes;
    NodePath cam_np;
    NodePath target;

    float move_speed;  // move speed of camera
    float delta_speed; // move speed scaled by ClockObject::get_global_clock()->get_dt()
	
    LVecBase3f default_pos;
};

#endif // SCENE_CAMERA_H
