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
    explicit Game(Demon& demon);
    void init();
    void on_evt(const std::string& event_name); // Pass string by const reference
	
    NodePath               render;
    NodePath               render2D;
    NodePath               aspect2D;
    NodePath               pixel2D;
    NodePath               main_cam;
    NodePath               cam2D;
    
    PT(DisplayRegion)      dr3D;
    PT(DisplayRegion)      dr2D;
    PT(MouseWatcher)       mouse_watcher;
	PT(MouseWatcherRegion) mouse_region;
    
    Panda3DImGui           p3d_imgui;

private:
    Demon& demon;

    void create_dr2D();
    void create_dr3D();
    void create_mouse_watcher_3D();
};

#endif // GAME_H
