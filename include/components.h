//
// Created by nosokvkokose on 24.02.25.
//

#ifndef COMPONENTS_H
#define COMPONENTS_H
#include <raylib.h>
#include <stdexcept>
#include <vector>

#include "raymath.h"

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

    inline Transform2D Transform2DZero() {
        return {0, 0, 0, 0};
    }


    struct Collider {
        friend Vector2 EPA(Collider &colliderA, Collider &colliderB,
                           const std::vector<Vector2> &simplex);
    protected:
        /// Used in case you need simplex after check
        bool checkCollision(Collider &other, std::vector<Vector2>& simplex);

        /// <seealso href="https://en.wikipedia.org/wiki/Gilbert%E2%80%93Johnson%E2%80%93Keerthi_distance_algorithm"/>
        /// GJK algorithm support point
        virtual Vector2 supportPoint(Vector2 direction) = 0;

        /// Box containing collider
        virtual Rectangle getCoveringBox() = 0;
        /// Box fully contained by collider
        virtual Rectangle getInnerBox() = 0;
    public:
        bool checkCollision(Collider &other);
        virtual ~Collider() = default;
        Vector2 getCollisionNormal(Collider &other);

        /// Set center (used by collider as pivot)
        virtual void setCenter(Vector2 center) = 0;

        virtual void rotate(float angle) = 0;
    };

    struct ColliderRect final : Collider {
    private:
        Rectangle rect;
    public:
        explicit ColliderRect(const Rectangle rect): rect(rect) {}
        ColliderRect(const Vector2 corner, const Vector2 size):
        rect(corner.x, corner.y, size.x, size.y) {}

        void setCenter(Vector2 center) override;

        Vector2 supportPoint(Vector2 direction) override;

        Rectangle getCoveringBox() override { return rect; };
        Rectangle getInnerBox() override { return rect; }

        void rotate(float angle) override;;
    };

    struct ColliderCircle final : Collider {
    private:
        float radius_;
        Vector2 center_;
    public:

        explicit ColliderCircle(const Transform2D &tr);
        ColliderCircle(const Vector2 center, const float radius):
        radius_(radius), center_(center) {}

        void setRadius(float radius);
        void setRadius(const Transform2D &tr);

        void setCenter(const Vector2 center) override { this->center_ = center; }

        Vector2 supportPoint(Vector2 direction) override;

        Rectangle getCoveringBox() override;
        Rectangle getInnerBox() override;

        void rotate(float angle) override;
    };

    struct ColliderPoly final : Collider {
    private:
        std::vector<Vector2> offsets_;
        Vector2 center_;
    public:
        ColliderPoly(const Vector2 center, const std::vector<Vector2>& vertices):
        center_{center} {
            if (vertices.empty())
                throw std::invalid_argument("Empty vertices array");

            for (auto &vertex: vertices) {
                offsets_.push_back(vertex - center);
            }
        }
        void setCenter(Vector2 center) override { center_ = center; }

        Vector2 supportPoint(Vector2 direction) override;

        Rectangle getCoveringBox() override;
        Rectangle getInnerBox() override;

        void rotate(float angle) override;

        std::vector<Vector2> getVertices() {
            std::vector<Vector2> vertices;
            for (const auto& offset : offsets_) {
                vertices.push_back(center_ + offset);
            }
            return vertices;
        }

        [[nodiscard]] Vector2 getCenter() const { return center_; }
    };

    class DrawnObject {
        /// Zero is drawn first. Others are drawn above by order
        int worldLayer_ = 0;
    public:
        virtual ~DrawnObject() = default;
        virtual void draw() = 0;
    };

    struct GameCamera {
        Camera2D camera;
        Vector2 targetOffset;
        float smoothSpeed;
        float zoom;

        GameCamera() :
            camera({ 0 }),
            targetOffset({ 0, 0 }),
            smoothSpeed(5.0f),
            zoom(1.0f)
        {
            camera.zoom = zoom;
        }
    };

    class TextureComponent {
    private:
        Texture2D texture;
        Color tint;
        Rectangle sourceRect;

    public:
        TextureComponent(const char* path, Color tint = WHITE);
        ~TextureComponent();


        Texture2D getTexture() const;
        void Draw(const Transform2D& transform) const;
        void Draw(const Transform2D& transform, float angle ) const;
        void SetTint(Color newTint) { tint = newTint; }
    };
}

#endif //COMPONENTS_H
