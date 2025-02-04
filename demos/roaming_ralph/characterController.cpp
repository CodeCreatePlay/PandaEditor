#include <unordered_map>

#include <clockObject.h>
#include <nodePath.h>
#include <animControlCollection.h>

#include "demon.h"
#include "animUtils.h"


class CharacterController {
public:
    CharacterController(NodePath& character)
	  : character(character)
	{
		global_clock = ClockObject::get_global_clock();
	}
    
	void load_anims(std::string assets_path)
	{
		Demon& demon = Demon::get_instance();
		
		// load animation
		NodePath walk_anim = demon.engine.resourceManager.load_model(assets_path + "/ralph-run.egg.pz");
		
		// Bind the animation to the character
		AnimUtils::bind_anims(character, walk_anim, animator);
	}
	
    void update(const std::unordered_map<std::string, bool>& key_map)
	{
		float dt = global_clock->get_dt();

		if (key_map.at("left"))
			character.set_h(character.get_h() + 300 * dt);
		
		if (key_map.at("right"))
			character.set_h(character.get_h() - 300 * dt);
		
		if (key_map.at("forward"))
			character.set_y(character, -25 * dt);

		if (key_map.at("forward") || key_map.at("left") || key_map.at("right"))
		{
			if (!is_moving)
			{
				animator.get_anim(0)->loop(true);
				is_moving = true;
			}
		}
		else
		{
			if (is_moving)
			{
				animator.stop_all();
				animator.pose("walk", 5);
				is_moving = false;
			}
		}
	}

private:
    NodePath& character;
    AnimControlCollection animator;
	
    bool is_moving;
	
	PT(ClockObject) global_clock;
};
