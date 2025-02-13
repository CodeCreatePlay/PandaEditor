#ifndef MARQUEE_H
#define MARQUEE_H

#include <string>
#include <vector>

#include <asyncTask.h>
#include <LVector4.h>
#include <camera.h>
#include <nodePath.h>


class Engine;

class Marquee {
public:
    Marquee(const std::string &name, Engine& engine);
    
    void init(NodePath render);
    void on_start();
    void on_stop();
    AsyncTask::DoneStatus on_update();
    std::vector<NodePath> get_found_nps();

private:
    NodePath create_fullscreen_quad(const std::string &_name);

    std::string   _name;
    Engine&       engine;
    NodePath      render;
    NodePath      quad;
	LPoint2f      init_mouse_pos;
    LVector4      saved_corners;
	PT(AsyncTask) update_task;
};

#endif // MARQUEE_H
