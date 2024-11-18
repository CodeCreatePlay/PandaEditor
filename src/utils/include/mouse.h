#ifndef MOUSE_H
#define MOUSE_H

#include <unordered_map>
#include <string>

#include <graphicsWindow.h>
#include <mouseWatcher.h>


class Mouse {
public:
    Mouse();
    Mouse(GraphicsWindow* win, MouseWatcher* mwn);

    void update();
	
    const float get_dx() const;
    const float get_dy() const;
    const float get_x()  const;
    const float get_y()  const;

private:
    GraphicsWindow* win;
    MouseWatcher* mwn;

    float x;  // mouse pos x
    float y;  // mouse pos y
    float dx; // mouse displacement x since last frame
    float dy; // mouse displacement y since last frame
	
    MouseData mData; // mouse position from GraphicsWindow
};

#endif // MOUSE_H
