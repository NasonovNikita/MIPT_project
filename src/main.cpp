#include <iostream>

#include "core/objectPool.h"
#include "game/gameObjects.h"
#include "game/gameObjectManager.h"
#include "game/entities/player.h"
#include "game/entities/units.h"
#include "core/cameraSystem.h"

constexpr int screenWidth = 1040;
constexpr int screenHeight = 1040;
constexpr float deltaTimePhys = 0.002f;

using game::game_objects::Asteroid;
using core::object_pool::ObjectPool;
using game::game_objects::GameObject;

static std::vector<std::shared_ptr<Asteroid>> asteroids;
static ObjectPool<Asteroid> asteroidPool;

auto& manager = game::management::GameObjectManager::getInstance();

components::GameCamera gameCamera;

#pragma region SupportFunctions
void CleanupAsteroidList() {
    for (const auto& asteroid : asteroids) {
        if (!asteroid->isActive()) asteroidPool.release(asteroid);
    }

    // Remove expired weak_ptrs
    std::erase_if(asteroids,
                  [](const std::shared_ptr<Asteroid>& asteroid) {
                      return !asteroid->isActive();
                  });
}

void updatePhysics() {
    for (const auto& asteroid : asteroids) {
        if (const auto asteroidPtr = asteroid.get()) {
            if (!asteroidPtr->isActive()) continue;

            auto tr = asteroidPtr->getTransform();
            if (tr.center.x + tr.scaledSize().x / 2 >= screenWidth) {
                tr.center.x = screenWidth - tr.scaledSize().x / 2;
                asteroidPtr->bounceByNormal({-1, 0});
            }
            if (tr.center.x - tr.scaledSize().x / 2 <= 0) {
                tr.center.x = screenWidth + tr.scaledSize().x / 2;
                asteroidPtr->bounceByNormal({1, 0});
            }
            if (tr.center.y + tr.scaledSize().y / 2 > screenHeight) {
                tr.center.y = screenHeight - tr.scaledSize().y / 2;
                asteroidPtr->bounceByNormal({0, -1});
            }
            if (tr.center.y - tr.scaledSize().y / 2 < 0) {
                tr.center.y = screenHeight + tr.scaledSize().y / 2;
                asteroidPtr->bounceByNormal({0, 1});
            }
            asteroidPtr->updateCollider();
        }
    }

    for (const auto& gameObject : GameObject::s_allObjects) {
        if (!gameObject->isActive()) continue;
        gameObject->physUpdate(deltaTimePhys);
    }

    const auto& collidingObjects = manager.getCollidingObjects();
    for (int i = 0; i < collidingObjects.size(); i++) {
        if (!collidingObjects[i]->isActive()) continue;

        for (int j = i; j < collidingObjects.size(); j++) {
            if (!collidingObjects[j]->isActive()) continue;

            if (!collidingObjects[i]->collider->checkCollision(*collidingObjects[j]->collider)) continue;

            collidingObjects[i]->onCollided(collidingObjects[j]);
            collidingObjects[j]->onCollided(collidingObjects[i]);
        }
    }
}

void TEMP_reviveAsteroid() {
    if (asteroids.empty()) {
        const auto acquiredAsteroid = asteroidPool.acquire();
        acquiredAsteroid->setActive(true);
        acquiredAsteroid->getTransform().center = Vector2(
            GetRandomValue(100, 700), GetRandomValue(100, 700));
        manager.registerExternalObject(acquiredAsteroid.get());
        asteroids.push_back(acquiredAsteroid);
    }
}

#pragma endregion

int main() {
    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);

    // Initialize camera
    gameCamera.camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
    gameCamera.smoothSpeed = 5.0f;
    gameCamera.zoom = 1.0f;

    // Player (singleton pattern remains)
    game::game_objects::Player::SpawnPlayer(
        components::Transform2D(100, 400, 50, 50), 100, 300, 3);
    manager.registerExternalObject(game::game_objects::Player::getInstance());

    // Create objects through manager
    const auto newAst = manager.createObject<Asteroid>(
        components::Transform2D(200, 200, 50, 50), 10, 50, 0);
    asteroids.push_back(newAst);

    float DT = 0;

    while (!WindowShouldClose()) {
        float frameTime = GetFrameTime(); // Store frame time for camera smoothing

        // Physics update
        DT += frameTime;
        while (DT > deltaTimePhys) {
            updatePhysics();
            DT -= deltaTimePhys;
        }

        // Update camera before rendering
        core::systems::CameraSystem::UpdateCamera(
            gameCamera,
            *game::game_objects::Player::getInstance(),
            frameTime
        );

        // Logic
        for (const auto& gameObject : game::management::GameObjectManager::getAllObjects()) {
            if (!gameObject->isActive()) continue;
            gameObject->logicUpdate();
        }

        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);

        // Begin camera mode
        core::systems::CameraSystem::BeginCameraDraw(gameCamera);

        for (auto* drawnObj : manager.getDrawnObjects()) {
            drawnObj->draw();
        }

        core::systems::CameraSystem::EndCameraDraw();

        // Draw UI elements that shouldn't move with camera (like FPS counter)
        DrawFPS(10, 10);

        EndDrawing();

        // Cleanup
        manager.destroyInactive();
        CleanupAsteroidList();
        TEMP_reviveAsteroid();
    }

    return 0;
}
