#include <string>
#include <unordered_map>

#include <genericAsyncTask.h>
#include <bitMask.h>
#include <camera.h>
#include <nodePath.h>
#include "engine.hpp"
#include "mousePicker.hpp"


MousePicker::MousePicker(const std::string& name, Engine& engine)
    : _name(name), _engine(engine), _last_x(-1.0f), _last_y(-1.0f) {}

void MousePicker::init() {
    BitMask32 from_collide_mask = BitMask32::all_on();

    // Create collision ray
    _picker_ray = new CollisionRay();
    _coll_handler = new CollisionHandlerQueue();

    // Create collision node
    PT(CollisionNode) picker_node = new CollisionNode(_name + " CollisionNode");
    picker_node->add_solid(_picker_ray);
    picker_node->set_into_collide_mask(BitMask32::all_off());
    picker_node->set_from_collide_mask(from_collide_mask);

    NodePath picker_np = _engine.scene_cam.attach_new_node(picker_node);

    // Add picker node to collision traverser
    _traverser.add_collider(picker_np, _coll_handler);

    // Bind mouse button events (commented out)
    /*
    for (const auto& event_name : {"mouse1", "control-mouse1", "mouse1-up"}) {
        // System::demon->accept(event_name, [this, event_name]() { this->fire_event(event_name); });
    }
    */
}

AsyncTask::DoneStatus MousePicker::update(GenericAsyncTask* task, float x, float y) {
    if (_engine.mouse_watcher->has_mouse()) {
        x = _engine.mouse_watcher->get_mouse().get_x();
        y = _engine.mouse_watcher->get_mouse().get_y();
    }

    if (x == _last_x && y == _last_y) {
        return AsyncTask::DS_cont;
    }

    _last_x = x;
    _last_y = y;

    _coll_handler->clear_entries();
    _picker_ray->set_from_lens(DCAST(Camera, _engine.scene_cam.node()), x, y);

    // Traverse the hierarchy and find collisions
    _traverser.traverse(_engine.render);

    if (_coll_handler->get_num_entries() > 0) {
        _coll_handler->sort_entries();
        PT(CollisionEntry) coll_entry = _coll_handler->get_entry(0);
        NodePath node = NodePath(coll_entry->get_into_node());

        if (node != _node) {
            if (!_node.is_empty()) {
                // System::evt_mgr->trigger(_node.get_name() + "-mouse-leave");
            }
            // System::evt_mgr->trigger(node.get_name() + "-mouse-enter", coll_entry);
        }

        // System::evt_mgr->trigger(node.get_name() + "-mouse-over", coll_entry);

        _coll_entry = coll_entry;
        _node = node;
    } else if (!_node.is_empty()) {
        // System::evt_mgr->trigger(_node.get_name() + "-mouse-leave");
        _node = NodePath();
    }

    return AsyncTask::DS_cont;
}

void MousePicker::fire_event(const std::string& event) {
    if (!_node.is_empty()) {
        // System::evt_mgr->trigger(_node.get_name() + "-" + event, _coll_entry);
    }
}

NodePath MousePicker::get_first_np() {
    if (_coll_handler->get_num_entries() > 0) {
        PT(CollisionEntry) coll_entry = _coll_handler->get_entry(0);
        _node = coll_entry->get_into_node_path();
        return _node;
    }
    return NodePath();
}
