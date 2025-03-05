#include "engine.hpp"
#include "constants.hpp"

int MOUSE_ALT  = KeyboardButton::alt().get_index();
int MOUSE_CTRL = KeyboardButton::control().get_index();

Engine::Engine() : mouse(*this), scene_cam(*this) {

    data_root = NodePath("DataRoot");

    // get global event queueand handler
    event_queue   = EventQueue::get_global_event_queue();
    event_handler = EventHandler::get_global_event_handler();

    // Initialize Panda3D engine and create window
    create_win();
    setup_mouse_keyboard(mouse_watcher);

    mouse.initialize();
	
    create_3d_render();
    create_2d_render();
    create_axis_grid();
    create_default_scene();

    // scene camera needs some references not available at time of its creation,
    // so we init them now.
    scene_cam.initialize();

    // reset everything,
    scene_cam.reset();
    reset_clock();
}

Engine::~Engine() {}

void Engine::create_win() {
    engine = GraphicsEngine::get_global_ptr();
    pipe = GraphicsPipeSelection::get_global_ptr()->make_default_pipe();

    FrameBufferProperties fb_props;
    fb_props.set_rgb_color(true);
    fb_props.set_color_bits(3 * 8);
    fb_props.set_depth_bits(24);
    fb_props.set_back_buffers(1);

    WindowProperties win_props = WindowProperties::get_default();
    GraphicsOutput *output = engine->make_output(
		pipe,
		"PandaEditor",
		0,
		fb_props,
		win_props,
		GraphicsPipe::BF_require_window);
    win = DCAST(GraphicsWindow, output);
}

void Engine::create_3d_render() {
    dr = win->make_display_region();
    dr->set_clear_color_active(true);
    dr->set_clear_color(LColor(0.3, 0.3, 0.3, 1.0));
	dr->set_sort(ENGINE_DR_3D_SORT);

    render = NodePath("render");
    render.node()->set_attrib(RescaleNormalAttrib::make_default());
    render.set_two_sided(0);
	render.set_shader_auto(true);

    mouse_watcher->set_display_region(dr);

    scene_cam.reparent_to(render);

    dr->set_camera(scene_cam);
}

void Engine::create_2d_render() {
    dr2D = win->make_display_region(0, 1, 0, 1);
    dr2D->set_sort(ENGINE_DR_2D_SORT);
    dr2D->set_active(true);

    render2D = NodePath("Render2d");
	render2D.set_depth_test(false);
    render2D.set_depth_write(false);

    aspect2D = render2D.attach_new_node("Aspect2d");

    // This special root, pixel2D, uses units in pixels that are relative
    // to the window. The upperleft corner of the window is (0, 0),
    // the lowerleft corner is (xsize, -ysize), in this coordinate system.
    // NodePath());

    PGTop* pixel2D_ = new PGTop("Pixel2d");
    pixel2D = render2D.attach_new_node(pixel2D_);
    pixel2D.set_pos(-1, 0, 1);

    auto mouse_watcher_node = DCAST(MouseWatcher, mouse_watcher);
    DCAST(PGTop, pixel2D.node())->set_mouse_watcher(mouse_watcher_node);

    LVecBase2i size = get_size();
    if (size.get_x() > 0 && size.get_y() > 0) {
		pixel2D.set_scale(2.0 / size.get_x(), 1.0, 2.0 / size.get_y());
	}

    // 
    cam2D = NodePath(new Camera("Camera2d"));
    cam2D.reparent_to(render2D);

    OrthographicLens *lens = new OrthographicLens();
    lens->set_film_size(2, 2);
    lens->set_near_far(-1000, 1000);
    (DCAST(Camera, cam2D.node()))->set_lens(lens);

    dr2D->set_camera(cam2D);
    mouse_watcher->set_display_region(dr2D);
}

void Engine::create_default_scene() {}

void Engine::create_axis_grid() {
    axis_grid = AxisGrid(100, 10, 2);
    axis_grid.create();
    axis_grid.set_light_off();
    axis_grid.reparent_to(render);
}

void Engine::setup_mouse_keyboard(PT(MouseWatcher)& mw) {
    if (!win->is_of_type(GraphicsWindow::get_class_type()) &&
        DCAST(GraphicsWindow, win)->get_num_input_devices() > 0)
        return;

    GraphicsWindow *window = DCAST(GraphicsWindow, win);
	
    int device_idx = data_root.get_num_children();
	
    PT(MouseAndKeyboard) mouse_and_keyboard = new MouseAndKeyboard(window, device_idx, win->get_input_device_name(device_idx));
    PT(MouseWatcher) mouse_watcher = new MouseWatcher("MouseWatcher");
    PT(ButtonThrower) button_thrower = new ButtonThrower("Button_Thrower");
		
    NodePath mk_node = data_root.attach_new_node(mouse_and_keyboard);
    NodePath mouse_watcher_np = mk_node.attach_new_node(mouse_watcher);
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
    button_throwers.push_back(button_thrower_np);

    // Assign the reference-counted MouseWatcher to mw
    mw = mouse_watcher;
}

void Engine::process_events(CPT_Event event) {
    if (!event->get_name().empty()) {

		// std::cout << "EventGenerated: " << event->get_name() << std::endl;

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

        if (event_handler) {
            event_handler->dispatch_event(event);
        }
		
		panda_events.emplace_back(event, param_list);
    }
}

void Engine::update() {
    // traverse the data graph.This reads all the control
    // inputs(from the mouse and keyboard, for instance) and also
    // directly acts upon them(for instance, to move the avatar).
    data_graph_trav.traverse(data_root.node());

    // process events
    while (!event_queue->is_queue_empty()) {
        process_events(event_queue->dequeue_event());
    }

    // update mouse and camera
    mouse.update();
    scene_cam.update();
}

void Engine::on_evt_size() {
    float aspect_ratio = get_aspect_ratio();
    if (aspect_ratio == 0)
        return;

    aspect2D.set_scale(1.0f / aspect_ratio, 1.0f, 1.0f);
    if(scene_cam)
        scene_cam.on_resize_event(aspect_ratio);

    LVecBase2i size = get_size();
    if (size.get_x() > 0 && size.get_y() > 0) {
        pixel2D.set_scale(2.0f / size.get_x(), 1.0f, 2.0f / size.get_y());
	}
	
	should_repaint = true;
}

void Engine::reset_clock() {
    ClockObject::get_global_clock()->set_real_time(TrueClock::get_global_ptr()->get_short_time());
	ClockObject::get_global_clock()->tick();
    AsyncTaskManager::get_global_ptr()->set_clock(ClockObject::get_global_clock());
}

void Engine::clean_up() {
	// 1. Remove all tasks
    // Retrieve all tasks currently managed
    AsyncTaskCollection all_tasks = AsyncTaskManager::get_global_ptr()->get_tasks();

    // Remove each task
    for (size_t i = 0; i < all_tasks.get_num_tasks(); ++i) {
        PT(AsyncTask) task = all_tasks.get_task(i);
        AsyncTaskManager::get_global_ptr()->remove(task);
    }
	
	// 2. Empty event queue and remove event hooks
	event_queue->clear();
	EventHandler::get_global_event_handler()->remove_all_hooks();
	
	// 3. Remove render and render 2D
	render.remove_node();
	render2D.remove_node();
	
	// 4. Destroy loader
	Loader::get_global_ptr()->stop_threads();

	// 5. Clear render textures
	GraphicsOutput *output = DCAST(GraphicsOutput, win);
	output->clear_render_textures();
	
	// 6. Remove all windows
    win->set_active(false);
    // engine->remove_all_windows();
}

void Engine::accept(const std::string& event_name, std::function<void()> callback) {
	event_map[event_name].push_back(callback);
}

void Engine::accept(std::function<void(std::string event_name)> callback) {
	unnamed_events.push_back(callback);
}

void Engine::trigger(const std::string& event_name) {
	if (event_map.find(event_name) != event_map.end()) {
		for (auto& callback : event_map[event_name]) {
			callback();
		}
	}
}

void Engine::dispatch_event(std::string evt_name) {
	Engine::trigger(evt_name);
}

void Engine::dispatch_events(bool ignore_mouse) {
	const Event* event;
	for (auto it = panda_events.begin(); it != panda_events.end(); ++it) {
		
		event = it->first.p();

		// send raw event hooks
		for (const auto& callback : unnamed_events) {
			callback(event->get_name());  // Call the callback with the argument
		}

		// other
		if(ignore_mouse && event->get_name().substr(0, 5) == "mouse")
			continue;
		
		// trigger named events
		Engine::trigger(event->get_name());
	}
	
	panda_events.clear();
}

float Engine::get_aspect_ratio() {
    return static_cast<float>(win->get_sbs_left_x_size()) / static_cast<float>(win->get_sbs_left_y_size());
}

LVecBase2i Engine::get_size() {
    if (win != nullptr) {
        if (DCAST(GraphicsWindow, win) && win->has_size()) {
			return LVecBase2i(win->get_sbs_left_x_size(), win->get_sbs_left_y_size());
        }
    }

    return LVecBase2i(win->get_sbs_left_x_size(), win->get_sbs_left_y_size());
}
