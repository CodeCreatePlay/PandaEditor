#include <unordered_map>

#include <nodePath.h>
#include <animControlCollection.h>

#include "demon.hpp"
#include "animUtils.hpp"

class CharacterController {
public:
    CharacterController(NodePath& character)
	  : character(character), is_moving(false)
	{}
    
	void init(const std::vector<NodePath>& anims)
	{
		// Bind the animation to the character
		for (const NodePath& anim : anims) {
			AnimUtils::bind_anims(character, anim, animator);
		}
	}
	
    void update(float dt, const std::unordered_map<std::string, bool>& key_map)
	{
		if (key_map.find("left") != key_map.end() && key_map.at("left"))
			character.set_h(character.get_h() + 300 * dt);
		
		if (key_map.find("right") != key_map.end() && key_map.at("right"))
			character.set_h(character.get_h() - 300 * dt);
		
		if (key_map.find("forward") != key_map.end() && key_map.at("forward"))
			character.set_y(character, -25 * dt);

		bool moving = key_map.find("forward") != key_map.end() && key_map.at("forward") ||
		              key_map.find("left") != key_map.end() && key_map.at("left") ||
		              key_map.find("right") != key_map.end() && key_map.at("right");

		if (moving)
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
    NodePath&             character;
    AnimControlCollection animator;
	bool                  is_moving;
};
