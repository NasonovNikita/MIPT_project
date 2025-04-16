#include "game/worldMap.h"
#include "game/gameObjectManager.h"
#include "game/entities/units.h"
#include "raylib.h"

namespace game::world {
    WorldMap::WorldMap(float radius, Vector2 center) :
        radius(radius), center(center) {
    }

    void WorldMap::Update() {
        // Check all active units for boundary crossing
        for (auto* obj : game::management::GameObjectManager::getAllObjects()) {
            if (obj->isActive() && IsOutOfBounds(obj->getTransform().center)) {
                if (auto* unit = dynamic_cast<game::game_objects::Unit*>(obj)) {
                    unit->takeDamage(999999); // Instant death
                }
            }
        }
    }

    void WorldMap::Draw() const {
        // Draw world boundary
        DrawCircleLines((int)center.x, (int)center.y, radius, RED);

        // Optional: Draw safe area indicator
        DrawCircleLines((int)center.x, (int)center.y, radius - 20, GREEN);
    }

    bool WorldMap::IsOutOfBounds(Vector2 position) const {
        return Vector2Distance(position, center) > radius;
    }
}