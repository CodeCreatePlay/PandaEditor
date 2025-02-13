#include <geomNode.h>
#include "axisGrid.hpp"


AxisGrid::AxisGrid(float grid_size, float grid_step, int sub_divisions)
    : NodePath("AxisGrid"),
      grid_size(grid_size),
      grid_step(grid_step),
      sub_divisions(sub_divisions),
      show_end_cap_lines(true),
      x_axis_color(1, 0, 0, 1),
      y_axis_color(0, 1, 0, 1),
      grid_color(0.4, 0.4, 0.4, 1),
      sub_div_color(0.35, 0.35, 0.35, 1),
      axis_thickness(1),
      grid_thickness(1),
      sub_div_thickness(1) {}

void AxisGrid::create() {
    // Set thickness
    axis_lines.set_thickness(axis_thickness);
    grid_lines.set_thickness(grid_thickness);
    sub_division_lines.set_thickness(sub_div_thickness);

    // Set colors
    grid_lines.set_color(grid_color);
    sub_division_lines.set_color(sub_div_color);

    // Generate grid lines
    _draw_grid_lines(grid_step, grid_lines);
    _draw_grid_lines(grid_step / sub_divisions, sub_division_lines);

    // Draw axes
    _draw_axis("x", axis_lines);
    _draw_axis("y", axis_lines);

    // Attach line nodes to grid
    _attach_lines(axis_lines);
    _attach_lines(grid_lines);
    _attach_lines(sub_division_lines);
}

void AxisGrid::_draw_grid_lines(float step, LineSegs& line_seg) {
    for (float pos : _frange(0, grid_size, step)) {
        // X-direction lines
        line_seg.move_to(pos, -grid_size, 0);
        line_seg.draw_to(pos, grid_size, 0);
        line_seg.move_to(-pos, -grid_size, 0);
        line_seg.draw_to(-pos, grid_size, 0);

        // Y-direction lines
        line_seg.move_to(-grid_size, pos, 0);
        line_seg.draw_to(grid_size, pos, 0);
        line_seg.move_to(-grid_size, -pos, 0);
        line_seg.draw_to(grid_size, -pos, 0);
    }
}

void AxisGrid::_draw_axis(const std::string& axis, LineSegs& line_seg) {
    if (axis == "x") {
        line_seg.set_color(x_axis_color);
        line_seg.move_to(-grid_size, 0, 0);
        line_seg.draw_to(grid_size, 0, 0);
    } else if (axis == "y") {
        line_seg.set_color(y_axis_color);
        line_seg.move_to(0, -grid_size, 0);
        line_seg.draw_to(0, grid_size, 0);
    }
}

void AxisGrid::_attach_lines(LineSegs& line_seg) {
    NodePath node_path(line_seg.create());
    node_path.reparent_to(*this);
}

std::vector<float> AxisGrid::_frange(float start, float stop, float step) {
    std::vector<float> result;
    for (float cur = start; cur < stop; cur += step) {
        result.push_back(cur);
    }
    return result;
}
