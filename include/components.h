//
// Created by nosokvkokose on 24.02.25.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <raylib.h>
#include <raymath.h>

namespace components {
    /// Info about position and sizes
    struct Transform2D final {
        Vector2 center;
        Vector2 size;
        Vector2 scale;

        /// Angle in degrees from 0 to 360
        float angle;

        [[nodiscard]] Vector2 scaledSize() const { return size * scale; }

        [[nodiscard]] Vector2 corner() const { return center - scaledSize() / 2; }

        explicit operator Rectangle() const {
            return {corner().x, corner().y, scaledSize().x, scaledSize().y};
        }
    };


    struct Collider {
        virtual ~Collider() = default;

        virtual bool checkCollision(Collider &other) = 0;

        /// Vector showing direction of force for the collision
        virtual Vector2 getCollisionNormal(Collider &other) = 0;
    };

    struct ColliderRect final : Collider {
        // ReSharper disable once CppDFANotInitializedField
        Rectangle rect;

        bool checkCollision(Collider &other) override;

        Vector2 getCollisionNormal(Collider &other) override;

        explicit ColliderRect(const Rectangle rect): rect(rect) {}
        ColliderRect(const Vector2 corner, const Vector2 size):
        rect(corner.x, corner.y, size.x, size.y) {}
    };

    struct ColliderCircle final : Collider {
    private:
        float radius;
    public:
        Vector2 center;

        bool checkCollision(Collider &other) override;

        Vector2 getCollisionNormal(Collider &other) override;

        explicit ColliderCircle(const Transform2D &tr);
        ColliderCircle(const Vector2 center, const float radius):
        radius(radius), center(center) {}

        void setRadius(float radius);
        void setRadius(const Transform2D &tr);
    };

    class DrawnObject {
    public:
        virtual ~DrawnObject() = default;
        virtual void draw() = 0;
    };
}
#endif //COMPONENTS_H
