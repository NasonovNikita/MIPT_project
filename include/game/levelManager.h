//
// Created by nosokvkokose on 21.04.25.
//

#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "gameObjectManager.h"
#include "gameObjects.h"
#include "core/objectPool.h"
#include "entities/player.h"
#include "entities/units.h"

namespace game::game_objects {
    class Asteroid;
}

namespace game::management {
    class LevelManager final : public game_objects::GameObject {
        GameObjectManager& manager = GameObjectManager::getInstance();
        std::vector<std::shared_ptr<game_objects::Asteroid>> asteroids;
        core::object_pool::ObjectPool<game_objects::Asteroid> asteroidPool;

        int score = 0;

        void spawnAsteroids() {
            // Create objects through manager
            const auto newAst = manager.createObject<game_objects::Asteroid>(
                components::Transform2D(200, 200, 50, 50), 10, 50, 0);
            asteroids.push_back(newAst);
        }

        void setWorldBorders();

        void endLevel();

        void win();

        void lose();

        void restart();

        void cleanupAsteroidList() {
            for (const auto& asteroid : asteroids) {
                if (!asteroid->isDead())
                    asteroidPool.release(asteroid);
            }

            // Remove expired weak_ptrs
            std::erase_if(asteroids,
                          [](const std::shared_ptr<game_objects::Asteroid>& asteroid) {
                              return !asteroid->isActive();
                          });
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

        void debugDrawColliders() {
            const auto playerCollider = dynamic_cast<components::ColliderPoly *>
                (game::game_objects::Player::GetInstance()->collider);
            auto plOutBox = playerCollider->getCoveringBox();
            auto plInBox = playerCollider->getInnerBox();
            DrawRectangleLines(plOutBox.x, plOutBox.y, plOutBox.width, plOutBox.height, BLACK);
            DrawRectangleLines(plInBox.x, plInBox.y, plInBox.width, plInBox.height, BLACK);
            auto vertices = playerCollider->getVertices();
            DrawTriangleLines(vertices[0], vertices[1], vertices[2], RED);
            const auto asteroidCollider = dynamic_cast<
                components::ColliderCircle *>(asteroids[0]->collider);
            const auto astOutBox = asteroidCollider->getCoveringBox();
            const auto astInBox = asteroidCollider->getInnerBox();
            DrawRectangleLines(astOutBox.x, astOutBox.y, astOutBox.width,
                               astOutBox.height, BLACK);
            DrawRectangleLines(astInBox.x, astInBox.y, astInBox.width,
                               astInBox.height, BLACK);
        }
    public:
        LevelManager() : GameObject(components::Transform2D(0, 0, 0, 0))
        { }

        LevelManager(const LevelManager &other) = delete;
        LevelManager& operator=(const LevelManager &other) = delete;

        void logicUpdate() override {
            cleanupAsteroidList();
            if (asteroids.empty()) {
                TEMP_reviveAsteroid();
            }
        }

        void start() override {
            // Player (singleton pattern remains)
            game::game_objects::Player::SpawnPlayer(

                components::Transform2D(100, 400, 50, 50), 10, 300, 3);
            manager.registerExternalObject(game::game_objects::Player::GetInstance());

            spawnAsteroids();
        }
    };
}

#endif //LEVELMANAGER_H
