#ifndef THREE_AXIS_GRID_H
#define THREE_AXIS_GRID_H

#include <lvector4.h>
#include <geomNode.h>
#include <geom.h>
#include <geomVertexFormat.h>
#include <geomVertexData.h>
#include <geomVertexWriter.h>
#include <geomLinestrips.h>
#include <nodePath.h>
#include <lineSegs.h>


class AxisGrid : public NodePath {
public:
    AxisGrid();
    void create(float size, float grid_step, int sub_divisions);

private:
    float grid_size;
    float grid_step;
    int sub_divisions;

    bool show_end_cape_lines;

    LVecBase4 x_axis_color;
    LVecBase4 y_axis_color;
    LVecBase4 grid_color;
    LVecBase4 sub_div_color;

    float axis_thickness;
    float grid_thickness;
    float sub_div_thickness;

    PT(GeomNode) axisLinesNode;
    PT(GeomNode) gridLinesNode;
    PT(GeomNode) subdivLinesNode;

    LineSegs axisLines;
    LineSegs gridLines;
    LineSegs subdivision_Lines;

    std::vector<float> myfrange(float start, float stop, float step);
};

#endif // THREE_AXIS_GRID_H