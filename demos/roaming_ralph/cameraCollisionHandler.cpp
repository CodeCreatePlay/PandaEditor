#include <collisionRay.h>
#include <collisionNode.h>
#include <collisionHandlerQueue.h>
#include <collisionTraverser.h>
#include <collideMask.h>


class CameraCollisionHandler {
public:
    CameraCollisionHandler(NodePath& target_np, CollisionTraverser& trav) 
		:	target_np(target_np),
		    c_trav(trav) 
	{}
	
	void init()
	{
		setup_collisions();
	}
	
    void update()
	{
        handle_collisions();
    }

private:
    NodePath& target_np;
    CollisionTraverser& c_trav;
	
    // Collision components
    PT(CollisionRay)  collision_ray;
    PT(CollisionNode) collision_ray_node;	
	NodePath          collision_np;
	
	PT(CollisionHandlerQueue) collision_handler_queue;


    void setup_collisions() 
	{
        // Create a downward collision ray to detect the terrain beneath target_np
        collision_ray = new CollisionRay();
        collision_ray->set_origin(0, 0, 9);
        collision_ray->set_direction(0, 0, -1);

        // Create a collision node to hold the ray
        collision_ray_node = new CollisionNode("RalphRay");
        collision_ray_node->add_solid(collision_ray);
        collision_ray_node->set_from_collide_mask(CollideMask::bit(0));
        collision_ray_node->set_into_collide_mask(CollideMask::all_off());

        // Attach the collision node to Ralph
        collision_np = target_np.attach_new_node(collision_ray_node);

        // Create the collision handler
        collision_handler_queue = new CollisionHandlerQueue();

        // Register the collider with the traverser
        c_trav.add_collider(collision_np, collision_handler_queue);
    }

    void handle_collisions()
	{
        // Store Ralph's current position in case we need to revert it
        LPoint3 start_pos = target_np.get_pos();

        // Collect collision entries
        int numEntries = collision_handler_queue->get_num_entries();
        std::vector<CollisionEntry*> entries;

        for (int i = 0; i < numEntries; ++i)
		{
            entries.push_back(collision_handler_queue->get_entry(i));
        }

        // Sort entries based on the Z-coordinate (lowest first)
		std::sort
		(
			entries.begin(),
			entries.end(), 
			[this](CollisionEntry* a, CollisionEntry* b) // Capture 'this' for target_np
			{
				return a->get_surface_point(target_np).get_z() < b->get_surface_point(target_np).get_z();
			}
		);

        // If there are valid collision entries, adjust Ralph's Z position
        if (!entries.empty() && entries[0]->get_into_node()->get_name() == "Collision")
		{
            LPoint3 surfacePoint = entries[0]->get_surface_point(NodePath());
            target_np.set_z(surfacePoint.get_z());
        }
		else 
		{
            // If no valid collision, restore Ralph's previous position
            target_np.set_pos(start_pos);
        }
    }
};
