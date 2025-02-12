#ifndef THREE_AXIS_GRID_H
#define THREE_AXIS_GRID_H

#include <lvector4.h>
#include <vector>
#include <nodePath.h>
#include <lineSegs.h>

// Forward declarations
class GeomNode;

class AxisGrid : public NodePath {
public:
    AxisGrid(float grid_size = 100, float grid_step = 10, int sub_divisions = 10);
	
    void create();
    
private:
    void _draw_grid_lines(float step, LineSegs& line_seg);
    void _draw_axis(const std::string& axis, LineSegs& line_seg);
    void _attach_lines(LineSegs& line_seg);
    std::vector<float> _frange(float start, float stop, float step);

    float grid_size;
    float grid_step;
    int sub_divisions;

    bool show_end_cap_lines;

    LVecBase4 x_axis_color;
    LVecBase4 y_axis_color;
    LVecBase4 grid_color;
    LVecBase4 sub_div_color;

    float axis_thickness;
    float grid_thickness;
    float sub_div_thickness;

    LineSegs axis_lines;
    LineSegs grid_lines;
    LineSegs sub_division_lines;
};

#endif // THREE_AXIS_GRID_H
