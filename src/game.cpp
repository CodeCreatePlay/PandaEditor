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

#include "game.hpp"
#include "demon.hpp"

// Constructor
Game::Game(Demon& demon) : demon(demon) { }

// Initialize the game
void Game::init() {
    create_dr3D();
    create_dr2D();

    // 3D Render
    render = NodePath("GameRender");
    render.reparent_to(demon.engine.render);

    // 2D Render
    render2D = NodePath("GameRender2D");
    render2D.set_depth_test(false);
    render2D.set_depth_write(false);
    render2D.set_material_off(true);
    render2D.set_two_sided(true);
    render2D.reparent_to(demon.engine.render2D);
	
    // Aspect 2D
    aspect2D = render2D.attach_new_node("GameAspect2d");

    // Pixel 2D
    auto pixel2D_ = new PGTop("GamePixel2d");
    pixel2D = render2D.attach_new_node(pixel2D_);
    pixel2D.set_pos(-1, 0, 1);

    // 3D Camera
    main_cam = NodePath(new Camera("GameCamera2D"));
    main_cam.reparent_to(render);

    auto perspective_lens = new PerspectiveLens();
    perspective_lens->set_fov(60);
    perspective_lens->set_aspect_ratio(800.0f / 600.0f);
    DCAST(Camera, main_cam.node())->set_lens(perspective_lens);
	
    dr3D->set_camera(main_cam);
	
    // 2D Camera
    cam2D = NodePath(new Camera("Camera2d"));
    cam2D.reparent_to(render2D);

    auto ortho_lens = new OrthographicLens();
    ortho_lens->set_film_size(2, 2);
    ortho_lens->set_near_far(-1000, 1000);
    DCAST(Camera, cam2D.node())->set_lens(ortho_lens);
	
    dr2D->set_camera(cam2D);
	
    // Mouse Watcher
    mouse_watcher = new MouseWatcher("GameMouse");

    if (!demon.engine.mouse_watchers.empty()) {
        auto mouse_watcher_ = demon.engine.mouse_watchers[0].get_parent().attach_new_node(mouse_watcher);
        mouse_watcher->set_display_region(dr2D);
    }

    DCAST(PGTop, pixel2D.node())->set_mouse_watcher(mouse_watcher);

    auto size = demon.engine.get_size();
    if (size.get_x() > 0 && size.get_y() > 0) {
        pixel2D.set_scale(2.0 / size.get_x(), 1.0, 2.0 / size.get_y());
    }
		
	// Define MouseWatcherRegion
	float view_size = demon.default_settings.game_view_size;
	
    float mwr_right = 2 * view_size - 1;
    float mwr_top   = 2 * view_size - 1;
	
	mouse_region = new MouseWatcherRegion(
        "GameMouseWatcherRegion", -1, mwr_right, -1, mwr_top
    );
	
	demon.engine.mouse_watcher->add_region(mouse_region);
	
    // Subscribe to events
    demon.engine.accept([this](const std::string& event_name) { this->on_evt(event_name); });
	
	// Everything done
	std::cout << "-- Game initialized successfully" << std::endl;
}

// Handle events
void Game::on_evt(const std::string& event_name) {
    if (event_name == "window-event") {
        auto size = demon.engine.get_size();
		
        if (size.get_x() > 0 && size.get_y() > 0) {
            pixel2D.set_scale(2.0 / size.get_x(), 1.0, 2.0 / size.get_y());
        }
		
        float aspect_ratio = demon.engine.get_aspect_ratio();
        if (aspect_ratio != 0) {
            aspect2D.set_scale(1.0f / aspect_ratio, 1.0f, 1.0f);
        }
    }
}

// Create a 3D display region
void Game::create_dr3D() {	
	float size = demon.default_settings.game_view_size;
	
	// left-right-bottom-top
    dr3D = demon.engine.win->make_display_region(0, size, 0, size);
    dr3D->set_sort(GAME_DR_3D_SORT);
    dr3D->set_clear_color_active(true);
    dr3D->set_clear_depth_active(true);
    dr3D->set_clear_color(LVecBase4(0.65f, 0.65f, 0.65f, 1.0f));
}

// Create a 2D display region
void Game::create_dr2D() {
	float size = demon.default_settings.game_view_size;
	
    dr2D = demon.engine.win->make_display_region(0, size, 0, size);
    dr2D->set_clear_depth_active(false);
    dr2D->set_sort(GAME_DR_2D_SORT);
    dr2D->set_active(true);
}
