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
#include "sceneCam.h"
#include "axisGrid.h"
#include "resourceManager.h"
#include "mouse.h"


class Engine {
public:
    Engine();
    ~Engine();

    // fields
    GraphicsEngine*  engine;
    PT(GraphicsPipe) pipe;
    GraphicsWindow*  win = nullptr;
    DisplayRegion*   dr;
	
    DisplayRegion* dr2d;
    MouseWatcher* mouse_watcher_2d;

    MouseWatcher* mouse_watcher = nullptr;
    std::vector<NodePath> mouse_watchers;
	std::vector<NodePath> button_throwers;

    NodePath data_root;
    DataGraphTraverser data_graph_trav;
    EventQueue* event_queue;
    EventHandler* event_handler;

    NodePath render;
    SceneCam scene_cam;
    NodePath render2d;
    NodePath aspect2d;
    NodePath pixel2d;
    NodePath cam2d;

    Mouse mouse;
    ResourceManager resourceManager;
    AxisGrid axisGrid;

    // methods
	void add_update_callback(void(*callback)());
    void update();

    float get_aspect_ratio();
    LVecBase2i get_size();
	
	void setup_mouse_keyboard(MouseWatcher*& mw);
	
    void add_event_hook(int key, std::function<void(const Event*, const std::vector<void*>&)> hook);
	void remove_event_hook(int key);

    void clean_up();

private:
    void create_win();
    void create_3d_render();
    void create_2d_render();
    void create_default_scene();
    void create_axis_grid();
    void reset_clock();
    void process_events(const Event* event);
    void on_evt_size();

    float aspect_ratio;
    std::vector<void(*)()> update_callbacks;
	std::unordered_map<int, std::function<void(const Event*, const std::vector<void*>&)>> evt_hooks;
};

#endif
