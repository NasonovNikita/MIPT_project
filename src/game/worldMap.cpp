#include "game/worldMap.h"
#include "game/gameObjectManager.h"
#include "game/entities/units.h"
#include "raylib.h"
#include "game/entities/bullet.h"

namespace game::world {
    void WorldMap::logicUpdate() {
        // Check all active units for boundary crossing
        for (auto* obj : management::GameObjectManager::getAllObjects()) {
            if (obj->isActive() && isOutOfBounds(obj->getTransform().center)) {
                if (auto* unit = dynamic_cast<game_objects::Unit*>(obj)) {
                    const auto collisionNormal = Vector2Normalize(
                        Vector2Subtract(obj->getTransform().center, center));
                    unit->bounceByNormal(collisionNormal);
                }
                else if (auto* bullet = dynamic_cast<game_objects::Bullet*>(obj)) {
                    bullet->destroy();
                }
            }
        }
    }

    void WorldMap::draw() {
        // Draw world boundary
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, RED);

        // Optional: Draw safe area indicator
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius - 20, GREEN);
    }

    bool WorldMap::isOutOfBounds(const Vector2 position) const {
        return Vector2Distance(position, center) > radius;
    }
}
