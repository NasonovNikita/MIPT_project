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

#include <iostream>


constexpr int c_acceleration = 500;
constexpr int c_rotation = 10;

constexpr float c_dashTimeOut = 4;
constexpr float c_dashTime = 0.5;
constexpr float c_dashInvincibilityTime = 0.5;

constexpr int c_contactDamage = 5;
constexpr float c_afterContactControlBlockTime = 0.2;
constexpr float c_damageInvincibilityTime = 1.5;
constexpr float c_damageImpulse = 0.5;

namespace game::game_objects {

    std::vector<Vector2> Player::getVertices() const {
        return {
            transform_.center + verticesOffsets[0],
            transform_.center + verticesOffsets[1],
            transform_.center + verticesOffsets[2]
        };
    }

    Player *Player::s_instance;

    void Player::physUpdate(const float deltaTime) {
        Unit::physUpdate(deltaTime);

        if (rotationAcceleration_ == 0 and currentRotationSpeed_ != 0) {
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

        verticesOffsets = { Vector2Rotate(verticesOffsets[0], dAngle),
                         Vector2Rotate(verticesOffsets[1], dAngle),
                         Vector2Rotate(verticesOffsets[2], dAngle) };



        collider->rotate(dAngle);
    }

    void Player::logicUpdate() {
    #pragma region movement
        auto getNoseDirection = [this] {
            return Vector2Normalize(verticesOffsets[2]);
        };

        auto isPressedUp = [] { return IsKeyDown(KEY_UP) or IsKeyDown(KEY_W); };
        auto isPressedDown = [] { return IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_S); };
        auto isPressedLeft = [] { return IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_A); };
        auto isPressedRight = [] { return IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_D); };

        if (!(isPressedUp() or isPressedDown()) and canControl()) {
            acceleration_ = 0;
            if (fabs(getSpeedModule()) <= 50) {
                currentSpeed_ = {0, 0};
            }
        }

        if (!(isPressedLeft() or isPressedRight()) and canControl()) {
            rotationAcceleration_ = 0;
            if (fabs(currentRotationSpeed_) <= 1)
                currentRotationSpeed_ = 0;
        }

        if (isPressedUp() and canControl()) {
            accelerationDirection = getNoseDirection();
            acceleration_ = c_acceleration;
        }

        if (isPressedDown() and canControl()) {
            accelerationDirection = Vector2Negate(getNoseDirection());
            acceleration_ = c_acceleration;
        }

        if (isPressedRight() and canControl()) {
            rotationAcceleration_ = c_rotation;
        }

        if (isPressedLeft() and canControl()) {
            rotationAcceleration_ = -c_rotation;
        }

        if (IsKeyDown(KEY_LEFT_CONTROL) or IsKeyDown(KEY_RIGHT_CONTROL)) {  // Control allowed always
            accelerationDirection = Vector2Normalize(Vector2Negate(currentSpeed_));
            acceleration_ = c_acceleration;
        }
    #pragma endregion

        // Shoot
        if (canShoot() and (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) or IsKeyDown(KEY_J))) {
            management::GameObjectManager::getInstance().createObject<Bullet>(
        components::Transform2D(getVertices()[2].x, getVertices()[2].y, 10, 10), 300, angle_);
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

            dash(Vector2Normalize(direction), maxSpeed_ * 1.5);
        }

        // Timers
        if (dashInvincibilityTime_ > 0)
            dashInvincibilityTime_ -= GetFrameTime();
        if (damageInvincibilityTime_ > 0)
            damageInvincibilityTime_ -= GetFrameTime();
        if (shootTimeOut > 0)
            shootTimeOut -= GetFrameTime();
        if (dashTimeOut > 0)
            dashTimeOut -= GetFrameTime();
        if (cantControlTime_ > 0)
            cantControlTime_ -= GetFrameTime();
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
        dashInvincibilityTime_ = c_damageInvincibilityTime;
    }

    void Player::dash(const Vector2 direction, const float speed) {
        currentSpeed_ = direction * speed;
        maxSpeed_ = speed;
        dashTimeOut = c_dashTimeOut;
        dashingTime_ = c_dashTime;
        dashInvincibilityTime_ = c_dashInvincibilityTime;
    }

    void Player::onCollided(CollidingObject *other) {
        if (const auto unit = dynamic_cast<Unit*>(other)) {
            if (unit->isEnemy() and !isInvincible()) {
                takeDamage(c_contactDamage);
                const auto collisionNormal = unit->collider->getCollisionNormal(*collider);
                currentSpeed_ = collisionNormal * maxSpeed_ * c_damageImpulse;
                cantControlTime_ = c_afterContactControlBlockTime;

                resolveCollision(*unit);
            }
        }
    }

    void Player::draw()  {
        const auto vertices = getVertices();

        if (!isInvincible())
            DrawTriangle(vertices[1], vertices[0], vertices[2], GREEN);
        else if (isDashing())
            DrawTriangle(vertices[1], vertices[0], vertices[2], BLUE);
        else if (dashInvincibilityTime_ > 0)
            DrawTriangle(vertices[1], vertices[0], vertices[2], RED);

        DrawTriangleLines(vertices[1], vertices[0], vertices[2], BLUE);
    }


}
