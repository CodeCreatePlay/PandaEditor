#include <typeinfo>

#include <LVector4.h>
#include <camera.h>
#include <geomVertexWriter.h>
#include <geomVertexFormat.h>
#include <geom.h>
#include <geomNode.h>
#include <geomTriangles.h>

#include "taskUtils.hpp"


class Marquee {
public:
    Marquee(const std::string &name) : _name(name) {}
	
	void init(Engine *engine, const NodePath& render) {
		
		_engine = engine;
		_render = !render.is_empty() ? render : _engine->render;

        // Create a procedural fullscreen quad
        quad = create_fullscreen_quad(_name);
		quad.set_color(1, 1, 1, 0.25f);
        quad.set_transparency(TransparencyAttrib::M_alpha);
        quad.hide();

        quad.reparent_to(_engine->render2d);

        // Load and set the shader
        quad.set_shader(Shader::load(Shader::SL_GLSL, "marquee.vert", "marquee.frag"));
		quad.set_shader_input("viewport", _engine->get_size());
        quad.set_shader_input("start",    LVecBase2(0.0, 0.0));
        quad.set_shader_input("end",      LVecBase2(0.0, 0.0));
	}
	
    void on_start() {
        if (_engine->mouse_watcher->has_mouse()) {
			
            init_mouse_pos = _engine->mouse_watcher->get_mouse();

            // Update shader inputs
            quad.set_shader_input("viewport", _engine->get_size());
            quad.set_shader_input("start", init_mouse_pos);
            quad.set_shader_input("end",   init_mouse_pos); // Initially, end matches start

			quad.show();

            // Add the update task
            if (!AsyncTaskManager::get_global_ptr()->find_task(_name + "-" + "MarqueeTask")) {
				
                update_task = (make_task([this](AsyncTask *task) -> AsyncTask::DoneStatus {
                    return on_update();
                }, _name + "-" + "MarqueeTask"));
                update_task->set_sort(1);
				
                AsyncTaskManager::get_global_ptr()->add(update_task);
            }
        }
    }

	AsyncTask::DoneStatus on_update() {
		if (_engine->mouse_watcher->has_mouse()) {
			
			LPoint2f current_mouse_pos = _engine->mouse_watcher->get_mouse();
			quad.set_shader_input("end",   current_mouse_pos);
		}

		return AsyncTask::DS_cont; // Continue the task
	}

    void on_stop() {
        if (update_task) {
			
            AsyncTaskManager::get_global_ptr()->remove(update_task);
            update_task = nullptr;
			
			quad.hide();
			
			// Store the corners in a vector
			LPoint2f current_mouse_pos = _engine->mouse_watcher->get_mouse();
			
			float x_min = std::min(init_mouse_pos.get_x(), current_mouse_pos.get_x());
            float y_min = std::min(init_mouse_pos.get_y(), current_mouse_pos.get_y());
            float x_max = std::max(init_mouse_pos.get_x(), current_mouse_pos.get_x());
            float y_max = std::max(init_mouse_pos.get_y(), current_mouse_pos.get_y());
			
            LVector4 corners(x_min, y_min, x_max, y_max);
			saved_corners = corners;
			
			// get_found_nps();
        }
    }
	
	std::vector<NodePath> get_found_nps() {
		
		std::vector<NodePath> nodes_found;

		Camera* cam = DCAST(Camera, _engine->scene_cam.node());

		// Extract the bounds of the marquee area
		float x_min = saved_corners.get_x();
		float y_min = saved_corners.get_y();
		float x_max = saved_corners.get_z();
		float y_max = saved_corners.get_w();

		// Traverse all nodes_found under _engine->_render
		NodePathCollection matching_nodes = _render.find_all_matches("**");  // "" matches all child nodes

		for (int i = 0; i < matching_nodes.get_num_paths(); ++i) {
			NodePath child = matching_nodes.get_path(i);
			continue;
			if (child.is_empty()) {
				continue;
			}
			
			// Get the world position of the NodePath relative to render2d
			LPoint3 np_world_pos = child.get_pos(_engine->render2d);
			LPoint3 p3 = _engine->scene_cam.get_relative_point(_engine->render2d, np_world_pos);

			// Project the 3D point into 2D using the camera's lens
			LPoint2 p2;
			if (!cam->get_lens()->project(p3, p2)) {
				continue;
			}
			
			if (!(typeid(child) == typeid(NodePath))) {
				continue;
			}
			
			// Check if the 2D point lies within the bounds
			if (p2.get_x() > x_min && p2.get_x() < x_max &&
				p2.get_y() > y_min && p2.get_y() < y_max) {

				nodes_found.push_back(child);
				// std::cout << child.get_name() << std::endl;
			}
		}
		
		return nodes_found;
	}

private:
	NodePath create_fullscreen_quad(const std::string &_name) {
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
		tris->add_vertices(0, 1, 2); // First triangle
		tris->close_primitive();     // Close the first triangle
		tris->add_vertices(0, 2, 3); // Second triangle
		tris->close_primitive();     // Close the second triangle

		// Add primitive to the geometry
		geom->add_primitive(tris);

		// Create a GeomNode and attach the geometry
		PT(GeomNode) geom_node = new GeomNode(_name);
		geom_node->add_geom(geom);

		// Return the NodePath
		return NodePath(geom_node);
	}
	
	std::string _name;
    Engine*     _engine;
	NodePath    _render;
    NodePath    quad;
	LVector4    saved_corners;
	
    PT(AsyncTask) update_task;
    LPoint2f init_mouse_pos;
};
