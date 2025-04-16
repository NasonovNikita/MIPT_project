#ifndef WORLD_MAP_H
#define WORLD_MAP_H

#include "gameObjects.h"
#include "components.h"

namespace game::world {
    class WorldMap {
    private:
        float radius;  // World boundary radius
        Vector2 center; // World center position

    public:
        WorldMap(float radius, Vector2 center = { 0, 0 });

        void Update();
        void Draw() const;

        // Check if position is outside world bounds
        bool IsOutOfBounds(Vector2 position) const;

        [[nodiscard]] float GetRadius() const { return radius; }
        [[nodiscard]] Vector2 GetCenter() const { return center; }
    };
}

#endif // WORLD_MAP_H