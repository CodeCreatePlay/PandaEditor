#include <unordered_map>
#include <nodePath.h>

class CameraController {
public:
    CameraController(NodePath& target_np, NodePath& cam) 
        : target_np(target_np),
          camera(cam)
    {}

    void init()
    {
        // Create a floater object, which floats 2 units above Ralph
        floater = NodePath("CamLookAtTarget");
        floater.reparent_to(target_np);
        floater.set_z(2.0f);

        // Set an initial position for the camera
        camera.set_pos(target_np.get_x(), target_np.get_y() - 20, 12.5);

        // Make the camera look at the floater above Ralph's head
        camera.look_at(floater);
    }

    void update(float dt, std::unordered_map<std::string, bool>& input_map) 
    {
        // Handle camera movement left and right
        if (input_map["cam-left"])
            camera.set_x(camera, -20 * dt);
        else if (input_map["cam-right"])
            camera.set_x(camera, 20 * dt);
		
        // Compute camera movement constraints
        LVector3 cam_vec = target_np.get_pos() - camera.get_pos();
        cam_vec.set_z(0);  // Keep camera movement in the X-Y plane
		
        float cam_dist = cam_vec.length();
        cam_vec.normalize();

        // Apply min and max distance constraints
        if (cam_dist > 20.0f)
		{
            camera.set_pos(camera.get_pos() + cam_vec * (cam_dist - 20.0f));
            cam_dist = 20.0f;
        }
        else if (cam_dist < 5.0f)
		{
            camera.set_pos(camera.get_pos() - cam_vec * (5.0f - cam_dist));
        }

        // Make the camera look at the floater above Ralph's head
        camera.look_at(floater);
    }

private:
    NodePath& target_np;
    NodePath& camera;
    NodePath  floater;
};
