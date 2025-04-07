//
// Created by nosokvkokose on 03.04.25.
//

#ifndef UNITS_H
#define UNITS_H
#include "game/stats.h"

namespace game::game_objects {
    class Unit : public CollidingObject, public DrawnGameObject, public MovingObject {
        stats::Stat hp_ {0};
    protected:

        void die();
    public:
        explicit Unit(const int hp, const float maxSpeed):
        MovingObject(maxSpeed), hp_(hp) {}

        [[nodiscard]] bool virtual isEnemy() = 0;

        void virtual takeDamage(int value);

        void forceDie() { die(); }

        void physUpdate(const float deltaTime) override {
            MovingObject::physUpdate(deltaTime);
            CollidingObject::physUpdate(deltaTime);
        }
    };


    class Asteroid final : public Unit {
    public:
        Asteroid(const components::Transform2D &tr, const int hp, const float maxSpeed, const float currentSpeed=-1):
        GameObject(tr), Unit(hp, maxSpeed) {
            if (abs(currentSpeed + 1) < 0.01f)
                currentSpeed_ = Vector2Normalize(currentSpeed_) * maxSpeed;
            else currentSpeed_ = Vector2Normalize(currentSpeed_) * currentSpeed;

            collider = new components::ColliderCircle(tr);
        }

        bool isEnemy() override { return true; }

        void setCenter(const float x, const float y) {
            transform_.center = Vector2(x, y);
            updateCollider();
        }

        void setDirectionOfSpeed(Vector2 const sp) {
            currentSpeed_ = sp * Vector2Length(currentSpeed_);
        }

        void draw() override;

        void onCollided(CollidingObject *other) override;
    };
}

#endif //UNITS_H
