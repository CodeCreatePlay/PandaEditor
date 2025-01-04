#ifndef GAME_H
#define GAME_H

#include "p3d_Imgui.hpp"


class MouseWatcher;
class DisplayRegion;
class ButtonThrower;
class NodePath;
class Demon;

class Game {
public:
    explicit Game(Demon* demon);
    void init();
    void update();
	
	//ButtonThrower* button_thrower;
	NodePath render;
    NodePath render2D;
	NodePath pixel2D;
	
	NodePath main_cam;
	NodePath cam2D;

    DisplayRegion* dr3D;
    DisplayRegion* dr2D;
	
	Panda3DImGui p3d_imgui;
	MouseWatcher* mouse_watcher;

private:
	Demon* demon;
    // MouseWatcher* mouse_watcher;

    void create_dr2D();
    void create_dr3D();
    void create_mouse_watcher_3D();
};

#endif // GAME_H
