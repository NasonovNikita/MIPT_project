#include "game/worldMap.h"
#include "game/gameObjectManager.h"
#include "game/entities/units.h"
#include "raylib.h"

namespace game::world {
    WorldMap::WorldMap(const float radius, const Vector2 center) :
        radius(radius), center(center) {
    }

    void WorldMap::Update() const {
        // Check all active units for boundary crossing
        for (auto* obj : game::management::GameObjectManager::getAllObjects()) {
            if (obj->isActive() && IsOutOfBounds(obj->getTransform().center)) {
                if (auto* unit = dynamic_cast<game::game_objects::Unit*>(obj)) {
                    unit->takeDamage(INFINITY); // Instant death
                }
            }
        }
    }

    void WorldMap::Draw() const {
        // Draw world boundary
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius, RED);

        // Optional: Draw safe area indicator
        DrawCircleLines(static_cast<int>(center.x), static_cast<int>(center.y), radius - 20, GREEN);
    }

    bool WorldMap::IsOutOfBounds(const Vector2 position) const {
        return Vector2Distance(position, center) > radius;
    }
}