#include <cmath>
#include <lquaternion.h>

#include "include/engine.h"
#include "include/sceneCam.h"


SceneCam::SceneCam(float speed, const LVecBase3f& default_pos)
    : speed(speed), default_pos(default_pos) {
    axes = nullptr;

    // Create a new camera
    PT(Camera) cam_node = new Camera("SceneCamera");
    cam_np = NodePath(cam_node);

    // Create a lens for the camera
    PT(PerspectiveLens) lens = new PerspectiveLens();
    lens->set_fov(60);
    lens->set_aspect_ratio(800.0f / 600.0f);
    cam_node->set_lens(lens);

    // Initialize the NodePath with the camera
    NodePath::operator=(cam_np);

    // Create a target to orbit around
    target = new NodePath("TargetNode");
}

NodePath* SceneCam::create_axes(float thickness, float length) {
    // Build line segments
    LineSegs ls;
    ls.set_thickness(thickness);

    // X Axis - Red
    ls.set_color(1.0f, 0.0f, 0.0f, 1.0f);
    ls.move_to(0.0f, 0.0f, 0.0f);
    ls.draw_to(length, 0.0f, 0.0f);

    // Y Axis - Green
    ls.set_color(0.0f, 1.0f, 0.0f, 1.0f);
    ls.move_to(0.0f, 0.0f, 0.0f);
    ls.draw_to(0.0f, length, 0.0f);

    // Z Axis - Blue
    ls.set_color(0.0f, 0.0f, 1.0f, 1.0f);
    ls.move_to(0.0f, 0.0f, 0.0f);
    ls.draw_to(0.0f, 0.0f, length);

    return new NodePath(ls.create());
}

void SceneCam::initialize(Engine* engine) {
    win = DCAST(GraphicsWindow, engine->win);
    mouse_watcher_node = engine->mouse_watcher;
    aspect2d = &engine->aspect2d;
    mouse = &engine->mouse;

    axes = create_axes();
    axes->set_name("SceneCameraAxes");
    axes->reparent_to(*aspect2d);
    axes->set_scale(0.008f);
}

void SceneCam::move(const LVecBase3f& move_vec) {
    // Modify the move vector by the distance to the target
    LVecBase3f camera_vec = get_pos() - target->get_pos();
    LVecBase3f modified_move_vec = move_vec * (camera_vec.length() / 300.0f);

    set_pos(*this, modified_move_vec);

    // Move the target so it stays with the camera
    target->set_quat(get_quat());

    LVecBase3f target_move_vec(modified_move_vec.get_x(), 0, modified_move_vec.get_z());
    target->set_pos(*target, target_move_vec);
}

void SceneCam::orbit(const LVecBase2f& delta) {
    // Get new hpr
    LVecBase3f hpr = get_hpr();
    hpr.set_x(hpr.get_x() + delta.get_x());
    hpr.set_y(hpr.get_y() + delta.get_y());

    // Set camera to new hpr
    set_hpr(hpr);

    // Get the H and P in radians
    float rad_x = hpr.get_x() * (M_PI / 180.0f);
    float rad_y = hpr.get_y() * (M_PI / 180.0f);

    // Get distance from camera to target
    LVecBase3f camera_vec = get_pos() - target->get_pos();
    float cam_vec_dist = camera_vec.length();

    // Get new camera pos
    LVecBase3f new_pos;
    new_pos.set_x(cam_vec_dist * sin(rad_x) * cos(rad_y));
    new_pos.set_y(-cam_vec_dist * cos(rad_x) * cos(rad_y));
    new_pos.set_z(-cam_vec_dist * sin(rad_y));
    new_pos += target->get_pos();

    // Set camera to new pos
    set_pos(new_pos);
}

void SceneCam::update() {
    if (!mouse_watcher_node->has_mouse() ||
        !mouse_watcher_node->is_button_down(KeyboardButton::alt())) {
        return;
    }

    // Orbit - If left input down
    if (mouse->is_button_down(0))
        orbit(LVecBase2f(mouse->get_dx() * speed, mouse->get_dy() * speed));
    
    // Dolly - If middle input down
    else if (mouse->is_button_down(1)) {
        move(LVecBase3f(mouse->get_dx() * speed, 0, -mouse->get_dy() * speed));
    }

    // Zoom - If right input down
    else if (mouse->is_button_down(2)) {
        move(LVecBase3f(0, -mouse->get_dx() * speed, 0));
    }
    
    update_axes();
}

void SceneCam::update_axes() {
    // Set rotation to inverse of camera rotation
    float aspect = static_cast<float>(win->get_sbs_left_x_size()) / static_cast<float>(win->get_sbs_left_y_size());
    axes->set_pos(aspect - 0.25f, 0.0f, 1.0f - 0.25f);

    LQuaternion camera_quat(get_quat());
    camera_quat.invert_in_place();

    axes->set_quat(camera_quat);
}

void SceneCam::on_resize_event(float aspect_ratio) {
    Lens* lens = DCAST(Camera, cam_np.node())->get_lens();

    if (lens) {
        DCAST(PerspectiveLens, lens)->set_aspect_ratio(aspect_ratio);
    }

    update_axes();
}

void SceneCam::reset() {
    // Reset camera and target back to default positions
    target->set_pos(LVecBase3f(0, 0, 0));
    set_pos(default_pos);

    // Set camera to look at target
    look_at(target->get_pos());
    target->set_quat(get_quat());

    update_axes();
}