// cameraSystem.cpp
#include "core/cameraSystem.h"

namespace core::systems {
    void CameraSystem::UpdateCamera(components::GameCamera& camera, 
                                  game::game_objects::GameObject& target, 
                                  float deltaTime) {
        // Get target position with offset
        Vector2 targetPosition = target.getTransform().center;
        targetPosition.x += camera.targetOffset.x;
        targetPosition.y += camera.targetOffset.y;
        
        // Smooth camera movement
        Vector2 currentPosition = {camera.camera.target.x, camera.camera.target.y};
        Vector2 newPosition = Vector2Lerp(
            currentPosition,
            targetPosition,
            1.0f - std::exp(-camera.smoothSpeed * deltaTime)
        );
        
        camera.camera.target = newPosition;
        camera.camera.zoom = camera.zoom;
    }

    void CameraSystem::BeginCameraDraw(const components::GameCamera& camera) {
        BeginMode2D(camera.camera);
    }

    void CameraSystem::EndCameraDraw() {
        EndMode2D();
    }
}