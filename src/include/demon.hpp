#ifndef DEMON_H
#define DEMON_H

#include <unordered_map>
#include <vector>

#include "engine.hpp"
#include "game.hpp"
#include "levelEditor.hpp"
#include "p3d_Imgui.hpp"
#include "constants.hpp"

class Demon {	
public:
	struct Settings {
		GameViewStyle game_view_style;
		float game_view_size;
	};

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
	void bind_events();
	void unbind_events();
	void enable_game_mode();
	void exit_game_mode();
	bool is_game_mode();
	void increase_game_view_size();
	void decrease_game_view_size();
	void update_game_view();
	void update_game_view(GameViewStyle style);
	void update_game_view(GameViewStyle style, float width,  float height);
	
	void exit();
	
	// Fields
	Engine engine;
	Game game;
	LevelEditor level_ed;
	Settings settings = {GameViewStyle::BOTTOM_LEFT, 0.3f};
	Settings default_settings = {GameViewStyle::BOTTOM_LEFT, 0.3f};
	
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
	bool _cleaned_up;
	bool _is_game_mode;
	bool _mouse_over_ui;
	int  _num_frames_since_last_repait;
		
	// Delete the 'delete' operator to prevent manual deletion
	// necessary for singleton
    void operator delete(void*) = delete;
    void operator delete[](void*) = delete;  // Prevents array deletion
};

#endif
