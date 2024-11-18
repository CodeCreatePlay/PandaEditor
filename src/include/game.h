#ifndef GAME_H
#define GAME_H

#include <map>
#include <string>
#include <vector>

#include <asyncTask.h>
#include <displayRegion.h>
#include <mouseWatcher.h>
#include <perspectiveLens.h>
#include <orthographicLens.h>
#include <nodePath.h>

#include "engine.h"


class Engine; // Forward declaration

class Game {
public:
    explicit Game(Engine* engine);
    void init();
    void update_task();

private:
    Engine* engine;

    NodePath render;
    NodePath render2D;
	
	NodePath main_cam;
	NodePath cam2D;

    DisplayRegion* dr3D;
    DisplayRegion* dr2D;

    MouseWatcher* mouse_watcher;

    std::map<std::string, void*> runtime_scripts;
    std::map<std::string, void*> components;
    std::map<void*, std::vector<void*>> attached_components;

    std::vector<void*> scenes; // Replace with appropriate scene type
    void* active_scene;        // Replace with appropriate scene type

    void create_dr2D();
    void create_dr3D();
    void create_mouse_watcher_3D();
};

#endif // GAME_H
