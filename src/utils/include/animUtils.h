#include <nodePath.h>

#include <animControlCollection.h>
#include <character.h>
#include <animBundleNode.h>
#include <partBundle.h>


class AnimUtils {
public:
	// Bind animations that are children of the character NodePath
	static void bind_anims(NodePath character_np, AnimControlCollection& acc);
	
	// Bind multiple animations from a separate animation NodePath
	static void bind_anims(NodePath character_np, NodePath anim_np, AnimControlCollection& acc);

private:
	// Helper method to get the PartBundle from the character NodePath
	static PT(PartBundle) get_part_bundle(NodePath character_np);

	// Private method to bind AnimBundles to a PartBundle
	static void _bind(PT(PartBundle) part_bundle, NodePath anim_np, AnimControlCollection& acc);
};
