#include <map>
#include <string>
#include <vector>

#include <asyncTask.h>
#include <displayRegion.h>
#include <mouseWatcher.h>
#include <perspectiveLens.h>
#include <orthographicLens.h>
#include <keyboardButton.h>
#include <nodePath.h>

#include "game.h"
#include "demon.h"


// Constructor
Game::Game(Demon* demon) : demon(demon), dr3D(nullptr), dr2D(nullptr) { }

// Initialize the game
void Game::init() {

    // Create display regions
    create_dr3D();
    create_dr2D();

    // 3D render
    render = NodePath("GameRender");
	render.reparent_to(demon->engine.render);

	// 2D render
    render2D = NodePath("GameRender2D");
    render2D.set_depth_test(false);
    render2D.set_depth_write(false);
    render2D.set_material_off(true);
    render2D.set_two_sided(true);
	render2D.reparent_to(demon->engine.render2d);
	
	// aspect 2D
	aspect2D = render2D.attach_new_node("GameAspect2d");
	
	// pixel 2D
	PGTop* pixel2D_ = new PGTop("GamePixel2d");
    pixel2D = render2D.attach_new_node(pixel2D_);
    pixel2D.set_pos(-1, 0, 1);
	
	// 3D camera
	main_cam = NodePath(new Camera("GameCamera2D"));
    main_cam.reparent_to(render);

    PerspectiveLens *perspective_lens = new PerspectiveLens();
    perspective_lens->set_fov(60);
    perspective_lens->set_aspect_ratio(800.0f / 600.0f);
    (DCAST(Camera, main_cam.node()))->set_lens(perspective_lens);
	
	dr3D->set_camera(main_cam);
	
	// 2D camera
	cam2D = NodePath(new Camera("Camera2d"));
    cam2D.reparent_to(render2D);

    OrthographicLens *ortho_lens = new OrthographicLens();
    ortho_lens->set_film_size(2, 2);
    ortho_lens->set_near_far(-1000, 1000);
    (DCAST(Camera, cam2D.node()))->set_lens(ortho_lens);
	
	dr2D->set_camera(cam2D);
	
    // Set up mouse watcher
	mouse_watcher = new MouseWatcher("GameMouse");
	NodePath mouse_watcher_ = demon->engine.mouse_watchers[0].get_parent().attach_new_node(mouse_watcher);
	
	//button_thrower = new ButtonThrower("Button_Thrower_01-game");
	//mouse_watcher_.attach_new_node(button_thrower);
	
    mouse_watcher->set_display_region(dr2D);
	DCAST(PGTop, pixel2D.node())->set_mouse_watcher(mouse_watcher);
	
	LVecBase2i size = demon->engine.get_size();
    if (size.get_x() > 0 && size.get_y() > 0) {
		pixel2D.set_scale(2.0 / size.get_x(), 1.0, 2.0 / size.get_y());
	}
	
    std::cout << "-- Game init successfully" << std::endl;
	
	// on any event
	demon->engine.add_event_hook(GAME_RAW_EVT_IDX,
		[this](const Event* evt, const std::vector<void*>& params)
		{ this->on_evt(evt, params); }
	);

	// test scene
	//NodePath enviro = demon->engine.resourceManager.load_model("models/environment");
	//enviro.reparent_to(render);
	//main_cam.set_pos(0, -155, 0);
	
	//NodePath smiley = demon->engine.resourceManager.load_model("models/smiley");
	//smiley.reparent_to(render);
	//smiley.set_pos(0, 30, 30);
}



void Game::on_evt(const Event* evt, const std::vector<void*>& params) {

	if (evt->get_name() == "window-event") {
		
		LVecBase2i size = demon->engine.get_size();
		
		if (size.get_x() > 0 && size.get_y() > 0) {
			pixel2D.set_scale(2.0 / size.get_x(), 1.0, 2.0 / size.get_y());
		}
		
		float aspect_ratio = demon->engine.get_aspect_ratio();
		if (aspect_ratio == 0)
			return;

		aspect2D.set_scale(1.0f / aspect_ratio, 1.0f, 1.0f);
	}
}

// Create a 3D display region
void Game::create_dr3D() {
	
    dr3D = demon->engine.win->make_display_region(0, 0.4, 0.6, 1);
    dr3D->set_sort(-1);
    dr3D->set_clear_color_active(true);
    dr3D->set_clear_depth_active(true);
    dr3D->set_clear_color(LVecBase4(0.65f, 0.65f, 0.65f, 1.0f));
}

// Create a 2D display region
void Game::create_dr2D() {

    dr2D = demon->engine.win->make_display_region(0, 0.4, 0.6, 1);
    dr2D->set_clear_depth_active(false);
    dr2D->set_sort(10);
    dr2D->set_active(true);
}
