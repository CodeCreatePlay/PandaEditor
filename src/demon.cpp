#include <asyncTask.h>
#include <genericAsyncTask.h>
#include <config_putil.h>
#include <nodePath.h>

#include "pathUtils.h"
#include "taskUtils.hpp"
#include "demon.h"


Demon::Demon() : game(&engine), le(&engine, &game), p3d_imgui(engine.win, engine.pixel2d), cleaned_up(false) {

	// 1. Init
	setup_paths();
	game.init();
	le.init();
	setup_imgui();
	
	// 2. Setup game and editor viewport camera masks
	BitMask32 ed_mask   = BitMask32::bit(0) | BitMask32::bit(1);
	BitMask32 game_mask = BitMask32::bit(1);
	
	DCAST(Camera, engine.scene_cam.node())->set_camera_mask(ed_mask);
	DCAST(Camera, engine.cam2d.node())->set_camera_mask(ed_mask);
	
	DCAST(Camera, game.main_cam.node())->set_camera_mask(game_mask);
	DCAST(Camera, game.cam2D.node())->set_camera_mask(game_mask);
	
	// hide editor only geo from game view
	engine.axisGrid.hide(game_mask);
	engine.render2d.find("**/SceneCameraAxes").hide(game_mask);
	
	// 3. Add an event hook to catch events
	engine.add_event_hook(0,
		[this](const Event* evt, const std::vector<void*>& params) { this->on_evt(evt, params); }
	);
	
	// 4. Create update task
	PT(AsyncTask) update_task = (make_task([this](AsyncTask *task) -> AsyncTask::DoneStatus {
		engine.update(); return AsyncTask::DS_cont;
	}, "EngineUpdate"));
	update_task->set_sort(0);

	AsyncTaskManager::get_global_ptr()->add(update_task);
	exit();
}

Demon::~Demon() { 
	exit(); 
}

void Demon::start() {
	
	while (!engine.win->is_closed()) {
		// update task manager
		AsyncTaskManager::get_global_ptr()->poll();
		
		// finally, render frame
		engine.engine->render_frame();
	}
}

void Demon::setup_paths() {
	std::string path = PathUtils::get_current_working_dir();
	get_model_path().prepend_directory(Filename::from_os_specific(path));
}

void Demon::enable_game_mode() {
	// hide editor only geometry
	
	// disable editor camera
	engine.dr->set_camera(NodePath());
}

void Demon::exit_game_mode() {
	// show editor only geometry
	
	
	// enable editor camera
	engine.dr->set_camera(engine.scene_cam);
}

void Demon::on_evt(const Event* evt, const std::vector<void*>&) {
	// std::cout << evt->get_name() << std::endl;
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
void Demon::setup_imgui() {
	// setup ImGUI for Panda3D
	p3d_imgui.setup_style();
    p3d_imgui.setup_geom();
    p3d_imgui.setup_shader(Filename("assets/shaders"));
    p3d_imgui.setup_font();
    p3d_imgui.setup_event();

    p3d_imgui.enable_file_drop();
	
	setup_imgui_render(&p3d_imgui);
	setup_imgui_button(&p3d_imgui);

    EventHandler::get_global_event_handler()->add_hook(
        "window-event",
        [](const Event*, void* user_data) { static_cast<Panda3DImGui*>(user_data)->on_window_resized(); },
        & p3d_imgui);
}

void Demon::setup_imgui_render(Panda3DImGui* panda3d_imgui_helper) {
	
    auto task_mgr = AsyncTaskManager::get_global_ptr();

    // NOTE: ig_loop has process_events and 50 sort.
    PT(GenericAsyncTask) new_frame_imgui_task = new GenericAsyncTask("new_frame_imgui", [](GenericAsyncTask*, void* user_data) {
        static_cast<Panda3DImGui*>(user_data)->new_frame_imgui();
        return AsyncTask::DS_cont;
    }, panda3d_imgui_helper);
    new_frame_imgui_task->set_sort(3);
    task_mgr->add(new_frame_imgui_task);

    PT(GenericAsyncTask) render_imgui_task = new GenericAsyncTask("render_imgui", [](GenericAsyncTask*, void* user_data) {
        static_cast<Panda3DImGui*>(user_data)->render_imgui();
        return AsyncTask::DS_cont;
    }, panda3d_imgui_helper);
    render_imgui_task->set_sort(4);
    task_mgr->add(render_imgui_task);
}

void Demon::setup_imgui_button(Panda3DImGui* panda3d_imgui_helper) {

	auto bt = engine.button_throwers[0];
	auto ev_handler = EventHandler::get_global_event_handler();

	ButtonThrower* bt_node = DCAST(ButtonThrower, bt.node());
	std::string ev_name;

	// ----------------------------------------------
	ev_name = bt_node->get_button_down_event();
	if (ev_name.empty()) {
		ev_name = "imgui-button-down";
		bt_node->set_button_down_event(ev_name);
	}
	
	ev_handler->add_hook(ev_name, [](const Event* ev, void* user_data) {
		const auto& key_name = ev->get_parameter(0).get_string_value();
		const auto& button = ButtonRegistry::ptr()->get_button(key_name);
		static_cast<Panda3DImGui*>(user_data)->on_button_down_or_up(button, true);
	}, panda3d_imgui_helper);

	// ----------------------------------------------
	ev_name = bt_node->get_button_up_event();
	if (ev_name.empty()) {
		ev_name = "imgui-button-up";
		bt_node->set_button_up_event(ev_name);
	}
	
	ev_handler->add_hook(ev_name, [](const Event* ev, void* user_data) {
		const auto& key_name = ev->get_parameter(0).get_string_value();
		const auto& button = ButtonRegistry::ptr()->get_button(key_name);
		static_cast<Panda3DImGui*>(user_data)->on_button_down_or_up(button, false);
	}, panda3d_imgui_helper);

	// ----------------------------------------------
	ev_name = bt_node->get_keystroke_event();
	if (ev_name.empty()) {
		ev_name = "imgui-keystroke";
		bt_node->set_keystroke_event(ev_name);
	}
	
	ev_handler->add_hook(ev_name, [](const Event* ev, void* user_data) {
		wchar_t keycode = ev->get_parameter(0).get_wstring_value()[0];
		static_cast<Panda3DImGui*>(user_data)->on_keystroke(keycode);
	}, panda3d_imgui_helper);
}
