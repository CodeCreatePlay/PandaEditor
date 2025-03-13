#include <algorithm>

#include <mouseButton.h>
#include <keyboardButton.h>
#include "taskUtils.hpp"
#include "constants.hpp"
#include "engine.hpp"
#include "mouse.hpp"

Mouse::Mouse(Engine& _engine) : _engine(_engine) {}

void Mouse::initialize() {
	_engine.accept("alt",        [this]() { this->set_modifier(ALT_KEY_IDX);    });
	_engine.accept("alt-up",     [this]() { this->clear_modifier(ALT_KEY_IDX);  });
	_engine.accept("control",    [this]() { this->set_modifier(CTRL_KEY_IDX);   });
	_engine.accept("control-up", [this]() { this->clear_modifier(CTRL_KEY_IDX); });
	
	// Initialize mouse button states
    _mouse_buttons[MouseButton::one().get_name()]   = false;
    _mouse_buttons[MouseButton::two().get_name()]   = false;
    _mouse_buttons[MouseButton::three().get_name()] = false;
    _mouse_buttons[MouseButton::four().get_name()]  = false;
    _mouse_buttons[MouseButton::five().get_name()]  = false;
}

void Mouse::update() {
    if (!_engine.mouse_watcher->has_mouse())
        return;

    for (auto& btn : _mouse_buttons)
        _mouse_buttons[btn.first] = _engine.mouse_watcher->is_button_down(btn.first);

    // Get pointer from screen, calculate delta
    const MouseData _m_data = _engine.win->get_pointer(0);
    
	if (_force_relative_mode) {
		_dx = _engine.mouse_watcher->get_mouse_x();
		_dy = _engine.mouse_watcher->get_mouse_y();
		
		_horizontal_axis = (_dx > 0) ? 1 : (_dx < 0) ? -1 : 0;
		_vertical_axis   = (_dy > 0) ? 1 : (_dy < 0) ? -1 : 0;
		
	} else {
		_dx = _x - _m_data.get_x();
		_dy = _y - _m_data.get_y();
		
		_horizontal_axis = (_dx > 0) ? 1 : (_dx < 0) ? -1 : 0;
		_vertical_axis   = (_dy > 0) ? 1 : (_dy < 0) ? -1 : 0;

		_x = _m_data.get_x();
		_y = _m_data.get_y();
	}
	
	if (_force_relative_mode) {
		force_relative_mode();
	}
}

void Mouse::force_relative_mode() {
	_engine.win->move_pointer(
		0,
		static_cast<int>(_engine.win->get_properties().get_x_size() / 2),
		static_cast<int>(_engine.win->get_properties().get_y_size() / 2));
}

void Mouse::clear_modifier(int index) {
    _modifiers.erase(std::remove(_modifiers.begin(), _modifiers.end(), index), _modifiers.end());
}

void Mouse::toggle_force_relative_mode() {
	_force_relative_mode = !_force_relative_mode;
	std::cout << "Toggle force mouse relative mode: " << _force_relative_mode << std::endl;
}

bool Mouse::has_modifier(int index) const {
    return std::find(_modifiers.begin(), _modifiers.end(), index) != _modifiers.end();
}

bool Mouse::has_mouse() const {
    return _engine.mouse_watcher->has_mouse();
}

bool Mouse::is_button_down(const std::string& buttonName) const {
    auto it = _mouse_buttons.find(buttonName);
    return (it != _mouse_buttons.end()) ? it->second : false;
}

float Mouse::get_x() const { return _x; }

float Mouse::get_y() const { return _y; }

float Mouse::get_dx() const { return _dx; }

float Mouse::get_dy() const { return _dy; }

float Mouse::get_vertical() const {
	return _vertical_axis;
}

float Mouse::get_horizontal() const { 
	return _horizontal_axis;
}

bool Mouse::is_mouse_centered() const {
	return _force_relative_mode || (_current_mouse_mode == WindowProperties::M_relative);
}

const std::unordered_map<std::string, bool>& Mouse::get_mouse_buttons() const { 
	return _mouse_buttons; 
}

void Mouse::set_modifier(int index) {
    if (std::find(_modifiers.begin(), _modifiers.end(), index) == _modifiers.end()) {
        _modifiers.push_back(index);
    }
}

void Mouse::set_mouse_mode(int requested_mouse_mode) {
	WindowProperties wp = _engine.win->get_properties();

	if (requested_mouse_mode == WindowProperties::M_absolute) {
		wp.set_mouse_mode(WindowProperties::M_absolute);
		std::cout << "Mouse mode set to: " << WindowProperties::M_absolute << std::endl;
	} else if (requested_mouse_mode == WindowProperties::M_relative) {
		wp.set_mouse_mode(WindowProperties::M_relative);
		std::cout << "Mouse mode set to: " << WindowProperties::M_relative << std::endl;
	} else if (requested_mouse_mode == WindowProperties::M_confined) {
		wp.set_mouse_mode(WindowProperties::M_confined);
		std::cout << "Mouse mode set to: " << WindowProperties::M_confined << std::endl;
	}
	else {
		std::cout << "Unable to set_mouse_mode: " << 
			requested_mouse_mode << " not found." << std::endl;
		wp.set_mouse_mode(WindowProperties::M_absolute);
		_current_mouse_mode = WindowProperties::M_absolute;
		return;
	}
	
	_engine.win->request_properties(wp);
	
	// Resolve mouse mode
	PT(AsyncTask) current_mouse_mode_resolve_update = 
		(make_task([this, requested_mouse_mode](AsyncTask *task) -> AsyncTask::DoneStatus {

		WindowProperties wp = _engine.win->get_properties();
        _current_mouse_mode = wp.get_mouse_mode();
		
        if (requested_mouse_mode != _current_mouse_mode)
            std::cout << "ACTUAL MOUSE MODE: " << _current_mouse_mode << std::endl;

		return AsyncTask::DS_done;
	}, "MouseModeResolveUpdate"));
	
	current_mouse_mode_resolve_update->set_delay(0);
	AsyncTaskManager::get_global_ptr()->add(current_mouse_mode_resolve_update);
}
