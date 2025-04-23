#include <iostream>

#include "core/objectPool.h"
#include "game/gameObjects.h"
#include "game/gameObjectManager.h"
#include "game/entities/player.h"
#include "game/entities/units.h"
#include "core/cameraSystem.h"
#include "core/animation.h"
#include "game/levelManager.h"

constexpr int screenWidth = 1040;
constexpr int screenHeight = 1040;
constexpr float deltaTimePhys = 0.002f;

using game::game_objects::Asteroid;
using core::object_pool::ObjectPool;
using game::game_objects::GameObject;


auto& objectManager = game::management::GameObjectManager::getInstance();

components::GameCamera gameCamera;

#pragma region SupportFunctions

void updatePhysics() {
    for (const auto& gameObject : GameObject::s_allObjects) {
        if (!gameObject->isActive()) continue;
        gameObject->physUpdate(deltaTimePhys);
    }

    const auto& collidingObjects = objectManager.getCollidingObjects();
    for (int i = 0; i < collidingObjects.size(); i++) {
        if (!collidingObjects[i]->isActive()) continue;

        for (int j = i + 1; j < collidingObjects.size(); j++) {
            if (!collidingObjects[j]->isActive()) continue;

            if (!collidingObjects[i]->collider->checkCollision(*collidingObjects[j]->collider) or
                !collidingObjects[j]->collider->checkCollision(*collidingObjects[i]->collider)) continue;

            collidingObjects[i]->onCollided(collidingObjects[j]);
            collidingObjects[j]->onCollided(collidingObjects[i]);
        }
    }
}

#pragma endregion

int main() {
    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);


    const std::string project_root_str(PROJECT_ROOT_PATH);
    const std::string explosionSheet_str("/assets/textures/explosion.png");
    const std::string full_path_str = project_root_str + explosionSheet_str;
    core::animation::AnimationSystem::Load("explosion", full_path_str.c_str(),
                                           {5, 5}, 0.04f, false);

    // Initialize camera
    gameCamera.camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    gameCamera.smoothSpeed = 5.0f;
    gameCamera.zoom = 1.0f;

    float DT = 0;


    objectManager.createObject<game::management::LevelManager>();
    
    while (!WindowShouldClose()) {
        const float frameTime = GetFrameTime(); // Store frame time for camera smoothing

        core::animation::AnimationSystem::Update(frameTime);

        // Physics update
        DT += frameTime;
        while (DT > deltaTimePhys) {
            updatePhysics();
            DT -= deltaTimePhys;
        }

        // Logic
        for (const auto& gameObject : game::management::GameObjectManager::getAllObjects()) {
            if (!gameObject->isActive()) continue;
            gameObject->logicUpdate();
        }

        // Update camera before rendering
        core::systems::CameraSystem::UpdateCamera(
            gameCamera,
            *game::game_objects::Player::GetInstance(),
            frameTime
        );

        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Begin camera mode
        core::systems::CameraSystem::BeginCameraDraw(gameCamera);

        for (auto* drawnObj : objectManager.getDrawnObjects()) {
            if (!drawnObj->isActive()) continue;

            drawnObj->draw();
        }

        core::animation::AnimationSystem::Draw();

        // DrawText(TextFormat("Animations: %d",
        //     core::animation::AnimationSystem::GetActiveCount()), 10, 40, 20, RED);

        core::systems::CameraSystem::EndCameraDraw();

        // Draw UI elements that shouldn't move with camera (like FPS counter)
        DrawFPS(10, 10);

        EndDrawing();

        // Cleanup
        objectManager.destroyObjectsToDestroy();
    }

    core::animation::AnimationSystem::UnloadAll();
    return 0;
}
