//
// Created by nosokvkokose on 03.04.25.
//

#include <raylib.h>
#include <raymath.h>
#include <vector>

#include "game/gameObjectManager.h"
#include "game/gameObjects.h"
#include "game/entities/bullet.h"
#include "game/entities/player.h"


constexpr int c_acceleration = 500;
constexpr int c_rotation = 10;

namespace game::game_objects {


    std::vector<Vector2> Player::getVertices() {
        return vertices;
    }

    Player *Player::s_instance;

    void Player::physUpdate(const float deltaTime) {
        Unit::physUpdate(deltaTime);

        vertices[0] += currentSpeed_ * deltaTime;
        vertices[1] += currentSpeed_ * deltaTime;
        vertices[2] += currentSpeed_ * deltaTime;

        if (rotationAcceleration_ == 0) {
            rotationAcceleration_  = -c_rotation * (currentRotationSpeed_ > 0 ? 1 : -1);
        }
        currentRotationSpeed_ += rotationAcceleration_ * deltaTime;
        if (currentRotationSpeed_ > maxRotationSpeed_) {
            currentRotationSpeed_ = maxRotationSpeed_;
        }
        if (-currentRotationSpeed_ > maxRotationSpeed_) {
            currentRotationSpeed_ = -maxRotationSpeed_;
        }


        const auto dAngle = currentRotationSpeed_ * deltaTime;

        angle_ += dAngle;
        if (angle_ > 180) angle_ -= 180;
        if (angle_ < -180) angle_ += 180;

        transform_.angle = angle_;

        vertices = { transform_.center + Vector2Rotate(vertices[0] - transform_.center, dAngle),
                         transform_.center + Vector2Rotate(vertices[1] - transform_.center, dAngle),
                         transform_.center + Vector2Rotate(vertices[2] - transform_.center, dAngle) };



        collider->rotate(dAngle);
    }

    void Player::logicUpdate() {
    #pragma region movement
        auto getNoseDirection = [this] {
            return Vector2Normalize(vertices[2] - transform_.center);
        };

        auto isPressedUp = [] { return IsKeyDown(KEY_UP) or IsKeyDown(KEY_W);};
        auto isPressedDown = [] { return IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_S); };
        auto isPressedLeft = [] { return IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_A); };
        auto isPressedRight = [] { return IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_D); };

        if (!(isPressedUp() or isPressedDown())) {
            acceleration_ = 0;
            if (fabs(getSpeedModule()) <= 50) {
                currentSpeed_ = {0, 0};
            }
        }

        if (!(isPressedLeft() or isPressedRight())) {
            rotationAcceleration_ = 0;
            if (fabs(currentRotationSpeed_) <= 1)
                currentRotationSpeed_ = 0;
        }

        if (isPressedUp()) {
            accelerationDirection = getNoseDirection();
            acceleration_ = c_acceleration;
        }

        if (isPressedDown()) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = -c_acceleration;
        }

        if (isPressedRight()) {
            rotationAcceleration_ = c_rotation;
        }

        if (isPressedLeft()) {
            rotationAcceleration_ = -c_rotation;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) or IsKeyDown(KEY_RIGHT_CONTROL)) {
            accelerationDirection = Vector2Normalize(Vector2Negate(currentSpeed_));
            acceleration_ = c_acceleration;
        }
    #pragma endregion

        // Shoot
        if (canShoot() and (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) or IsKeyPressed(KEY_J))) {
            management::GameObjectManager::getInstance().createObject<Bullet>(
        components::Transform2D(vertices[2].x, vertices[2].y, 10, 10), 300, angle_);
            shootTimeOut = 0.2;
        }

        // Dash
        if (IsKeyPressed(KEY_SPACE) and canDash()) {
            Vector2 direction = {0, 0};
            bool hasInput = false;
            const Vector2 noseDir = getNoseDirection(); // Store once to avoid repeated calls

            if (isPressedUp() && !isPressedDown()) {
                direction = Vector2Add(direction, noseDir);
                hasInput = true;
            }
            if (isPressedDown() && !isPressedUp()) {
                direction = Vector2Subtract(direction, noseDir);
                hasInput = true;
            }
            if (isPressedLeft() && !isPressedRight()) {
                const Vector2 leftDir = {noseDir.y, -noseDir.x};  // -90 degrees rotation
                direction = Vector2Add(direction, leftDir);
                hasInput = true;
            }
            if (isPressedRight() && !isPressedLeft()) {
                const Vector2 rightDir = {-noseDir.y, noseDir.x};  // 90 degrees rotation
                direction = Vector2Add(direction, rightDir);
                hasInput = true;
            }

            if (!hasInput)
                direction = noseDir;

            dash(Vector2Normalize(direction), maxSpeed_ * 2);
        }

        // Timers
        if (invincibilityTime_ > 0)
            invincibilityTime_ -= GetFrameTime();
        if (shootTimeOut > 0)
            shootTimeOut -= GetFrameTime();
        if (dashTimeOut > 0)
            dashTimeOut -= GetFrameTime();
        if (dashingTime_ > 0) {
            dashingTime_ -= GetFrameTime();
        }
        else if (maxSpeed_ > maxSpeedDashless_) {
            maxSpeed_ -= 20;
        } else {
            maxSpeed_ = maxSpeedDashless_;
        }
    }

    void Player::takeDamage(const int value) {
        if (isInvincible()) return;

        Unit::takeDamage(value);
    }

    void Player::dash(const Vector2 direction, const float speed) {
        currentSpeed_ = direction * speed;
        maxSpeed_ = speed;
        dashTimeOut = 4;
        dashingTime_ = 0.5;
        invincibilityTime_ = 0.5;
    }
}
