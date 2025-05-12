//
// Created by nosokvkokose on 12.05.25.
//

#include "game/levelManager.h"

namespace game::management {
    void LevelManager::startLevel() {
        const auto player = game_objects::Player::SpawnPlayer(
            components::Transform2D(WORLD_CENTER, {50, 50}), 10, 300, 3);
        player->LoadTexture(textures::playerTexture.c_str());
        manager.registerExternalObject(game_objects::Player::GetInstance());

        preferredAsteroidsCount = 15;
        spawnAsteroids(preferredAsteroidsCount);
        core::button::ButtonSystem::Load(
            "restart",
            textures::restartButtonTexture.c_str(),
            {3, 1},
            Rectangle{ player->getTransform().corner().x - 90, player->getTransform().corner().y - 40, 180, 80},
            [this]() {
                // ������ �������� ����
                lose();
                restart();
            }
        );
        setWorldBorders();
    }

    void LevelManager::spawnAsteroids(const int count) {
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
                    const float distance = GetRandomValue(0, WORLD_RADIUS * 0.9f);
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
                newAst->LoadTexture(textures::asteroidTexture.c_str());

                asteroids.push_back(newAst);
            }
        }

    void LevelManager::endLevel() {
        for (const auto& asteroid : asteroids) {
            asteroid->destroy();
        }
        asteroids.clear();

        if (const auto player = game_objects::Player::GetInstance()) {
            player->destroy();
        }
    }

}