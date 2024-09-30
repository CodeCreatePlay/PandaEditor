#include "include/mouse.h"


Mouse::Mouse() : x(0), y(0), dx(0), dy(0) {
}

Mouse::Mouse(GraphicsWindow* win, MouseWatcher* mwn)
    : win(win), mwn(mwn), x(0), y(0), dx(0), dy(0) {
    
    mouse_btns[0] = MouseButton::one();
    mouse_btns[1] = MouseButton::two();
    mouse_btns[2] = MouseButton::three();
    mouse_btns[3] = MouseButton::four();
    mouse_btns[4] = MouseButton::five();
    
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

const bool Mouse::is_button_down(int idx) const { return mwn->is_button_down(mouse_btns.at(idx)); }
const float Mouse::get_dx() const { return dx; }
const float Mouse::get_dy() const { return dy; }
const float Mouse::get_x()  const { return x; }
const float Mouse::get_y()  const { return y; }