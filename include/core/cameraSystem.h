// cameraSystem.h
#pragma once
#include "components.h"
#include "game/gameObjects.h"

namespace core::systems {
    class CameraSystem {
    public:
        static void UpdateCamera(components::GameCamera& camera,
            game::game_objects::GameObject& target,
            float deltaTime);

        static void BeginCameraDraw(const components::GameCamera& camera);
        static void EndCameraDraw();
    };
}