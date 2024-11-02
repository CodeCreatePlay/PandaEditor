#include <genericAsyncTask.h>
#include <config_putil.h>
#include "include/demon.h"


Demon::Demon() {
	setup_paths();

	engine.add_event_hook(0, [this](const Event* evt, const std::vector<void*>& params) {
		this->on_evt(evt, params);
	});
}

Demon::~Demon() {}

void Demon::on_evt(const Event* evt, const std::vector<void*>&) {}

void Demon::update() {
    engine.update();
}

void Demon::setup_paths() {
	std::string path = PathUtils::get_current_working_dir();
	get_model_path().prepend_directory(Filename::from_os_specific(path));
}

/*
imgui integration

Demon::Demon() : p3d_imgui(DCAST(GraphicsWindow, engine.win), engine.pixel2d) {

	setup_paths();
	setup_imgui();
	
   engine.set_event_hook([this](const Event* evt, const std::vector<void*>& params) {
		this->on_evt(evt, params);
	});
}

void Demon::setup_imgui() {
	// setup ImGUI for Panda3D
	p3d_imgui.setup_style();
    p3d_imgui.setup_geom();
    p3d_imgui.setup_shader(Filename("src/p3d_imgui/shader"));
    p3d_imgui.setup_font();
    p3d_imgui.setup_event();

    p3d_imgui.enable_file_drop();
	
	setup_imgui_render(&p3d_imgui);
	setup_imgui_button(&p3d_imgui);

    EventHandler::get_global_event_handler()->add_hook(
        "window-event",
        [](const Event*, void* user_data) { static_cast<Panda3DImGui*>(user_data)->on_window_resized(); },
        & p3d_imgui);

    EventHandler::get_global_event_handler()->add_hook
	(Panda3DImGui::NEW_FRAME_EVENT_NAME, [](const Event*) { on_imgui_new_frame(); });
}

void Demon::setup_imgui_render(Panda3DImGui* panda3d_imgui_helper) {
    auto task_mgr = AsyncTaskManager::get_global_ptr();

    // NOTE: ig_loop has process_events and 50 sort.
    PT(GenericAsyncTask) new_frame_imgui_task = new GenericAsyncTask("new_frame_imgui", [](GenericAsyncTask*, void* user_data) {
        static_cast<Panda3DImGui*>(user_data)->new_frame_imgui();
        return AsyncTask::DS_cont;
    }, panda3d_imgui_helper);
    new_frame_imgui_task->set_sort(0);
    task_mgr->add(new_frame_imgui_task);

    PT(GenericAsyncTask) render_imgui_task = new GenericAsyncTask("render_imgui", [](GenericAsyncTask*, void* user_data) {
        static_cast<Panda3DImGui*>(user_data)->render_imgui();
        return AsyncTask::DS_cont;
    }, panda3d_imgui_helper);
    render_imgui_task->set_sort(40);
    task_mgr->add(render_imgui_task);
}

void Demon::setup_imgui_button(Panda3DImGui* panda3d_imgui_helper) {
    if (auto bt = window_framework->get_mouse().find("kb-events"))
    {
        auto ev_handler = EventHandler::get_global_event_handler();

        ButtonThrower* bt_node = DCAST(ButtonThrower, bt.node());
        std::string ev_name;

        ev_name = bt_node->get_button_down_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-button-down";
            bt_node->set_button_down_event(ev_name);
        }
        ev_handler->add_hook(ev_name, [](const Event* ev, void* user_data) {
            const auto& key_name = ev->get_parameter(0).get_string_value();
            const auto& button = ButtonRegistry::ptr()->get_button(key_name);
            static_cast<Panda3DImGui*>(user_data)->on_button_down_or_up(button, true);
        }, panda3d_imgui_helper);

        ev_name = bt_node->get_button_up_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-button-up";
            bt_node->set_button_up_event(ev_name);
        }
        ev_handler->add_hook(ev_name, [](const Event* ev, void* user_data) {
            const auto& key_name = ev->get_parameter(0).get_string_value();
            const auto& button = ButtonRegistry::ptr()->get_button(key_name);
            static_cast<Panda3DImGui*>(user_data)->on_button_down_or_up(button, false);
        }, panda3d_imgui_helper);

        ev_name = bt_node->get_keystroke_event();
        if (ev_name.empty())
        {
            ev_name = "imgui-keystroke";
            bt_node->set_keystroke_event(ev_name);
        }
        ev_handler->add_hook(ev_name, [](const Event* ev, void* user_data) {
            wchar_t keycode = ev->get_parameter(0).get_wstring_value()[0];
            static_cast<Panda3DImGui*>(user_data)->on_keystroke(keycode);
        }, panda3d_imgui_helper);
    }
}

void Demon::on_imgui_new_frame()
{
    static bool show_demo_window = true;
    static bool show_another_window = false;
    static LVecBase3f clear_color = LVecBase3f(0);

    // 1. Show a simple window.
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets automatically appears in a window called "Debug".
    {
        static float f = 0.0f;
        static int counter = 0;
        ImGui::Text("Hello, world!");                              // Display some text (you can use a format string too)
        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);               // Edit 1 float using a slider from 0.0f to 1.0f    
        ImGui::ColorEdit3("clear color", (float*)&clear_color[0]); // Edit 3 floats representing a color

        ImGui::Checkbox("Demo Window", &show_demo_window);         // Edit bools storing our windows open/close state
        ImGui::Checkbox("Another Window", &show_another_window);

        if (ImGui::Button("Button"))                               // Buttons return true when clicked (NB: most widgets return true when edited/activated)
            counter++;
        ImGui::SameLine();
        ImGui::Text("counter = %d", counter);

        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    }

    // 2. Show another simple window. In most cases you will use an explicit Begin/End pair to name your windows.
    if (show_another_window)
    {
        ImGui::Begin("Another Window", &show_another_window);
        ImGui::Text("Hello from another window!");
        if (ImGui::Button("Close Me"))
            show_another_window = false;
        ImGui::End();
    }

    // 3. Show the ImGui demo window. Most of the sample code is in ImGui::ShowDemoWindow(). Read its code to learn more about Dear ImGui!
    if (show_demo_window)
    {
        ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_FirstUseEver); // Normally user code doesn't need/want to call this because positions are saved in .ini file anyway. Here we just want to make the demo initial state a bit more friendly!
        ImGui::ShowDemoWindow(&show_demo_window);
    }
}
*/
