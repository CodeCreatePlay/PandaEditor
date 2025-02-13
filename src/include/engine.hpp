#ifndef ENGINE_H
#define ENGINE_H

#include <vector>
#include <functional>

// Core/Utility headers
#include <clockObject.h>
#include <trueClock.h>
#include <eventQueue.h>
#include <eventHandler.h>
// Graphics System headers
#include <windowProperties.h>
#include <frameBufferProperties.h>
#include <graphicsPipe.h>
#include <graphicsPipeSelection.h>
#include <graphicsEngine.h>
// camera and lenses
#include <camera.h>
#include <orthographicLens.h>
// Input and Event-Handling headers
#include <mouseWatcher.h>
#include <mouseAndKeyboard.h>
#include <keyboardButton.h>
#include <buttonThrower.h>
// Scene Graph and Rendering headers
#include <nodePath.h>
#include <pgTop.h>
#include <dataGraphTraverser.h>
#include <dataNodeTransmit.h>
// Asynchronous Task Management
#include <asyncTaskManager.h>
#include <genericAsyncTask.h>
// other
#include "sceneCam.hpp"
#include "axisGrid.hpp"
#include "resourceManager.hpp"
#include "mouse.hpp"


extern int MOUSE_ALT;
extern int MOUSE_CTRL;

class Engine {
public:
    Engine();
    ~Engine();

    // fields
	PT(GraphicsPipe)      pipe;
    PT(GraphicsEngine)    engine;
    PT(GraphicsWindow)    win;
    PT(DisplayRegion)     dr;
    PT(DisplayRegion)     dr2d;

    PT(MouseWatcher)      mouse_watcher;
	
    std::vector<NodePath> mouse_watchers;
	std::vector<NodePath> button_throwers;

    NodePath              data_root;
    DataGraphTraverser    data_graph_trav;
    EventQueue*           event_queue;
    EventHandler*         event_handler;

    NodePath              render;
    SceneCam              scene_cam;
    NodePath              render2d;
    NodePath              aspect2d;
    NodePath              pixel2d;
    NodePath              cam2d;

    Mouse                 mouse;
    ResourceManager       resourceManager;
    AxisGrid              axisGrid;
	
	std::unordered_map<std::string, std::vector<std::function<void()>>> event_map;
    std::vector<std::function<void(std::string event_name)>>            unnamed_events;
	
    bool should_repaint;
	
    // methods
	void accept(const std::string& event_name, std::function<void()> callback);
    void accept(std::function<void(std::string event_name)> callback);
	void clean_up();
	void dispatch_event(std::string evt_name);
	void dispatch_events(bool ignore_mouse = false);
	void on_evt_size();
	void trigger(const std::string& event_name);
	void update();

	float get_aspect_ratio();
    LVecBase2i get_size();
	
private:
    void create_win();
    void create_3d_render();
    void create_2d_render();
    void create_default_scene();
    void create_axis_grid();
	void process_events(CPT_Event event);
	void reset_clock();
	void setup_mouse_keyboard(PT(MouseWatcher)& mw);
		
	// cache
	std::vector<std::pair<CPT_Event, std::vector<void*>>> panda_events;
};

#endif