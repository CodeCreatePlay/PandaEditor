#include <collisionTraverser.h>
#include <collisionHandlerQueue.h>
#include <collisionRay.h>
#include <collisionNode.h>
#include <collideMask.h>
#include <nodePath.h>

class CharacterCollisionHandler {
public:
    CharacterCollisionHandler(NodePath& character, CollisionTraverser& c_trav)
		: character(character),
		  c_trav(c_trav) 
	{}
		
    void init()
	{
		collision_ray = new CollisionRay();
		collision_ray->set_origin(0, 0, 9);
		collision_ray->set_direction(0, 0, -1);

		collision_ray_node = new CollisionNode("RalphRay");
		collision_ray_node->add_solid(collision_ray);
		collision_ray_node->set_from_collide_mask(CollideMask::bit(0));
		collision_ray_node->set_into_collide_mask(CollideMask::all_off());

		collision_np = character.attach_new_node(collision_ray_node);

		collision_handler_queue = new CollisionHandlerQueue();
		c_trav.add_collider(collision_np, collision_handler_queue);
	}
	
    void update()
	{
		int numEntries = collision_handler_queue->get_num_entries();
		std::vector<CollisionEntry*> entries;

		for (int i = 0; i < numEntries; ++i)
			entries.push_back(collision_handler_queue->get_entry(i));

		std::sort
		(
			entries.begin(),
			entries.end(), 
			[](CollisionEntry* a, CollisionEntry* b)
			{ return a->get_surface_point(NodePath()).get_z() < b->get_surface_point(NodePath()).get_z(); }
		);

		if (!entries.empty() && entries[0]->get_into_node()->get_name() == "Collision")
		{
			character.set_z(entries[0]->get_surface_point(NodePath()).get_z());
		}
	}

private:
    NodePath& character;
    CollisionTraverser& c_trav;

    PT(CollisionRay) collision_ray;
    PT(CollisionNode) collision_ray_node;
    NodePath collision_np;
	
    PT(CollisionHandlerQueue) collision_handler_queue;
};
