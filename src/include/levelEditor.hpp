#ifndef LEVEL_EDITOR_H
#define LEVEL_EDITOR_H

#include "mousePicker.hpp"
#include "marquee.hpp"


class Engine;
class Game;

class LevelEditor {
public:
    LevelEditor(Engine* engine, Game* game);
	void init();
	std::vector<NodePath> get_selected_nps();

private:
	void on_event(const Event* evt, const std::vector<void*>&);

    Engine*     engine;
	Game*       game;
	MousePicker mouse_picker;
	Marquee     marquee;
	
	std::vector<NodePath> selected_nps;
};

#endif
