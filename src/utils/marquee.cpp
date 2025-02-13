#include <typeinfo>
#include <algorithm>

#include <geomVertexWriter.h>
#include <geomVertexFormat.h>
#include <geom.h>
#include <geomNode.h>
#include <geomTriangles.h>
#include <shader.h>
#include <transparencyAttrib.h>
#include <mouseWatcher.h>

#include "taskUtils.hpp"
#include "constants.hpp"
#include "engine.h"
#include "marquee.hpp"


// Constructor
Marquee::Marquee(const std::string &name, Engine& engine) : _name(name), engine(engine) {}

// Initialize the marquee system
void Marquee::init(NodePath render) {
    this->render = render;
    // Create a procedural fullscreen quad
    quad = create_fullscreen_quad(_name);
    quad.set_color(1, 1, 1, 0.25f);
    quad.set_transparency(TransparencyAttrib::M_alpha);
    quad.hide();
    quad.reparent_to(engine.render2d);

    // Load and set the shader
    quad.set_shader(Shader::load(Shader::SL_GLSL, "assets/shaders/marquee.vert", "assets/shaders/marquee.frag"));
    quad.set_shader_input("viewport", engine.get_size());
    quad.set_shader_input("start", LVecBase2(0.0, 0.0));
    quad.set_shader_input("end", LVecBase2(0.0, 0.0));
}

// Start the marquee selection
void Marquee::on_start() {
    if (engine.mouse_watcher->has_mouse()) {
        init_mouse_pos = engine.mouse_watcher->get_mouse();

        // Update shader inputs
        quad.set_shader_input("viewport", engine.get_size());
        quad.set_shader_input("start", init_mouse_pos);
        quad.set_shader_input("end", init_mouse_pos);

        quad.show();

        // Add the update task
        if (!AsyncTaskManager::get_global_ptr()->find_task(_name + "-MarqueeTask")) {
            update_task = make_task([this](AsyncTask *task) -> AsyncTask::DoneStatus {
                return on_update();
            }, _name + "-MarqueeTask");
            update_task->set_sort(MARQUEE_TASK_SORT);
            AsyncTaskManager::get_global_ptr()->add(update_task);
        }
    }
}

// Update function for marquee selection
AsyncTask::DoneStatus Marquee::on_update() {
    if (engine.mouse_watcher->has_mouse()) {
        LPoint2f current_mouse_pos = engine.mouse_watcher->get_mouse();
        quad.set_shader_input("end", current_mouse_pos);
    }
    return AsyncTask::DS_cont; // Continue the task
}

// Stop marquee selection and store selected area
void Marquee::on_stop() {
    if (update_task) {
        AsyncTaskManager::get_global_ptr()->remove(update_task);
        update_task = nullptr;
        quad.hide();

        // Store the corners in a vector
        LPoint2f current_mouse_pos = engine.mouse_watcher->get_mouse();

        float x_min = std::min(init_mouse_pos.get_x(), current_mouse_pos.get_x());
        float y_min = std::min(init_mouse_pos.get_y(), current_mouse_pos.get_y());
        float x_max = std::max(init_mouse_pos.get_x(), current_mouse_pos.get_x());
        float y_max = std::max(init_mouse_pos.get_y(), current_mouse_pos.get_y());

        saved_corners = LVector4(x_min, y_min, x_max, y_max);
    }
}

// Get all NodePaths found inside the marquee area
std::vector<NodePath> Marquee::get_found_nps() {
    std::vector<NodePath> nodes_found;
    PT(Camera) cam = DCAST(Camera, engine.scene_cam.node());

    // Extract the bounds of the marquee area
    float x_min = saved_corners.get_x();
    float y_min = saved_corners.get_y();
    float x_max = saved_corners.get_z();
    float y_max = saved_corners.get_w();

    // Find all nodes under render
    NodePathCollection matching_nodes = render.find_all_matches("**"); // "" matches all child nodes
    for (int i = 0; i < matching_nodes.get_num_paths(); ++i) {
        NodePath child = matching_nodes.get_path(i);
        if (child.is_empty()) continue;

        // Get the world position of the NodePath relative to render2d
        LPoint3 np_world_pos = child.get_pos(engine.render2d);
        LPoint3 p3 = engine.scene_cam.get_relative_point(engine.render2d, np_world_pos);

        // Project the 3D point into 2D using the camera's lens
        LPoint2 p2;
        if (!cam->get_lens()->project(p3, p2)) continue;

        if (!(typeid(child) == typeid(NodePath))) continue;

        // Check if the 2D point lies within the bounds
        if (p2.get_x() > x_min && p2.get_x() < x_max &&
            p2.get_y() > y_min && p2.get_y() < y_max) {
            nodes_found.push_back(child);
        }
    }
    return nodes_found;
}

// Create a fullscreen quad for selection overlay
NodePath Marquee::create_fullscreen_quad(const std::string &_name) {
    // Create a vertex format
    GeomVertexFormat* format = const_cast<GeomVertexFormat*>(GeomVertexFormat::get_v3());
    PT(GeomVertexData) vdata = new GeomVertexData(_name, format, Geom::UH_static);

    // Add vertex data
    GeomVertexWriter vertex_writer(vdata, "vertex");
    vertex_writer.add_data3f(-1, 1, -1);  // Bottom-left
    vertex_writer.add_data3f( 1, 1, -1);  // Bottom-right
    vertex_writer.add_data3f( 1, 1,  1);  // Top-right
    vertex_writer.add_data3f(-1, 1,  1);  // Top-left

    // Create geometry
    PT(Geom) geom = new Geom(vdata);
    PT(GeomTriangles) tris = new GeomTriangles(Geom::UH_static);

    // Define the two triangles
    tris->add_vertices(0, 1, 2);
    tris->close_primitive();
    tris->add_vertices(0, 2, 3);
    tris->close_primitive();

    // Add primitive to the geometry
    geom->add_primitive(tris);

    // Create a GeomNode and attach the geometry
    PT(GeomNode) geom_node = new GeomNode(_name);
    geom_node->add_geom(geom);

    // Return the NodePath
    return NodePath(geom_node);
}
