#include <game/gameObjects.h>
#include <core/objectPool.h>

#include "core/objectManager.h"

constexpr int screenWidth = 1040;
constexpr int screenHeight = 1040;
constexpr float deltaTimePhys = 0.002f;

using game::game_objects::Asteroid;
using core::object_pool::ObjectPool;

static std::vector<std::weak_ptr<Asteroid>> asteroids;

auto& manager = core::management::ObjectManager::Instance();

void CleanupAsteroidList() {
    // Remove expired weak_ptrs
    std::erase_if(asteroids,
                  [](const std::weak_ptr<Asteroid>& weakAsteroid) {
                      return weakAsteroid.expired(); // True if object was deleted
                  });
}

void updatePhysics() {
    for (const auto& asteroid : asteroids) {
        if (const auto asteroidPtr = asteroid.lock()) {
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

    for (const auto& gameObject : game::game_objects::GameObject::s_allObjects) {
        if (!gameObject->isActive()) continue;
        gameObject->physUpdate(deltaTimePhys);
    }

    const auto& collidingObjects = manager.GetCollidingObjects();
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

int main() {
    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);

    // Player (singleton pattern remains)
    game::game_objects::Player::SpawnPlayer(
        components::Transform2D(100, 400, 50, 50), 100, 300, 10);
    manager.RegisterExternalObject(game::game_objects::Player::getInstance());

    // Create objects through manager
    asteroids.emplace_back(manager.CreateObject<Asteroid>(
        components::Transform2D(200, 200, 50, 50), 10, 50, 0));

    float DT = 0;

    while (!WindowShouldClose()) {
        // Physics update
        DT += GetFrameTime();
        while (DT > deltaTimePhys) {
            updatePhysics();
            DT -= deltaTimePhys;
        }

        // Logic
        for (const auto& gameObject : game::game_objects::GameObject::s_allObjects) {
            if (!gameObject->isActive()) continue;
            gameObject->logicUpdate();
        }

        // Rendering
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (auto* drawnObj : manager.GetDrawnObjects()) {
            drawnObj->draw();
        }
        EndDrawing();

        // Cleanup
        manager.DestroyInactive();
        CleanupAsteroidList();
    }

    return 0;
}
