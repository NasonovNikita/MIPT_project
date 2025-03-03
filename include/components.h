//
// Created by nosokvkokose on 24.02.25.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <raylib.h>
#include <raymath.h>
#include <utility>
#include <vector>

namespace components {
    /// Info about position and sizes
    struct Transform2D final {
        Vector2 center;
        Vector2 size;
        Vector2 scale {1, 1};

        /// Angle in degrees from 0 to 360
        float angle = 0;

        [[nodiscard]] Vector2 scaledSize() const { return size * scale; }

        [[nodiscard]] Vector2 corner() const { return center - scaledSize() / 2; }

        Transform2D(float x, float y, float width, float height, float angle=0);
        Transform2D(Vector2 center, Vector2 size, float angle=0);

        explicit operator Rectangle() const {
            return {corner().x, corner().y, scaledSize().x, scaledSize().y};
        }
    };


    struct Collider {
        friend Vector2 EPA(Collider& colliderA, Collider& colliderB, const std::vector<Vector2>& simplex);
    protected:
        bool checkCollision(Collider &other, std::vector<Vector2>& simplex);

        virtual Vector2 supportPoint(Vector2 direction) = 0;

        virtual Rectangle getCoveringBox() = 0;
        virtual Rectangle getInnerBox() = 0;
    public:
        bool checkCollision(Collider &other);
        virtual ~Collider() = default;
        Vector2 getCollisionNormal(Collider &other);

        virtual void setCenter(Vector2 center) = 0;
    };

    struct ColliderRect final : Collider {
    private:
        Rectangle rect;
    public:
        explicit ColliderRect(const Rectangle rect): rect(rect) {}
        ColliderRect(const Vector2 corner, const Vector2 size):
        rect(corner.x, corner.y, size.x, size.y) {}

        // Set center (used by collider as pivot)
        void setCenter(Vector2 center) override;

        Vector2 supportPoint(Vector2 direction) override;

        Rectangle getCoveringBox() override { return rect; };
        Rectangle getInnerBox() override { return rect; };
    };

    struct ColliderCircle final : Collider {
    private:
        float radius_;
    public:
        Vector2 center;

        explicit ColliderCircle(const Transform2D &tr);
        ColliderCircle(const Vector2 center, const float radius):
        radius_(radius), center(center) {}

        void setRadius(float radius);
        void setRadius(const Transform2D &tr);

        void setCenter(const Vector2 center) override { this->center = center; }

        Vector2 supportPoint(Vector2 direction) override;

        Rectangle getCoveringBox() override;
        Rectangle getInnerBox() override;
    };

    struct ColliderPoly final : Collider {
    private:
        std::vector<Vector2> vertices_;
        Vector2 center_;
    public:
        ColliderPoly(const Vector2 center, std::vector<Vector2> vertices):
        vertices_(std::move(vertices)), center_{center} {}
        void setCenter(Vector2 center) override;

        Vector2 supportPoint(Vector2 direction) override;

        Rectangle getCoveringBox() override;
        Rectangle getInnerBox() override;
    };

    class DrawnObject {
        /// Zero is drawn first. Others are drawn above by order
        int worldLayer_ = 0;
    public:
        virtual ~DrawnObject() = default;
        virtual void draw() = 0;
    };
}

#endif //COMPONENTS_H
