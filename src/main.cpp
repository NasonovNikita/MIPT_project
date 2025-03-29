#include <game/gameObjects.h>
#include <core/objectPool.h>

#include "core/objectManager.h"

constexpr int screenWidth = 1040;
constexpr int screenHeight = 1040;
constexpr float deltaTimePhys = 0.002f;

using game::game_objects::Asteroid;
using core::object_pool::ObjectPool;

static std::vector<Asteroid*> asteroids;

auto& manager = core::management::ObjectManager::Instance();

void updatePhysics() {
    for (const auto asteroid : asteroids) {
        if (!asteroid->isActive()) continue;

        auto tr = asteroid->getTransform();
        if (tr.center.x + tr.scaledSize().x / 2 >= screenWidth) {
            tr.center.x = screenWidth - tr.scaledSize().x / 2;
            asteroid->bounceByNormal({-1, 0});
        }
        if (tr.center.x - tr.scaledSize().x / 2 <= 0) {
            tr.center.x = screenWidth + tr.scaledSize().x / 2;
            asteroid->bounceByNormal({1, 0});
        }
        if (tr.center.y + tr.scaledSize().y / 2 > screenHeight) {
            tr.center.y = screenHeight - tr.scaledSize().y / 2;
            asteroid->bounceByNormal({0, -1});
        }
        if (tr.center.y - tr.scaledSize().y / 2 < 0) {
            tr.center.y = screenHeight + tr.scaledSize().y / 2;
            asteroid->bounceByNormal({0, 1});
        }
        asteroid->updateCollider();
    }

    for (const auto& gameObject : game::game_objects::GameObject::s_allObjects) {
        gameObject->physUpdate(deltaTimePhys);
    }

    const auto& collidingObjects = manager.GetCollidingObjects();
    for (int i = 0; i < collidingObjects.size(); i++) {
        for (int j = i; j < collidingObjects.size(); j++) {
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

    manager.CreateObject<game::game_objects::Bullet>(
        components::Transform2D(100, 200, 10, 10), 300);

    while (!WindowShouldClose()) {
        // Physics update
        for (auto* obj : manager.GetAllObjects()) {
            obj->physUpdate(deltaTimePhys);
        }

        // Collision detection
        auto& colliders = manager.GetCollidingObjects();
        for (size_t i = 0; i < colliders.size(); ++i) {
            for (size_t j = i + 1; j < colliders.size(); ++j) {
                if (colliders[i]->collider->checkCollision(*colliders[j]->collider)) {
                    colliders[i]->onCollided(colliders[j]);
                    colliders[j]->onCollided(colliders[i]);
                }
            }
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
    }

    return 0;
}
