#include "include/axisGrid.h"


AxisGrid::AxisGrid() :
    NodePath("AxisGrid"),
    grid_size(100),
    grid_step(10),
    sub_divisions(10),
    show_end_cape_lines(true),
    x_axis_color(1, 0, 0, 1),
    y_axis_color(0, 1, 0, 1),
    grid_color(0.4, 0.4, 0.4, 1),
    sub_div_color(0.35, 0.35, 0.35, 1),
    axis_thickness(1),
    grid_thickness(1),
    sub_div_thickness(1) {
}

void AxisGrid::create(float size, float grid_step, int sub_divisions) {
    this->grid_size = size;
    this->grid_step = grid_step;
    this->sub_divisions = sub_divisions;

    axisLines.move_to(0, 0, 0);
    gridLines.move_to(0, 0, 0);
    subdivision_Lines.move_to(0, 0, 0);

    axisLines.set_thickness(axis_thickness);
    gridLines.set_thickness(grid_thickness);
    subdivision_Lines.set_thickness(sub_div_thickness);

    gridLines.set_color(grid_color);
    subdivision_Lines.set_color(sub_div_color);

    for (float x : myfrange(0, grid_size, grid_step)) {
        gridLines.move_to(x, -grid_size, 0);
        gridLines.draw_to(x, grid_size, 0);
        gridLines.move_to(-x, -grid_size, 0);
        gridLines.draw_to(-x, grid_size, 0);
    }

    for (float z : myfrange(0, grid_size, grid_step)) {
        gridLines.move_to(-grid_size, z, 0);
        gridLines.draw_to(grid_size, z, 0);
        gridLines.move_to(-grid_size, -z, 0);
        gridLines.draw_to(grid_size, -z, 0);
    }

    float adjusted_step = grid_step / sub_divisions;
    for (float x : myfrange(0, grid_size, adjusted_step)) {
        subdivision_Lines.move_to(x, -grid_size, 0);
        subdivision_Lines.draw_to(x, grid_size, 0);
        subdivision_Lines.move_to(-x, -grid_size, 0);
        subdivision_Lines.draw_to(-x, grid_size, 0);
    }

    for (float y : myfrange(0, grid_size, adjusted_step)) {
        subdivision_Lines.move_to(-grid_size, y, 0);
        subdivision_Lines.draw_to(grid_size, y, 0);
        subdivision_Lines.move_to(-grid_size, -y, 0);
        subdivision_Lines.draw_to(grid_size, -y, 0);
    }

    axisLines.set_color(x_axis_color);
    axisLines.move_to(0, 0, 0);
    axisLines.move_to(-grid_size, 0, 0);
    axisLines.draw_to(grid_size, 0, 0);

    axisLines.set_color(y_axis_color);
    axisLines.move_to(0, 0, 0);
    axisLines.move_to(0, -grid_size, 0);
    axisLines.draw_to(0, grid_size, 0);

    axisLinesNode = axisLines.create(false);
    NodePath axisLinesNP(axisLinesNode);
    axisLinesNP.reparent_to(*this);

    gridLinesNode = gridLines.create(false);
    NodePath gridLinesNP(gridLinesNode);
    gridLinesNP.reparent_to(*this);

    subdivLinesNode = subdivision_Lines.create(false);
    NodePath subdivLinesNP(subdivLinesNode);
    subdivLinesNP.reparent_to(*this);
}

std::vector<float> AxisGrid::myfrange(float start, float stop, float step) {
    std::vector<float> result;
    for (float cur = start; cur < stop; cur += step) {
        result.push_back(cur);
    }
    return result;
}
