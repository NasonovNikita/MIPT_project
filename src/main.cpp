#include <iostream>

#include "core/objectPool.h"
#include "game/gameObjects.h"
#include "game/gameObjectManager.h"
#include "game/entities/player.h"
#include "core/cameraSystem.h"
#include "core/buttonSystem.h"
#include "core/animation.h"
#include "game/levelManager.h"

constexpr int screenWidth = 1040;
constexpr int screenHeight = 1040;
constexpr float deltaTimePhys = 1.f / 60 / 2;
constexpr Vector2 center = {screenWidth / 2.0f, screenHeight / 2.0f};

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

    const std::unique_ptr<components::TextureComponent> BackGround = std::make_unique<
        components::TextureComponent>(textures::background.c_str());

    core::animation::AnimationSystem::LoadAll();
    // Initialize camera
    gameCamera.camera.offset = center;
    gameCamera.smoothSpeed = 5.0f;
    gameCamera.zoom = 0.75f;

    float DT = 0;

    const auto levelManager = objectManager.createObject<game::management::LevelManager>();

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

        core::button::ButtonSystem::Update();

        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Begin camera mode
        core::systems::CameraSystem::BeginCameraDraw(gameCamera);

        BackGround->Draw(components::Transform2D(
            center.x, center.y, BackGround->getTexture().width,
            BackGround->getTexture().height));


        for (auto* drawnObj : objectManager.getDrawnObjects()) {
            if (!drawnObj->isActive()) continue;

            drawnObj->draw();
        }

        core::animation::AnimationSystem::Draw();
        //
        core::button::ButtonSystem::Draw(game::game_objects::Player::GetInstance()->getTransform());

        core::systems::CameraSystem::EndCameraDraw();

        // Draw UI elements that shouldn't move with camera (like FPS counter)
        DrawFPS(10, 10);
        DrawText(TextFormat("Score: %d", levelManager->getScore()),
            10, 70, 20, RED);

        EndDrawing();

        // Cleanup
        objectManager.destroyObjectsToDestroy();
    }

    core::animation::AnimationSystem::UnloadAll();
    return 0;
}
