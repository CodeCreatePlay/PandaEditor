#ifndef DEMON_H
#define DEMON_H

#include "engine.h"
#include "project.h"

// #include "..//imgui/p3D_Imgui.hpp"
#include "..//utils/include/pathUtils.h"


class Demon {
public:
    Demon();   // Constructor
    ~Demon();  // Destructor
    void update();

    Engine engine;
    GraphicsWindow *win;
    Project project;
    // Panda3DImGui p3d_imgui;

private:
    void init();
};

#endif
