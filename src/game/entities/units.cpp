//
// Created by nosokvkokose on 03.04.25.
//

#include "game//gameObjects.h"
#include "game/entities/units.h"
#include "core/animation.h"
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

    void Asteroid::takeDamage(const int value) {
        // hp_.ChangeValue(-value);
        Unit::takeDamage(value);

        if(!isActive()) {
            // Create explosion transform at player position
            components::Transform2D explosionTransform = getTransform();
            explosionTransform.size *= 3; // Larger size for visibility

            // Play explosion animation
            core::animation::AnimationSystem::Play("asteroidExplosion", explosionTransform);
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
    }
}
