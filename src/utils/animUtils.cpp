#include <nodePath.h>

#include <character.h>
#include <animBundleNode.h>
#include <partBundle.h>
#include <animControlCollection.h>

#include "animUtils.hpp"


// Bind animations that are children of the character NodePath
void AnimUtils::bind_anims(NodePath character_np, AnimControlCollection& acc) {
	// Get the PartBundle from the character NodePath
	PT(PartBundle) part_bundle = get_part_bundle(character_np);

	// Check if PartBundle was found
	if (part_bundle == nullptr) {
		std::cerr << "Error: No PartBundle found in the character!" << std::endl;
		return;
	}

	// Now bind all AnimBundles found in the character node
	_bind(part_bundle, character_np, acc);
}

// Bind multiple animations from a separate animation NodePath
void AnimUtils::bind_anims(NodePath character_np, NodePath anim_np, AnimControlCollection& acc) {
	// Get the PartBundle from the character NodePath
	PT(PartBundle) part_bundle = get_part_bundle(character_np);

	// Check if PartBundle was found
	if (part_bundle == nullptr) {
		std::cerr << "Error: No PartBundle found in character!" << std::endl;
		return;
	}

	// Bind all AnimBundles found in the animation NodePath
	_bind(part_bundle, anim_np, acc);
}

// Helper method to get the PartBundle from the character NodePath
PT(PartBundle) AnimUtils::get_part_bundle(NodePath character_np) {
	// Traverse the character node hierarchy to find the Character and its PartBundle
	for (int i = 0; i < character_np.get_num_children(); ++i) {
		NodePath child = character_np.get_child(i);
		
		if (child.node()->is_of_type(Character::get_class_type())) {
			// Found the Character node, get the PartBundle
			PT(Character) character = DCAST(Character, child.node());
			return character->get_bundle(0);  // Get the first PartBundle
		}
	}
	
	return nullptr;
}

// Private method to bind AnimBundles to a PartBundle
void AnimUtils::_bind(PT(PartBundle) part_bundle, NodePath anim_np, AnimControlCollection& acc) {
	for (int i = 0; i < anim_np.get_num_children(); ++i) {
		NodePath child = anim_np.get_child(i);
		
		if (child.node()->is_of_type(AnimBundleNode::get_class_type())) {
			// Found an AnimBundleNode, get the AnimBundle
			PT(AnimBundleNode) anim_node = DCAST(AnimBundleNode, child.node());
			PT(AnimBundle) anim_bundle = anim_node->get_bundle();
			
			// Bind the animation to the PartBundle
			PT(AnimControl) anim_control = part_bundle->bind_anim(anim_bundle);

			// Store the AnimControl in the AnimControlCollection
			acc.store_anim(anim_control, anim_bundle->get_name());
		}
	}
}
