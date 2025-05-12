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
#include "UI/buttonSystem.h"

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

        void spawnAsteroids(int count);

        void reviveAsteroids() {
            const int deadAsteroids = preferredAsteroidsCount - static_cast<int>(asteroids.size());
            score += deadAsteroids;
            spawnAsteroids(deadAsteroids);
        }

        void setWorldBorders() {
            manager.registerExternalObject(&worldMap);
        }

        void endLevel();

        void win() {}

        void lose() {}

        void restart() {
            score = 0;
            endLevel();
            startLevel();
        }

        void startLevel();

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

    public:
        LevelManager() : GameObject(components::Transform2D(0, 0, 0, 0)),
                         worldMap(WORLD_RADIUS, WORLD_CENTER) {
        }

        LevelManager(const LevelManager &other) = delete;
        LevelManager& operator=(const LevelManager &other) = delete;

        void logicUpdate() override {
            cleanupAsteroidList();
            reviveAsteroids();

            if (const auto player = game_objects::Player::GetInstance(); player && player->isDead()) {
                core::button::ButtonSystem::setInvisibility((std::string)"restart", false);
            }
        }

        void start() override {
            startLevel();
        }

        [[nodiscard]] int getScore() const { return score; }
    };
}

#endif //LEVELMANAGER_H
