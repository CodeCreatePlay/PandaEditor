#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <config_putil.h>
#include <nodePath.h>
#include <bitMask.h>

#include "pathUtils.h"
#include "taskUtils.hpp"
#include "demon.h"
#include "imgui.h"


Demon::Demon() : game(*this), le(*this) {
	// 1. Initialize
	// init base editor
	setup_paths();
	init_imgui(&p3d_imgui, &engine.pixel2d, engine.mouse_watcher, "Editor");
		
	// init game and level editor
	game.init();
	init_imgui(&game.p3d_imgui, &game.pixel2D, game.mouse_watcher, "Game");
	
	// init level editor();
	le.init();
	
	// 2. Setup game and editor viewport camera masks
	BitMask32 ed_mask   = BitMask32::bit(0);
	BitMask32 game_mask = BitMask32::bit(1);
	
	DCAST(Camera, engine.scene_cam.node())->set_camera_mask(ed_mask);
	DCAST(Camera, engine.cam2d.node())->set_camera_mask(ed_mask);
	
	DCAST(Camera, game.main_cam.node())->set_camera_mask(game_mask);
	DCAST(Camera, game.cam2D.node())->set_camera_mask(game_mask);
	
	// hide editor only geo from game view and vice versa
	engine.axisGrid.hide(game_mask);
	engine.render2d.find("**/SceneCameraAxes").hide(game_mask);
	p3d_imgui.get_root().hide(game_mask);
	
	game.p3d_imgui.get_root().hide(ed_mask);
	game.p3d_imgui.get_root().show(game_mask);
		
	// 3. Create update task
	PT(AsyncTask) update_task = (make_task([this](AsyncTask *task) -> AsyncTask::DoneStatus {

		engine.update();		
		imgui_update();
		engine.dispatch_events(mouse_over_ui);
		engine.engine->render_frame();

		mouse_over_ui = false;
		
		if(engine.should_repaint) {
			
			p3d_imgui.should_repaint = true;
			game.p3d_imgui.should_repaint = true;
			
			if(frames_passed_since_last_repait > 2) {
				
				engine.should_repaint = false;
				frames_passed_since_last_repait = 0;
			}
			frames_passed_since_last_repait++;
		}
		
		return AsyncTask::DS_cont;
	}, "EngineUpdate"));
	
	update_task->set_sort(MAIN_TASK_SORT);
	AsyncTaskManager::get_global_ptr()->add(update_task);
	
	// 4. event hooks
	engine.accept("window-event", [this]() { engine.on_evt_size(); } );

    // 5. Loop through all tasks in the task manager
    auto task_mgr = AsyncTaskManager::get_global_ptr();
    AsyncTaskCollection tasks = task_mgr->get_tasks();
    for (int i = 0; i < tasks.get_num_tasks(); ++i) {
        PT(AsyncTask) task = tasks.get_task(i);
        std::cout << "Task " << i + 1 << ": " << task->get_name() << std::endl;
    }
	
	// Reset
	cleaned_up    = false;
	is_game_mode  = false;
	mouse_over_ui = false;
}

Demon::~Demon() { 
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

void Demon::enable_game_mode() {
	
	// hide editor only geometry
	engine.render.hide();
	engine.render2d.hide();
	
	// disable editor camera
	engine.dr->set_camera(NodePath());
	
	// set game mode to true
	is_game_mode = true;
}

void Demon::exit_game_mode() {
	
	engine.render.show();
	engine.render2d.show();
	engine.dr->set_camera(engine.scene_cam);
	is_game_mode = false;
}

void Demon::update_game_view(GameViewStyle style) {
    float width  = 0.4f;
    float height = 0.4f;

    switch (style) {
        case CENTER:
            game.dr3D->set_dimensions(0, 0.5f - width / 2, 0.5f + width / 2, 0.5f - height / 2, 0.5f + height / 2);
            game.dr2D->set_dimensions(0, 0.5f - width / 2, 0.5f + width / 2, 0.5f - height / 2, 0.5f + height / 2);
            break;

        case BOTTOM_LEFT:
            game.dr3D->set_dimensions(0, 0, width, 0, height);
            game.dr2D->set_dimensions(0, 0, width, 0, height);
            break;

        case BOTTOM_RIGHT:
            game.dr3D->set_dimensions(0, 1 - width, 1, 0, height);
            game.dr2D->set_dimensions(0, 1 - width, 1, 0, height);
            break;

        case TOP_LEFT:
            game.dr3D->set_dimensions(0, 0, width, 1 - height, 1);
            game.dr2D->set_dimensions(0, 0, width, 1 - height, 1);
            break;

        case TOP_RIGHT:
            game.dr3D->set_dimensions(0, 1 - width, 1, 1 - height, 1);
            game.dr2D->set_dimensions(0, 1 - width, 1, 1 - height, 1);
            break;

        default:
            game.dr3D->set_dimensions(0, 0.5f - width / 2, 0.5f + width / 2, 0.5f - height / 2, 0.5f + height / 2);
            game.dr2D->set_dimensions(0, 0.5f - width / 2, 0.5f + width / 2, 0.5f - height / 2, 0.5f + height / 2);
            break;
    }
}

void Demon::exit() {
	
	if(cleaned_up)
		return;
	
	engine.clean_up();
	p3d_imgui.clean_up();
	engine.engine->remove_all_windows();

	cleaned_up = true;
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
	// editor ui update
	ImGui::SetCurrentContext(this->p3d_imgui.context_);
	
	if (this->p3d_imgui.should_repaint) {
		this->p3d_imgui.on_window_resized();
		this->p3d_imgui.should_repaint = false;
	}
	
	this->p3d_imgui.new_frame_imgui();
	this->handle_imgui_mouse(this->engine.mouse_watcher, &this->p3d_imgui);
	
	engine.trigger("editor_imgui");

	this->p3d_imgui.render_imgui();
	if(ImGui::GetIO().WantCaptureMouse) { mouse_over_ui = true; }
	
	// game view imgui
	ImGui::SetCurrentContext(this->game.p3d_imgui.context_);
	
	if (this->game.p3d_imgui.should_repaint) {
		this->game.p3d_imgui.on_window_resized();
		this->game.p3d_imgui.should_repaint = false;
	}
	
	this->game.p3d_imgui.new_frame_imgui();
	this->handle_imgui_mouse(this->game.mouse_watcher, &this->game.p3d_imgui);
	engine.trigger("game_imgui");

	this->game.p3d_imgui.render_imgui();
	if (ImGui::GetIO().WantCaptureMouse) { mouse_over_ui = true; }
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
