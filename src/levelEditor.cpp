#include "engine.h"
#include "game.h"
#include "levelEditor.hpp"


LevelEditor::LevelEditor(Engine* engine, Game* game) : 
	engine(engine),
	game(game),
	mouse_picker("SelectionMousePicker"),
	marquee("Marquee") {}

void LevelEditor::init() {
	
	mouse_picker.init(engine);
	marquee.init(engine, game->render);
	
	engine->add_event_hook(1,
		[this](const Event* evt, const std::vector<void*>& params) { this->on_event(evt, params); }
	);
}

std::vector<NodePath> LevelEditor::get_selected_nps() { return selected_nps; }

void LevelEditor::on_event(const Event* evt, const std::vector<void*>&) {
	
	if(evt->get_name() == "mouse1") {
		mouse_picker.update();
		marquee.on_start();
	}
	else if(evt->get_name() == "mouse1-up") {
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
}
