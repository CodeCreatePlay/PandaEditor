#ifndef MOUSE_H
#define MOUSE_H

#include <unordered_map>
#include <vector>
#include <string>


class Engine;

class Mouse {
public:
    Mouse(Engine& engine);
	void initialize();
    void update();
    void set_modifier(int index);
    void clear_modifier(int index);
    bool has_modifier(int modifier) const;
    bool is_button_down(const std::string& buttonName) const;
    bool has_mouse() const;

    // Getters
	float get_x()    const;
    float get_y()    const;
    float get_dx()   const;
    float get_dy()   const;
	int get_zoom()   const;
	
	float get_vertical()   const;
	float get_horizontal() const;
	
    const std::unordered_map<std::string, bool>& get_mouse_buttons() const;

private:
    float x;
    float y;
	
    float dx;
    float dy;
	
	int zoom;
	
	float vertical_axis;
	float horizontal_axis;
	
	Engine& engine;
    MouseData* mData;
    std::unordered_map<std::string, bool> mouse_buttons;
    std::vector<int> modifiers;
};

#endif // MOUSE_H
