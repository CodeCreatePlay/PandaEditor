#ifndef RUNTIME_SCRIPT_H
#define RUNTIME_SCRIPT_H

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
		// Create update task
		update_task = (make_task([this](AsyncTask *task) -> AsyncTask::DoneStatus {
			
			if (game.mouse_watcher->has_mouse() || 
				(demon.engine.mouse_watcher->has_mouse() &&
				demon.engine.mouse.is_mouse_centered()))
			{
				dt = ClockObject::get_global_clock()->get_dt();
				this->on_update(task);
			}
			
			return AsyncTask::DS_cont;
		}, "RuntimeScriptUpdate"));

        // ----------------------------------------------------------- //
        // Hook event listener
        demon.engine.accept([this](const std::string& event_name) { this->on_event(event_name); });
        demon.engine.accept("game_mode_enabled",  [this]() { this->start_update_task(); });
        demon.engine.accept("game_mode_disabled", [this]() { this->stop_update_task(); });
    }

    virtual ~RuntimeScript() {
        remove_task(task_name);
    }
	
	void start() {
        demon.start();
    }
	
	int get_sort() { return -1; }
	
	int get_priority() { return -1; }
	
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
			input_map[it->second.first] = it->second.second;
		}		
	}
	
	float get_dt() {
		return ClockObject::get_global_clock()->get_dt();
	}

private:
    std::string task_name;
	
	PT(AsyncTask) update_task;
    std::unordered_map<std::string, std::pair<std::string, bool>> buttons_map_;
	
	void start_update_task() {
        // Get derived class name
        task_name = typeid(*this).name();
        size_t pos = task_name.find("class ");
        if (pos != std::string::npos) {
            task_name = task_name.substr(pos + 6);
        }
        task_name += "Task";

		update_task->set_name(task_name);
		if (!has_task(task_name))
			AsyncTaskManager::get_global_ptr()->add(update_task);
	}
	
	void stop_update_task() {
		remove_task(task_name); // defined in taskUtils.hpp
	}
};

#endif // RUNTIME_SCRIPT_H
