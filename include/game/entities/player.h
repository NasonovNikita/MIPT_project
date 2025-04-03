//
// Created by nosokvkokose on 03.04.25.
//

#ifndef PLAYER_H
#define PLAYER_H
#include <raylib.h>
#include <vector>

#include "components.h"
#include "units.h"
#include "game/gameObjects.h"

namespace game::game_objects {
    class Player final : public Unit {
        float angle_ = 0;
        float maxRotationSpeed_;
        float currentRotationSpeed_ = 0;
        float rotationAcceleration_ = 0;
        std::vector<Vector2> vertices = { Vector2 (0, 0), Vector2 (0, 0), Vector2 (0, 0)};
        static Player *s_instance;

        explicit Player(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float maxRotationSpeed):
        GameObject(tr), Unit(hp, maxSpeed),
        maxRotationSpeed_(maxRotationSpeed) {
            vertices = { tr.corner(), tr.corner() + Vector2 {0, tr.scaledSize().y}, tr.center + Vector2 {tr.scaledSize().x / 2, 0}};
            collider = new components::ColliderPoly(tr.center, vertices);
        }

    public:
        static Player *getInstance() { return s_instance; }

        static Player* SpawnPlayer(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float maxRotationSpeed) {
            s_instance = new Player(tr, hp, maxSpeed, maxRotationSpeed);
            return s_instance;
        }

        std::vector<Vector2> getVertices();

        void draw() override;
        void physUpdate(float deltaTime) override;
        void logicUpdate() override;
    };
}

#endif //PLAYER_H
