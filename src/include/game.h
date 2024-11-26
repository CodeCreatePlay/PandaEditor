#ifndef GAME_H
#define GAME_H

class NodePath;
class MouseWatcher;
class DisplayRegion;
class Engine;

class Game {
public:
    explicit Game(Engine* engine);
    void init();
    void update_task();
	
	NodePath render;
    NodePath render2D;
	
	NodePath main_cam;
	NodePath cam2D;

    DisplayRegion* dr3D;
    DisplayRegion* dr2D;

private:
    Engine* engine;
    MouseWatcher* mouse_watcher;

    void create_dr2D();
    void create_dr3D();
    void create_mouse_watcher_3D();
};

#endif // GAME_H
