#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include "gameObjects.h"
#include "components.h"

namespace game::world {
    class WorldMap final : public game_objects::DrawnGameObject {
        float radius;  // World boundary radius
        Vector2 center; // World center position

    public:
        explicit
        WorldMap(const float radius, const Vector2 center = {0, 0}) : GameObject(
        components::Transform2D(center, {radius, radius})), radius(radius),
            center(center) {
        }

        void logicUpdate() override;
        void draw() override;

        // Check if position is outside world bounds
        [[nodiscard]] bool isOutOfBounds(Vector2 position) const;

        [[nodiscard]] float getRadius() const { return radius; }
        [[nodiscard]] Vector2 getCenter() const { return center; }
    };
}

#endif // WORLD_MAP_H