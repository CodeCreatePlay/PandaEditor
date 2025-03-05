#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <config_putil.h>
#include <nodePath.h>
#include <bitMask.h>

#include "pathUtils.hpp"
#include "taskUtils.hpp"
#include "mathUtils.hpp"
#include "demon.hpp"
#include "imgui.h"

Demon::Demon() : game(*this), level_ed(*this) {
	setup_paths();
	
	// Initializations
	init_imgui(&p3d_imgui, &engine.pixel2D, engine.mouse_watcher, "Editor");
	
	game.init();
	init_imgui(&game.p3d_imgui, &game.pixel2D, game.mouse_watcher, "Game");
	
	level_ed.init();
	
	// Setup game and editor viewport camera masks
	BitMask32 ed_mask   = BitMask32::bit(0);
	BitMask32 game_mask = BitMask32::bit(1);
	
	DCAST(Camera, engine.scene_cam.node())->set_camera_mask(ed_mask);
	DCAST(Camera, engine.cam2D.node())->set_camera_mask(ed_mask);
	
	DCAST(Camera, game.main_cam.node())->set_camera_mask(game_mask);
	DCAST(Camera, game.cam2D.node())->set_camera_mask(game_mask);
	
	// Hide editor only geo from game view and vice versa
	engine.axis_grid.hide(game_mask);
	engine.render2D.find("**/SceneCameraAxes").hide(game_mask);
	p3d_imgui.get_root().hide(game_mask);
	
	game.p3d_imgui.get_root().hide(ed_mask);
	game.p3d_imgui.get_root().show(game_mask);
		
	// Create update task
	PT(AsyncTask) update_task = (make_task([this](AsyncTask *task) -> AsyncTask::DoneStatus {

		engine.update();		
		imgui_update();
		engine.dispatch_events(_mouse_over_ui);
		engine.engine->render_frame();

		_mouse_over_ui = false;
		
		if(engine.should_repaint) {
			
			p3d_imgui.should_repaint = true;
			game.p3d_imgui.should_repaint = true;
			
			if(_num_frames_since_last_repait > 2) {
				
				engine.should_repaint = false;
				_num_frames_since_last_repait = 0;
			}
			_num_frames_since_last_repait++;
		}
		
		return AsyncTask::DS_cont;
	}, "EngineUpdate"));
	
	update_task->set_sort(MAIN_TASK_SORT);
	AsyncTaskManager::get_global_ptr()->add(update_task);
	
	// Add event hooks
	engine.accept("window-event", [this]() { engine.on_evt_size(); } );

    // Loop through all tasks in the task manager
    auto task_mgr = AsyncTaskManager::get_global_ptr();
    AsyncTaskCollection tasks = task_mgr->get_tasks();
    for (int i = 0; i < tasks.get_num_tasks(); ++i) {
        PT(AsyncTask) task = tasks.get_task(i);
        std::cout << "Task " << i + 1 << ": " << task->get_name() << std::endl;
    }
	
	// Bind events
	bind_events();
	
	// Set some defaults
	float game_view_size = default_settings.game_view_size;
	update_game_view(GameViewStyle::BOTTOM_LEFT, game_view_size, game_view_size);
	
	engine.mouse.set_mouse_mode(WindowProperties::M_absolute);
	
	_cleaned_up    = false;
	_is_game_mode  = false;
	_mouse_over_ui = false;
}

Demon::~Demon() { 
	unbind_events();
	exit(); 
}

void Demon::start() {
	while (!engine.win->is_closed()) {
		AsyncTaskManager::get_global_ptr()->poll();	
	}
}

void Demon::setup_paths() {
	std::string path = PathUtils::get_current_working_dir();
	get_model_path().prepend_directory(Filename::from_os_specific(path));
}

void Demon::bind_events() {
	engine.accept("shift-1",   [this]() { update_game_view(GameViewStyle::BOTTOM_LEFT);  });
	engine.accept("shift-2",   [this]() { update_game_view(GameViewStyle::BOTTOM_RIGHT); });
	engine.accept("shift-3",   [this]() { update_game_view(GameViewStyle::TOP_LEFT);     });
	engine.accept("shift-4",   [this]() { update_game_view(GameViewStyle::TOP_RIGHT);    });
	engine.accept("shift-5",   [this]() { update_game_view(GameViewStyle::CENTER);       });
	engine.accept("shift-+",   [this]() { increase_game_view_size();                     });
	engine.accept("shift--",   [this]() { decrease_game_view_size();                     });
	
	engine.accept("control-1",   [this]() { engine.mouse.set_mouse_mode(WindowProperties::M_absolute); });
	engine.accept("control-2",   [this]() { engine.mouse.set_mouse_mode(WindowProperties::M_relative); });
	engine.accept("control-3",   [this]() { engine.mouse.set_mouse_mode(WindowProperties::M_confined); });
	engine.accept("control-r",   [this]() { engine.mouse.toggle_force_relative_mode(); });
	
	engine.accept("shift-g",   [this]() {
		if (!is_game_mode())
			enable_game_mode();
		else
			exit_game_mode();
	});
}

void Demon::unbind_events() {}

void Demon::enable_game_mode() {
	// Sanity check
	if (_is_game_mode)
		return;
	
	// Remove camera from display regions
	engine.dr->set_camera(NodePath());
	engine.dr2D->set_camera(NodePath());
		
	// Temporarily set display region as inactive
	engine.dr->set_active(false);
	engine.dr2D->set_active(false);
	
	// Set game mode to true
	_is_game_mode = true;
}

void Demon::exit_game_mode() {
	// Sanity check
	if (!_is_game_mode)
		return;
	
	// Remove camera from display regions
	engine.dr->set_camera(engine.scene_cam);
	engine.dr2D->set_camera(engine.cam2D);
		
	// Set display regions back to active
	engine.dr->set_active(true);
	engine.dr2D->set_active(true);
		
	// Set game mode to true
	_is_game_mode = false;
}

void Demon::increase_game_view_size() {
	float increment = (1.0f - default_settings.game_view_size) / 4.0f;	
	float min_size = default_settings.game_view_size;
	settings.game_view_size = clamp(settings.game_view_size + increment, min_size, 1.0f);
	this->update_game_view();
}

void Demon::decrease_game_view_size() {	
	float decrement = (1.0f - default_settings.game_view_size) / 4.0f;
	float min_size = default_settings.game_view_size;
	settings.game_view_size = clamp(settings.game_view_size - decrement, min_size, 1.0f);
	this->update_game_view();
}

void Demon::update_game_view() {
	GameViewStyle style = settings.game_view_style;
	float size = settings.game_view_size;	
	this->update_game_view(style, size, size);
}

void Demon::update_game_view(GameViewStyle style) {
	settings.game_view_style = style;
	float size = settings.game_view_size;	
	this->update_game_view(style, size, size);
}

void Demon::update_game_view(GameViewStyle style, float width, float height) {
    float left, right, bottom, top;

    switch (style) {
        case CENTER:
            left   = 0.5f - width / 2;
            right  = 0.5f + width / 2;
            bottom = 0.5f - height / 2;
            top    = 0.5f + height / 2;
            break;

        case BOTTOM_LEFT:
            left   = 0;
            right  = width;
            bottom = 0;
            top    = height;
            break;

        case BOTTOM_RIGHT:
            left   = 1 - width;
            right  = 1;
            bottom = 0;
            top    = height;
            break;

        case TOP_LEFT:
            left   = 0;
            right  = width;
            bottom = 1 - height;
            top    = 1;
            break;

        case TOP_RIGHT:
            left   = 1 - width;
            right  = 1;
            bottom = 1 - height;
            top    = 1;
            break;

        default:
            left   = 0.5f - width / 2;
            right  = 0.5f + width / 2;
            bottom = 0.5f - height / 2;
            top    = 0.5f + height / 2;
            break;
    }

    // Update 3D and 2D Display Regions
    game.dr3D->set_dimensions(left, right, bottom, top);
    game.dr2D->set_dimensions(left, right, bottom, top);

    // Convert from [0,1] range to [-1,1] range for MouseWatcherRegion
    float mw_left   = 2 * left - 1;
    float mw_right  = 2 * right - 1;
    float mw_bottom = 2 * bottom - 1;
    float mw_top    = 2 * top - 1;

    // Update the MouseWatcherRegion
    game.mouse_region->set_frame(mw_left, mw_right, mw_bottom, mw_top);
}

bool Demon::is_game_mode() {
	return _is_game_mode == true;
}

void Demon::exit() {
	
	if(_cleaned_up)
		return;
	
	engine.clean_up();
	p3d_imgui.clean_up();
	engine.engine->remove_all_windows();

	_cleaned_up = true;
}

// ----------------------------------------- imgui integration ----------------------------------------- //
void Demon::init_imgui(Panda3DImGui *panda3d_imgui, NodePath *parent, MouseWatcher* mw, std::string name) {
	
	// Setup ImGUI for Panda3D
	panda3d_imgui->init(engine.win, mw, parent);
	panda3d_imgui->setup_style();
    panda3d_imgui->setup_geom();
    panda3d_imgui->setup_shader(Filename("assets/shaders"));
    panda3d_imgui->setup_font();
    panda3d_imgui->setup_event();
    panda3d_imgui->enable_file_drop();
}

void Demon::imgui_update() {
	// Editor view ui update
	ImGui::SetCurrentContext(this->p3d_imgui.context_);
	
	if (this->p3d_imgui.should_repaint) {
		this->p3d_imgui.on_window_resized();
		this->p3d_imgui.should_repaint = false;
	}
	
	this->p3d_imgui.new_frame_imgui();
	this->handle_imgui_mouse(this->engine.mouse_watcher, &this->p3d_imgui);
	
	engine.trigger("main_gui");

	this->p3d_imgui.render_imgui();
	if(ImGui::GetIO().WantCaptureMouse) { _mouse_over_ui = true; }
	
	// Game view ui imgui
	ImGui::SetCurrentContext(this->game.p3d_imgui.context_);
	
	if (this->game.p3d_imgui.should_repaint) {
		this->game.p3d_imgui.on_window_resized();
		this->game.p3d_imgui.should_repaint = false;
	}
	
	this->game.p3d_imgui.new_frame_imgui();
	this->handle_imgui_mouse(this->game.mouse_watcher, &this->game.p3d_imgui);
	engine.trigger("game_view_gui");

	this->game.p3d_imgui.render_imgui();
	if (ImGui::GetIO().WantCaptureMouse) { _mouse_over_ui = true; }
}

void Demon::handle_imgui_mouse(MouseWatcher* mw, Panda3DImGui* panda3d_imgui) {
	
	if(!mw->has_mouse())
		return;
	
	for (const ButtonHandle& button: panda3d_imgui->btn_handles) {
		
		if(mw->is_button_down(button))
			panda3d_imgui->on_button_down_or_up(button, true);
		else
			panda3d_imgui->on_button_down_or_up(button, false);
	}
}
