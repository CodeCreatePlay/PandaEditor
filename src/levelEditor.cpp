#include "demon.h"
#include "levelEditor.hpp"


LevelEditor::LevelEditor(Demon &demon) : 
	demon(demon),
	mouse_picker("SelectionMousePicker", demon.engine),
	marquee("Marquee", demon.engine) {}

void LevelEditor::init() {
	mouse_picker.init();
	marquee.init(demon.game.render);

	demon.engine.accept( "mouse1",    [this]() { this->on_mouse();    });
	demon.engine.accept( "mouse1-up", [this]() { this->on_mouse_up(); });
}

std::vector<NodePath> LevelEditor::get_selected_nps() { return selected_nps; }

void LevelEditor::on_mouse() {
	this->mouse_picker.update();
	this->marquee.on_start();
}

void LevelEditor::on_mouse_up() {
	marquee.on_stop();

	// get selected nodes
	selected_nps = marquee.get_found_nps();
	return;
	selected_nps.push_back(mouse_picker.get_first_np());

	for (const auto& np : selected_nps) {
		if(np.is_empty())
			continue;
		std::cout << np.get_name() << std::endl;
	}
}
