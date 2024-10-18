#include "include/engine.h"


Engine::Engine() : aspect_ratio(1.0f) {

    data_root = NodePath("DataRoot");

    // get global event queueand handler
    event_queue   = EventQueue::get_global_event_queue();
    event_handler = EventHandler::get_global_event_handler();

    // Initialize Panda3D engine and create window
    create_win();
    setup_input_handling();
    mouse = Mouse(DCAST(GraphicsWindow, win), mouse_watcher);
    create_3d_render();
    create_2d_render();
    create_axis_grid();
    create_default_scene();

    // scene camera needs some references not available at time of its creation,
    // so we set them now.
    scene_cam.initialize(this);

    // reset everything,
    scene_cam.reset();
    reset_clock();
}

Engine::~Engine() {
    exit();
}

void Engine::add_update_callback(void(*callback)()) {
    // Check if the function pointer already exists in the vector
    if (std::find(update_callbacks.begin(), update_callbacks.end(), callback) == update_callbacks.end()) {
        // Add the function pointer if it's not already in the vector
        update_callbacks.push_back(callback);
    }
}

void Engine::create_win() {
    engine = GraphicsEngine::get_global_ptr();
    pipe = GraphicsPipeSelection::get_global_ptr()->make_default_pipe();

    FrameBufferProperties fb_props;
    fb_props.set_rgb_color(true);
    fb_props.set_color_bits(3 * 8);
    fb_props.set_depth_bits(24);
    fb_props.set_back_buffers(1);

    WindowProperties win_props = WindowProperties::get_default();
    GraphicsOutput *output = engine->make_output(pipe, "PandaEditor", 0, fb_props, win_props, GraphicsPipe::BF_require_window);
    win = DCAST(GraphicsWindow, output);
}

void Engine::create_3d_render() {
    dr = win->make_display_region();
    dr->set_clear_color_active(true);
    dr->set_clear_color(LColor(0.3, 0.3, 0.3, 1.0));

    render = NodePath("render");
    render.node()->set_attrib(RescaleNormalAttrib::make_default());
    // render.node()->set_attrib(ShadeModelAttrib::make(ShadeModelAttrib::M_smooth));
    render.set_two_sided(0);

    mouse_watcher->set_display_region(dr);

    scene_cam = SceneCam();
    scene_cam.reparent_to(render);

    dr->set_camera(scene_cam);
}

void Engine::create_2d_render() {
    dr2d = win->make_display_region(0, 1, 0, 1);
    dr2d->set_sort(1);
    dr2d->set_active(true);

    render2d = NodePath("Render2d");
    render2d.set_depth_test(false);
    render2d.set_depth_write(false);

    aspect2d = render2d.attach_new_node("Aspect2d");

    // This special root, pixel2d, uses units in pixels that are relative
    // to the window. The upperleft corner of the window is (0, 0),
    // the lowerleft corner is (xsize, -ysize), in this coordinate system.
    // NodePath());

    PGTop* top_node = new PGTop("my_top_node");
    pixel2d = render2d.attach_new_node(top_node);
    pixel2d.set_pos(-1, 0, 1);

    auto mouse_watcher_node = DCAST(MouseWatcher, mouse_watcher);
    DCAST(PGTop, pixel2d.node())->set_mouse_watcher(mouse_watcher_node);

    LVecBase2i size = get_size();
    if (size.get_x() > 0 && size.get_y() > 0) {
		pixel2d.set_scale(2.0 / size.get_x(), 1.0, 2.0 / size.get_y());
	}

    // 
    cam2d = NodePath(new Camera("Camera2d"));
    cam2d.reparent_to(render2d);

    OrthographicLens *lens = new OrthographicLens();
    lens->set_film_size(2, 2);
    lens->set_near_far(-1000, 1000);
    (DCAST(Camera, cam2d.node()))->set_lens(lens);

    dr2d->set_camera(cam2d);
    mouse_watcher->set_display_region(dr2d);
}

void Engine::create_default_scene() {
    // Create default scene here
}

void Engine::create_axis_grid() {
    axisGrid = AxisGrid();
    axisGrid.create(100, 10, 2);
    axisGrid.set_light_off();
    axisGrid.reparent_to(render);
}

void Engine::reset_clock() {
    ClockObject::get_global_clock()->set_real_time(TrueClock::get_global_ptr()->get_short_time());
    TrueClock::get_global_ptr();
    AsyncTaskManager::get_global_ptr()->set_clock(ClockObject::get_global_clock());
}

void Engine::setup_input_handling() {

    if (!win->is_of_type(GraphicsWindow::get_class_type()) &&
        DCAST(GraphicsWindow, win)->get_num_input_devices() > 0)
        return;

    GraphicsWindow *window = DCAST(GraphicsWindow, win);

    MouseAndKeyboard* mouse_and_keyboard = new MouseAndKeyboard(window, 0, "MouseAndKeyboard_01");
    NodePath mk_node = data_root.attach_new_node(mouse_and_keyboard);

    mouse_watcher = new MouseWatcher("MouseWatcher_01");
    NodePath mouse_watcher_np = mk_node.attach_new_node(mouse_watcher);

    PT(ButtonThrower) button_thrower = new ButtonThrower("Button_Thrower_01");
    NodePath button_thrower_np = mouse_watcher_np.attach_new_node(button_thrower);

    if (win->get_side_by_side_stereo()) {
        mouse_watcher->set_display_region(win->get_overlay_display_region());
    }

    ModifierButtons mb;
    mb.add_button(KeyboardButton::shift());
    mb.add_button(KeyboardButton::control());
    mb.add_button(KeyboardButton::alt());
    mb.add_button(KeyboardButton::meta());
    mouse_watcher->set_modifier_buttons(mb);

    ModifierButtons mods;
    mods.add_button(KeyboardButton::shift());
    mods.add_button(KeyboardButton::control());
    mods.add_button(KeyboardButton::alt());
    mods.add_button(KeyboardButton::meta());
    button_thrower->set_modifier_buttons(mods);

    mouse_watchers.push_back(mouse_watcher_np);
}

void Engine::add_event_hook(int key, std::function<void(const Event*, const std::vector<void*>&)> hook) {
	
	auto result = evt_hooks.emplace(key, hook);
	
	if (result.second) {
		evt_hooks[key] = hook;
    }
	else {
        std::cout << "Unable to set event hook, Key: " << key << " already exists!" << std::endl;
    }
}

void Engine::remove_event_hook(int key) {
	if (evt_hooks.find(key) == evt_hooks.end()) {
		evt_hooks.erase(key);
    }
	else {
        std::cout << "Unable to remove event hook, Key: " << key << " does not exists!" << std::endl;
    }
}

void Engine::process_events(const Event *event) {
    if (!event->get_name().empty()) {

        if (event->get_name() == "window-event") {
            on_evt_size();
        }

        std::vector<void*> param_list;
        for (int i = 0; i < event->get_num_parameters(); ++i) {

            const EventParameter& event_parameter = event->get_parameter(i);

            if (event_parameter.is_int()) {
                param_list.push_back(new int(event_parameter.get_int_value()));
            }
            else if (event_parameter.is_double()) {
                param_list.push_back(new double(event_parameter.get_double_value()));
            }
            else if (event_parameter.is_string()) {
                param_list.push_back(new std::string(event_parameter.get_string_value()));
            }
            else if (event_parameter.is_wstring()) {
                param_list.push_back(new std::wstring(event_parameter.get_wstring_value()));
            }
            else if (event_parameter.is_typed_ref_count()) {
                param_list.push_back(event_parameter.get_typed_ref_count_value());
            }
            else {
                param_list.push_back(event_parameter.get_ptr());
            }
        }
		
		for(const auto& pair : evt_hooks) {
			pair.second(event, param_list);
		}

        if (event_handler) {
            event_handler->dispatch_event(event);
        }
    }
}

void Engine::on_evt_size() {
    get_aspect_ratio();  // update

    if (aspect_ratio == 0)
        return;

    aspect2d.set_scale(1.0f / aspect_ratio, 1.0f, 1.0f);
    if(scene_cam)
        scene_cam.on_resize_event(aspect_ratio);

    LVecBase2i size = get_size();
    if (size.get_x() > 0 && size.get_y() > 0) {
        pixel2d.set_scale(2.0f / size.get_x(), 1.0f, 2.0f / size.get_y());
	}
}

void Engine::update() {
    // keep taskmanager updated
    if (mouse_watcher->has_mouse()) {
        AsyncTaskManager::get_global_ptr()->poll();
    }
    
    // traverse the data graph.This reads all the control
    // inputs(from the mouse and keyboard, for instance) and also
    // directly acts upon them(for instance, to move the avatar).
    data_graph_trav.traverse(data_root.node());

    // process events
    while (!event_queue->is_queue_empty()) {
        process_events(event_queue->dequeue_event());
    }

    //
    mouse.update();
    scene_cam.update();

    // update callbacks
    for (const auto& callback : update_callbacks)
        callback();

    // finally, render frame
    engine->render_frame();
}

float Engine::get_aspect_ratio() {
    aspect_ratio = static_cast<float>(win->get_sbs_left_x_size()) / static_cast<float>(win->get_sbs_left_y_size());
    return aspect_ratio;
}

LVecBase2i Engine::get_size() {
    if (win != nullptr) {

        if (DCAST(GraphicsWindow, win) && win->has_size()) {
            GraphicsWindow* gwin = DCAST(GraphicsWindow, win);
            LVecBase2i size = gwin->get_size();
			return LVecBase2i(size.get_x(), size.get_y());
        }
    }

    WindowProperties props = WindowProperties::get_default();
    return LVecBase2i(props.get_x_size(), props.get_y_size());
}

void Engine::exit() {
    win->set_active(false);
    engine->remove_window(win);
}
