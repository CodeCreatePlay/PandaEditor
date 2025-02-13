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
    // Delete copy constructor and assignment operator
	// necessary for singleton
    Demon(const Demon&) = delete;
    Demon& operator=(const Demon&) = delete;
	
    static Demon& get_instance() {
        static Demon instance;
        return instance;
    }
	
	// Methods
	void start();
	void enable_game_mode();
	void exit_game_mode();
	void update_game_view(GameViewStyle style);
	void exit();
	
	// Fields
	Engine engine;
	Game game;
	LevelEditor le;
	
private:
    Demon();
    ~Demon();

	// Methods
	void setup_paths();
	
	// ImGui fields and methods
    Panda3DImGui p3d_imgui;
	void init_imgui(Panda3DImGui *panda3d_imgui, NodePath *parent, MouseWatcher* mw, std::string name);
	void init_imgui_task();
	void imgui_update();
	void handle_imgui_mouse(MouseWatcher* mw, Panda3DImGui* panda3d_imgui);
	
	// Fields
	bool cleaned_up;
	bool is_game_mode;
	bool mouse_over_ui;
	int  frames_passed_since_last_repait;
	
	// Other
	// Delete the 'delete' operator to prevent manual deletion
	// necessary for singleton
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;  // Prevents array deletion
};

#endif
