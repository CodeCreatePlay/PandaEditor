#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <cmath>
#include <lquaternion.h>

#include <lvecBase2.h>
#include <lvecBase3.h>
#include <graphicsWindow.h>
#include <mouseWatcher.h>
#include <mouseButton.h>
#include <keyboardButton.h>
#include <perspectiveLens.h>
#include <camera.h>
#include <nodePath.h>
#include <lineSegs.h>


// Forward declaration
class Engine;
class Mouse;

class SceneCam : public NodePath {
public:
    SceneCam(float speed = 20.0f, const LVecBase3f& default_pos = LVecBase3f(300, 400, 350));
    
    NodePath* create_axes(float thickness = 1.0f, float length = 25.0f);
    void initialize(Engine* engine);
    void move(const LVecBase3f& move_vec);
    void orbit(const LVecBase2f& delta);
    void update();
    void update_axes();
	void reset();
	void enable();
	void disable();
    void on_resize_event(float aspect_ratio);

private:
    GraphicsWindow* win;
    MouseWatcher*   mwn;
	NodePath*       aspect2d;
    Mouse*          mouse;

    NodePath*       axes;
    NodePath        cam_np;

    float           speed;
    LVecBase3f      default_pos;

    NodePath*       target;
	
	// cache
	float 			tempSpeed;
};

#endif // SCENE_CAMERA_H