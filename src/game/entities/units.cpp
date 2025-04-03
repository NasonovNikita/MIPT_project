//
// Created by nosokvkokose on 03.04.25.
//

#include "game//gameObjects.h"
#include "game/entities/units.h"

#include "game/entities/player.h"

namespace game::game_objects {
    void Unit::die() {
        setActive(false);
    }

    void Unit::takeDamage(const int value) {
        hp_.ChangeValue(-value);

        if (hp_.getValue() == 0) {
            die();
        }
    }

    void Asteroid::draw() {
        if (!isActive()) return;

        DrawCircle(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, GRAY);
        DrawCircleLines(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, BLACK);
    }

    void Asteroid::onCollided(CollidingObject *other) {
        if (other == this) return;

        if (const auto otherAsteroid = dynamic_cast<Asteroid*>(other)) {
            const Vector2 collisionNormal = collider->getCollisionNormal(*other->collider);
            bounceFromOther(*otherAsteroid, collisionNormal);

            resolveCollision(*other);
        }

        if (other == Player::getInstance()) {
            takeDamage(5);
        }
    }
}
