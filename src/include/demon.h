#ifndef DEMON_H
#define DEMON_H

#include "engine.h"
#include "game.h"
#include "levelEditor.hpp"
// #include "..//p3d_imgui/p3d_Imgui.hpp"
// #include "imgui.h"


class Demon {
public:
    Demon();
    ~Demon();
		
	void start();
	void setup_paths();
	void enable_game_mode();
	void exit_game_mode();
	void on_evt(const Event* evt, const std::vector<void*>&);
	
	Engine engine;
	Game game;
	LevelEditor le;
	
	/*
	// imgui
    Panda3DImGui p3d_imgui;
	void setup_imgui();
	void setup_imgui_render(Panda3DImGui* panda3d_imgui_helper);
	void setup_imgui_button(Panda3DImGui* panda3d_imgui_helper);
	static void on_imgui_new_frame();
	*/
};

#endif
