#ifndef MOUSE_H
#define MOUSE_H

#include <unordered_map>
#include <vector>
#include <string>

class Engine;

class Mouse {
public:
    Mouse(Engine& _engine);
	void initialize();
    void update();
	void force_relative_mode();
    void clear_modifier(int index);
	void toggle_force_relative_mode();
    bool has_modifier(int modifier) const;
    bool has_mouse() const;
	bool is_button_down(const std::string& buttonName) const;

    // Getters
	float get_x()  const;
    float get_y()  const;
    float get_dx() const;
    float get_dy() const;
	int get_zoom() const;
	
	float get_vertical()   const;
	float get_horizontal() const;
	
	bool is_mouse_centered() const;
	
	const std::unordered_map<std::string, bool>& get_mouse_buttons() const;
	
	// Setters
	void set_modifier(int index);
	void set_mouse_mode(int mouse_mode_idx);
	
private:
    float _x;
    float _y;
	
    float _dx;
    float _dy;
	
	int _zoom;
	
	float _vertical_axis;
	float _horizontal_axis;
		
	int _current_mouse_mode;
	bool _force_relative_mode;
	
	Engine& _engine;
    std::unordered_map<std::string, bool> _mouse_buttons;
    std::vector<int> _modifiers;
};

#endif // MOUSE_H
