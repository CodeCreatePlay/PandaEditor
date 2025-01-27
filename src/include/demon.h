#ifndef DEMON_H
#define DEMON_H

#include <unordered_map>
#include <vector>

#include "engine.h"
#include "game.h"
#include "levelEditor.hpp"
#include "p3d_Imgui.hpp"
#include "constants.hpp"


class Demon {
public:
    Demon();
    ~Demon();
	
	void start();
	void setup_paths();
	void enable_game_mode();
	void exit_game_mode();
	void update_game_view(GameViewStyle style);
	void exit();
	
	Engine engine;
	Game game;
	LevelEditor le;
	
private:
	bool cleaned_up;
	bool is_game_mode;
	bool mouse_over_ui;
	
	int frames_passed_since_last_repait;
	
	// ---------------------------------------------------------------------------------------------
	// imgui
    Panda3DImGui p3d_imgui;
	void init_imgui(Panda3DImGui *panda3d_imgui, NodePath *parent, MouseWatcher* mw, std::string name);
	void init_imgui_task();
	void imgui_update();
	void handle_imgui_mouse(MouseWatcher* mw, Panda3DImGui* panda3d_imgui);
};

#endif
