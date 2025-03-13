#ifndef RUNTIME_SCRIPT_H
#define RUNTIME_SCRIPT_H

#include <clockObject.h>

#include "demon.hpp"
#include "mouse.hpp"
#include "game.hpp"
#include "taskUtils.hpp"
#include "mathUtils.hpp"

class RuntimeScript {
public:
    RuntimeScript() :
        demon(Demon::get_instance()),
        mouse(demon.engine.mouse),
        resource_manager(demon.engine.resource_manager),
        game(demon.game) {
        
        // ----------------------------------------------------------- //
        // Get derived class name
        task_name = typeid(*this).name();
        size_t pos = task_name.find("class ");
        if (pos != std::string::npos) {
            task_name = task_name.substr(pos + 6);
        }
        task_name += "Update";

        // Add the task to task manager
        add_task([this](AsyncTask* task) {

			if (game.mouse_watcher->has_mouse() || 
				(demon.engine.mouse_watcher->has_mouse() &&
				demon.engine.mouse.is_mouse_centered()))
			{
				dt = ClockObject::get_global_clock()->get_dt();
				this->on_update(task);
			}
            return AsyncTask::DS_cont;
        }, task_name);
        
        // ----------------------------------------------------------- //
        // Hook event listener
        demon.engine.accept([this](const std::string& event_name) { this->on_event(event_name); });
    }

    virtual ~RuntimeScript() {
        remove_task(task_name);  // Cleanup task to prevent memory leaks
    }
	
	void start() {
        demon.start();
    }
	
	const std::unordered_map<std::string, bool>& get_buttons_map() {
		return input_map;
	}
    
protected:
    Demon&           demon;
    Mouse&           mouse;
    ResourceManager& resource_manager;
    Game&            game;
	
	float dt;
	std::unordered_map<std::string, bool> input_map;

    template <typename Callable>
    void accept(const std::string& event_name, Callable callable) {
        demon.engine.accept(event_name, callable);
    }
	
	void register_button_map(std::unordered_map<std::string, std::pair<std::string, bool>>& map) {
		input_map.clear();
		buttons_map_ = map;
		
		// fill key map
		for (auto& it : buttons_map_) {
			if (input_map.find(it.second.first) == input_map.end()) {
				input_map[it.second.first] = false;
			}
		}
	}
	
    virtual void on_update(const PT(AsyncTask)&) {}
	
    virtual void on_event(const std::string& event_name) {
		// update input_map
		auto& it = buttons_map_.find(event_name);
		if (it != buttons_map_.end()) {
			input_map.at(it->second.first) = it->second.second;
		}		
	}
	
	float get_dt() {
		return ClockObject::get_global_clock()->get_dt();
	}

private:
    std::string task_name;
    std::unordered_map<std::string, std::pair<std::string, bool>> buttons_map_;
};

#endif // RUNTIME_SCRIPT_H