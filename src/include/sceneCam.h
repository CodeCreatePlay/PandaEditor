#ifndef SCENE_CAMERA_H
#define SCENE_CAMERA_H

#include <lvecBase2.h>
#include <lvecBase3.h>
#include <lineSegs.h>
#include <graphicsWindow.h>
#include <mouseWatcher.h>
#include <keyboardButton.h>
#include <perspectiveLens.h>
#include <camera.h>
#include <nodePath.h>


// Forward declaration
class Engine;
class Mouse;

class SceneCam : public NodePath {
public:
    SceneCam(float speed = 0.5f, const LVecBase3f& default_pos = LVecBase3f(300, 400, 350));
    
    NodePath* create_axes(float thickness = 1.0f, float length = 25.0f);
    void initialize(Engine* engine);
    void move(const LVecBase3f& move_vec);
    void orbit(const LVecBase2f& delta);
    void update();
    void update_axes();
    void on_resize_event(float aspect_ratio);
    void reset();

private:
    GraphicsWindow* win;
    MouseWatcher*   mouse_watcher_node;

    NodePath*       axes;
    NodePath        cam_np;
    NodePath*       aspect2d;
    Mouse*          mouse;

    float           speed;
    LVecBase3f      default_pos;

    NodePath*       target;
};

#endif // SCENE_CAMERA_H