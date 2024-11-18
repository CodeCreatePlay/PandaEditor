#include "include/mouse.h"


Mouse::Mouse() : x(0), y(0), dx(0), dy(0) {}

Mouse::Mouse(GraphicsWindow* win, MouseWatcher* mwn)
    : win(win), mwn(mwn), x(0), y(0), dx(0), dy(0) {
        
    mData = win->get_pointer(0);
}

void Mouse::update() {
    if (!mwn->has_mouse()) {
        return;
    }

    // Get pointer from screen, calculate delta
    mData = win->get_pointer(0);

    dx = x - mData.get_x();
    dy = y - mData.get_y();

    x = mData.get_x();
    y = mData.get_y();

    // Uncomment for debug output
    // std::cout << "dx: " << dx << " dy: " << dy << " x: " << x << " y: " << y << std::endl;
}

const float Mouse::get_dx() const { return dx; }
const float Mouse::get_dy() const { return dy; }
const float Mouse::get_x()  const { return x; }
const float Mouse::get_y()  const { return y; }