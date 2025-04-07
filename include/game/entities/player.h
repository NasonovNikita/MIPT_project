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
        float maxSpeedDashless_;
        float angle_ = 0;
        float maxRotationSpeed_;
        float currentRotationSpeed_ = 0;
        float rotationAcceleration_ = 0;

        std::vector<Vector2> vertices = { Vector2 (0, 0), Vector2 (0, 0), Vector2 (0, 0)};
        static Player *s_instance;

        float shootTimeOut = 0;
        float dashTimeOut = 0;
        float dashingTime_ = 0;
        float dashInvincibilityTime_ = 0;
        float damageInvincibilityTime_ = 0;
        float cantControlTime_ = 0;

        explicit Player(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float maxRotationSpeed):
        GameObject(tr), Unit(hp, maxSpeed),
        maxRotationSpeed_(maxRotationSpeed) {
            maxSpeedDashless_ = maxSpeed;
            vertices = { tr.corner(), tr.corner() + Vector2 {0, tr.scaledSize().y}, tr.center + Vector2 {tr.scaledSize().x / 2, 0}};
            collider = new components::ColliderPoly(tr.center, vertices);
        }

    public:
        static Player *getInstance() {
            return s_instance;
        }

        static Player* SpawnPlayer(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float maxRotationSpeed) {
            s_instance = new Player(tr, hp, maxSpeed, maxRotationSpeed);
            return s_instance;
        }

        bool isEnemy() override { return false; }

        [[nodiscard]] bool isInvincible() const {
            return dashInvincibilityTime_ > 0 or damageInvincibilityTime_ > 0;
        }
        [[nodiscard]] bool canControl() const { return cantControlTime_ <= 0; }
        [[nodiscard]] bool canShoot() const { return shootTimeOut <= 0; }
        [[nodiscard]] bool canDash() const { return dashTimeOut <= 0; }
        [[nodiscard]] bool isDashing() const { return dashingTime_ > 0; }

        std::vector<Vector2> getVertices();

        void draw() override;

        void physUpdate(float deltaTime) override;
        void logicUpdate() override;
        void takeDamage(int value) override;

        void dash(Vector2 direction, float speed);

        void onCollided(CollidingObject *other) override;
    };
}

#endif //PLAYER_H
