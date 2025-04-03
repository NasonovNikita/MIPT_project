//
// Created by nosokvkokose on 03.04.25.
//

#ifndef BULLET_H
#define BULLET_H
#include "../gameObjects.h"

#include "units.h"

namespace game::game_objects {
    class Bullet final : public CollidingObject, public MovingObject, public DrawnGameObject {
    public:
        Bullet(const components::Transform2D &tr, const float maxSpeed, const float angle = 0):
        GameObject(tr), CollidingObject(new components::ColliderCircle(tr)),
        MovingObject(maxSpeed) {
            currentSpeed_ = {maxSpeed * cos(angle), maxSpeed * sin(angle)};
        }

        void draw() override {
            if (!isActive()) return;

            DrawCircle(static_cast<int>(transform_.center.x),
                       static_cast<int>(transform_.center.y),
                       transform_.scaledSize().x / 2, RED);
            DrawCircleLines(static_cast<int>(transform_.center.x),
                       static_cast<int>(transform_.center.y),
                       transform_.scaledSize().x / 2, YELLOW);
        }
        void physUpdate(const float deltaTime) override {
            CollidingObject::physUpdate(deltaTime);
            MovingObject::physUpdate(deltaTime);
        }
        void onCollided(CollidingObject *other) override {
            const auto asteroid = dynamic_cast<Asteroid*>(other);

            if (!asteroid) return;
            asteroid->takeDamage(10);
            setActive(false);
        }
    };
}

#endif //BULLET_H
