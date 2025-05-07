//
// Created by nosokvkokose on 21.04.25.
//

#ifndef LEVELMANAGER_H
#define LEVELMANAGER_H

#include "gameObjectManager.h"
#include "gameObjects.h"
#include "texturePaths.h"
#include "worldMap.h"
#include "core/objectPool.h"
#include "entities/player.h"
#include "entities/units.h"

constexpr int SCREEN_WIDTH = 1040;
constexpr int SCREEN_HEIGHT = 1040;
constexpr float WORLD_RADIUS = 1000.f;
constexpr Vector2 WORLD_CENTER = {SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f};

namespace game::game_objects {
    class Asteroid;
}

namespace game::management {
    class LevelManager final : public game_objects::GameObject {
        GameObjectManager& manager = GameObjectManager::getInstance();
        std::vector<std::shared_ptr<game_objects::Asteroid>> asteroids;
        core::object_pool::ObjectPool<game_objects::Asteroid> asteroidPool;
        world::WorldMap worldMap;

        int preferredAsteroidsCount = 0;

        int score = 0;

        void spawnAsteroids(const int count) {
            for (int i = 0; i < count; i++) {
                // Random position within world radius
                // Get player instance and check if exists
                const auto player = game_objects::Player::GetInstance();
                Vector2 pos;
                float size;
                bool validPosition = false;

                // Keep generating positions until we find one that's far enough from player
                while (!validPosition) {
                    const float spawnAngle = GetRandomValue(0, 360) * DEG2RAD;
                    const float distance = GetRandomValue(0, WORLD_RADIUS);
                    pos = {
                    WORLD_CENTER.x + cosf(spawnAngle) * distance,
                    WORLD_CENTER.y + sinf(spawnAngle) * distance
                };

                // Random size (adjust min/max as needed)
                    size = GetRandomValue(30, 70);

                    // If no player or far enough from player, position is valid
                    if (constexpr float minDistanceFromPlayer = 150.f;
                        !player || Vector2Distance(
                            pos, player->getTransform().center) >
                        minDistanceFromPlayer + size) {
                        validPosition = true;
                    }
                }

                // Random speed (0.5 to maxSpeed)
                float speed = GetRandomValue(50, 100); // Adjust range as needed

                const auto newAst = manager.createObject<game_objects::Asteroid>(
                    components::Transform2D(pos.x, pos.y, size, size),
                    10,  // HP
                    1000, // maxSpeed (adjust as needed)
                    speed // currentSpeed (randomized)
                );
                const float speedAngle = GetRandomValue(0, 360) * DEG2RAD;
                newAst->setDirectionOfSpeed(Vector2Rotate(Vector2One(), speedAngle));

                asteroids.push_back(newAst);
            }
        }

        void reviveAsteroids() {
            const int deadAsteroids = preferredAsteroidsCount - static_cast<int>(asteroids.size());
            score += deadAsteroids;
            spawnAsteroids(deadAsteroids);
        }

        void setWorldBorders() {
            manager.registerExternalObject(&worldMap);
        }

        void endLevel() {
            for (const auto& asteroid : asteroids) {
                asteroid->destroy();
            }
            asteroids.clear();

            const auto player = game_objects::Player::GetInstance();
            if (player) {
                player->destroy();
            }
        }

        void win() {}

        void lose() {}

        void restart() {
            score = 0;
            endLevel();
            startLevel();
        }

        void startLevel() {
            const auto player = game_objects::Player::SpawnPlayer(
                components::Transform2D(WORLD_CENTER, {50, 50}), 10, 300, 3);
            player->LoadTexture(textures::playerTexture.c_str());
            manager.registerExternalObject(game_objects::Player::GetInstance());

            preferredAsteroidsCount = 15;
            spawnAsteroids(preferredAsteroidsCount);
            setWorldBorders();
        }

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

        void debugDrawColliders() {
            const auto playerCollider = dynamic_cast<components::ColliderPoly *>
                (game_objects::Player::GetInstance()->collider);
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
        LevelManager() : GameObject(components::Transform2D(0, 0, 0, 0)),
                         worldMap(WORLD_RADIUS, WORLD_CENTER) {
        }

        LevelManager(const LevelManager &other) = delete;
        LevelManager& operator=(const LevelManager &other) = delete;

        void logicUpdate() override {
            cleanupAsteroidList();
            reviveAsteroids();

            const auto player = game_objects::Player::GetInstance();
            if (player && player->isDead()) {
                lose();
                restart();
            }
        }

        void start() override {
            startLevel();
        }

        [[nodiscard]] int getScore() const { return score; }
    };
}

#endif //LEVELMANAGER_H
