#ifndef DEMON_H
#define DEMON_H

#include <asyncTask.h>
#include <genericAsyncTask.h>
#include "..//utils/include/pathUtils.h"
#include "engine.h"
// #include "..//p3d_imgui/p3d_Imgui.hpp"
// #include "imgui.h"


class Demon {
public:
    Demon();
    ~Demon();
	
	Engine engine;
	
	void setup_paths();
	void on_evt(const Event* evt, const std::vector<void*>&);
	void start();
	bool temp;
	
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
