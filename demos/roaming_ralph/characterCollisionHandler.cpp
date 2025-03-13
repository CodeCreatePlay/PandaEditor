#include <lpoint3.h>
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
        
    void init(const LPoint3& start_pos)
    {
        this->start_pos = start_pos;
        
        collision_ray = new CollisionRay();
        collision_ray->set_origin(0, 0, 10);
        collision_ray->set_direction(0, 0, -1);

        collision_ray_node = new CollisionNode("CharacterCollisionRay");
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
        
        if (numEntries == 0) {
            character.set_pos(start_pos);
            return;
        }

        // Sort the collision entries directly using Panda3D's built-in function
        collision_handler_queue->sort_entries();

        // Use the first entry
        CollisionEntry* entry = collision_handler_queue->get_entry(0);
        if (entry->get_into_node()->get_name() == "Collider") {
            character.set_z(entry->get_surface_point(NodePath()).get_z());
        }
    }

private:
    NodePath& character;
    LPoint3   start_pos;
    
    PT(CollisionRay)          collision_ray;
    PT(CollisionNode)         collision_ray_node;
    NodePath                  collision_np;
    PT(CollisionHandlerQueue) collision_handler_queue;
    CollisionTraverser&       c_trav;
};
