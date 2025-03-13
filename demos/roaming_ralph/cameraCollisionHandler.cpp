#include <collisionRay.h>
#include <collisionNode.h>
#include <collisionHandlerQueue.h>
#include <collisionTraverser.h>
#include <collideMask.h>

class CameraCollisionHandler {
public:
    CameraCollisionHandler(NodePath& target_np, CollisionTraverser& trav) 
        : target_np(target_np),
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
    PT(CollisionRay)          collision_ray;
    PT(CollisionNode)         collision_ray_node;    
    NodePath                  collision_np;
    PT(CollisionHandlerQueue) collision_handler_queue;

    void setup_collisions() 
    {
        // Create a downward collision ray
        collision_ray = new CollisionRay();
        collision_ray->set_origin(0, 0, 9);
        collision_ray->set_direction(0, 0, -1);

        // Create a collision node to hold the ray
        collision_ray_node = new CollisionNode("CameraCollisionRay");
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
        int numEntries = collision_handler_queue->get_num_entries();
        if (numEntries == 0) return;

        // Panda3D's built-in sorting
        collision_handler_queue->sort_entries();

        // Get the lowest collision entry directly
        CollisionEntry* entry = collision_handler_queue->get_entry(0);
        if (entry->get_into_node()->get_name() == "Collider")
        {
            target_np.set_z(entry->get_surface_point(NodePath()).get_z());
        }
    }
};
