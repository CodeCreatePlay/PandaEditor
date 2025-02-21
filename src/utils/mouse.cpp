#include <algorithm>

#include <mouseButton.h>
#include <keyboardButton.h>
#include "engine.hpp"
#include "mouse.hpp"


Mouse::Mouse(Engine& engine) : engine(engine), mData(nullptr) {}

void Mouse::initialize()
{
	engine.accept("alt",        [this]() { this->set_modifier(MOUSE_ALT);    });
	engine.accept("alt-up",     [this]() { this->clear_modifier(MOUSE_ALT);  });
	engine.accept("control",    [this]() { this->set_modifier(MOUSE_CTRL);   });
	engine.accept("control-up", [this]() { this->clear_modifier(MOUSE_CTRL); });
	
	// Initialize mouse button states
    mouse_buttons[MouseButton::one().get_name()]   = false;
    mouse_buttons[MouseButton::two().get_name()]   = false;
    mouse_buttons[MouseButton::three().get_name()] = false;
    mouse_buttons[MouseButton::four().get_name()]  = false;
    mouse_buttons[MouseButton::five().get_name()]  = false;
}

void Mouse::update()
{
    if (!engine.mouse_watcher->has_mouse())
        return;

    for (auto& btn : mouse_buttons)
        mouse_buttons[btn.first] = engine.mouse_watcher->is_button_down(btn.first);

    // Get pointer from screen, calculate delta
    mData = &engine.win->get_pointer(0);
    
    dx = x - mData->get_x();
    dy = y - mData->get_y();

	if (dx < 0) horizontal_axis = -1;
	else if (dx > 0) horizontal_axis = 1;
	else horizontal_axis = 0;
	
	if (dy < 0) vertical_axis = -1;
	else if (dy > 0) vertical_axis = 1;
	else vertical_axis = 0;

    x = mData->get_x();
    y = mData->get_y();
}

void Mouse::set_modifier(int index)
{
    if (std::find(modifiers.begin(), modifiers.end(), index) == modifiers.end())
	{
        modifiers.push_back(index);
    }
}

void Mouse::clear_modifier(int index)
{
    modifiers.erase(std::remove(modifiers.begin(), modifiers.end(), index), modifiers.end());
}

bool Mouse::has_modifier(int index) const
{
    return std::find(modifiers.begin(), modifiers.end(), index) != modifiers.end();
}

bool Mouse::is_button_down(const std::string& buttonName) const
{
    auto it = mouse_buttons.find(buttonName);
    return (it != mouse_buttons.end()) ? it->second : false;
}

bool Mouse::has_mouse() const
{
    return engine.mouse_watcher->has_mouse();
}

float Mouse::get_dx() const { return dx; }
float Mouse::get_dy() const { return dy; }

float Mouse::get_x() const { return x; }
float Mouse::get_y() const { return y; }

float Mouse::get_vertical() const {
	return vertical_axis;
}

float Mouse::get_horizontal() const { 
	return horizontal_axis;
}

const std::unordered_map<std::string, bool>& Mouse::get_mouse_buttons() const { return mouse_buttons; }
