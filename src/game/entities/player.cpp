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

namespace game::game_objects {


    std::vector<Vector2> Player::getVertices() {
        return vertices;
    }


    Player *Player::s_instance;

    void Player::draw() {
        DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

        DrawTriangleLines(vertices[0], vertices[1], vertices[2], BLUE);
    }

    void Player::physUpdate(float deltaTime) {
        Unit::physUpdate(deltaTime);

        vertices[0] += currentSpeed_ * deltaTime;
        vertices[1] += currentSpeed_ * deltaTime;
        vertices[2] += currentSpeed_ * deltaTime;

        currentRotationSpeed_ += rotationAcceleration_ * deltaTime;
        if (currentRotationSpeed_ > maxRotationSpeed_) {
            currentRotationSpeed_ = maxRotationSpeed_;
        }
        if (-currentRotationSpeed_ > maxRotationSpeed_) {
            currentRotationSpeed_ = -maxRotationSpeed_;
        }

        const auto dAngle_ = currentRotationSpeed_ * deltaTime;

        angle_ += dAngle_;
        if (angle_ > 180) angle_ -= 180;
        if (angle_ < -180) angle_ += 180;

        transform_.angle = angle_;

        vertices = { transform_.center + Vector2Rotate(vertices[0] - transform_.center, dAngle_),
                         transform_.center + Vector2Rotate(vertices[1] - transform_.center, dAngle_),
                         transform_.center + Vector2Rotate(vertices[2] - transform_.center, dAngle_) };



        collider->rotate(dAngle_);
    }

    void Player::logicUpdate() {
        if (!(IsKeyDown(KEY_UP) or IsKeyDown(KEY_W) or IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_S))) {
            acceleration_ = 0;
            if (fabs(getSpeedModule()) <= 50) {
                currentSpeed_ = {0, 0};
            }
        }

        if (!(IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_A) or IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_D))) {
            rotationAcceleration_ = 0;
            if (fabs(currentRotationSpeed_) <= 1)
                currentRotationSpeed_ = 0;
        }

        if (IsKeyDown(KEY_UP) or IsKeyDown(KEY_W)) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = 500;
        }

        if (IsKeyDown(KEY_DOWN) or IsKeyDown(KEY_S)) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = -500;
        }

        if (IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_D)) {
            rotationAcceleration_ = 10;
        }

        if (IsKeyDown(KEY_LEFT) or IsKeyDown(KEY_A)) {
            rotationAcceleration_ = -10;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            management::GameObjectManager::getInstance().createObject<Bullet>(
        components::Transform2D(vertices[2].x, vertices[2].y, 10, 10), 300, angle_);
        }
    }
}
