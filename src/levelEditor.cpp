#include "demon.h"
#include "levelEditor.hpp"


LevelEditor::LevelEditor(Demon *demon) : 
	demon(demon),
	mouse_picker("SelectionMousePicker"),
	marquee("Marquee") {}

void LevelEditor::init() {
	
	mouse_picker.init(&demon->engine);
	marquee.init(&demon->engine, demon->game.render);
	
	demon->engine.define_event( "mouse1",    [this](std::vector<void*>& params) { return this->on_mouse(params);    }, {} );
	demon->engine.define_event( "mouse1-up", [this](std::vector<void*>& params) { return this->on_mouse_up(params); }, {} );
}

std::vector<NodePath> LevelEditor::get_selected_nps() { return selected_nps; }

void LevelEditor::on_mouse(std::vector<void*>& params) {
	
	this->mouse_picker.update();
	this->marquee.on_start();
}

void LevelEditor::on_mouse_up(std::vector<void*>& params) {
	
	marquee.on_stop();
		
	// get selected nodes
	selected_nps = marquee.get_found_nps();
	selected_nps.push_back(mouse_picker.get_first_np());

	for (const auto& np : selected_nps) {
		if(np.is_empty())
			continue;
		std::cout << np.get_name() << std::endl;
	}
}
