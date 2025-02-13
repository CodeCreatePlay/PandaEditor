#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "mousePicker.hpp"
#include "marquee.hpp"


class Demon;

class LevelEditor {
public:
    LevelEditor(Demon& demon);
	void init();
	std::vector<NodePath> get_selected_nps();

private:
	void on_mouse();
	void on_mouse_up();

	Demon&      demon;
	MousePicker mouse_picker;
	Marquee     marquee;
	
	std::vector<NodePath> selected_nps;
};

#endif
