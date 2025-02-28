//
// Created by nosokvkokose on 24.02.25.
//

#include <components.h>
#include <stdexcept>

namespace components {
    Transform2D::Transform2D(const float x, const float y, const float width,
                             const float height, const float angle):
    center({x, y}), size({width, height}), angle(angle) {}

    Transform2D::Transform2D(const Vector2 center, const Vector2 size, const float angle):
    center(center), size(size), angle(angle) {}

    // Helper function to check if the simplex contains the origin
    bool containsOrigin(std::vector<Vector2>& simplex, Vector2& direction) {
        if (simplex.size() == 2) {
            // Line segment case
            const Vector2 a = simplex[1];
            const Vector2 b = simplex[0];
            const Vector2 ab = b - a;
            const Vector2 ao = Vector2Negate(a);

            // New direction is perpendicular to AB, pointing toward the origin
            direction = Vector2{-ab.y, ab.x};
            if (Vector2DotProduct(direction, ao) < 0) {
                direction = Vector2{ab.y, -ab.x};
            }
            return false;
        } else if (simplex.size() == 3) {
            // Triangle case
            const Vector2 a = simplex[2];
            const Vector2 b = simplex[1];
            const Vector2 c = simplex[0];
            const Vector2 ab = b - a;
            const Vector2 ac = c - a;
            const Vector2 ao = Vector2Negate(a);

            // Check which edge is closest to the origin
            auto abPerp = Vector2{-ab.y, ab.x};
            auto acPerp = Vector2{ac.y, -ac.x};

            if (Vector2DotProduct(abPerp, ao) > 0) {
                // Origin is outside edge AB
                simplex.erase(simplex.begin()); // Remove point C
                direction = abPerp;
                return false;
            }
            if (Vector2DotProduct(acPerp, ao) > 0) {
                // Origin is outside edge AC
                simplex.erase(simplex.begin() + 1); // Remove point B
                direction = acPerp;
                return false;
            }

            return true;
        }
        return false;
    }

    bool Collider::checkCollision(Collider &other) {
        if (!CheckCollisionRecs(getPrimitiveBox(), other.getPrimitiveBox())) {
            return false;
        }

        // Initial direction
        Vector2 direction = {1, 0};

        // Initial support point
        const Vector2 support = supportPoint(direction) - other.supportPoint(
                                    Vector2Negate(direction));

        // Simplex (initially contains one point)
        std::vector simplex = {support};

        // New search direction
        direction = Vector2Negate(support);

        // GJK loop
        while (true) {
            // Get a new support point
            Vector2 newSupport = supportPoint(direction) - other.supportPoint(
                                     Vector2Negate(direction));

            // If the new support point does not go past the origin, no collision
            if (Vector2DotProduct(newSupport, direction) < 0) {
                return false;
            }

            // Add the new support point to the simplex
            simplex.push_back(newSupport);

            // Check if the simplex contains the origin
            if (containsOrigin(simplex, direction)) {
                return true;
            }
        }
    }

    void ColliderRect::setCenter(const Vector2 center) {
        rect.x = center.x + rect.width / 2;
        rect.y = center.y + rect.height / 2;
    }

    Vector2 ColliderRect::supportPoint(const Vector2 direction) {
        const Vector2 corners[4] = {
            {rect.x, rect.y},
            {rect.x + rect.width, rect.y},
            {rect.x, rect.y + rect.height},
            {rect.x + rect.width, rect.y + rect.height}
        };

        Vector2 result = corners[0];
        float maxDot = Vector2DotProduct(result, direction);

        for (int i = 1; i < 4; ++i) {
            if (const float dot = Vector2DotProduct(corners[i], direction);
                dot > maxDot) {
                maxDot = dot;
                result = corners[i];
            }
        }

        return result;
    }

    ColliderCircle::ColliderCircle(const Transform2D &tr):
    radius_(0), center(tr.center) { // Set readius(0) for Clang-Tidy to shut up about non itialized member
        setRadius(tr);
    }

    void ColliderCircle::setRadius(const float radius) {
        if (radius <= 0)
            throw std::invalid_argument("Radius must be positive");
        this->radius_ = radius;
    }

    void ColliderCircle::setRadius(const Transform2D &tr) {
        if (tr.size.x != tr.size.y)
            throw std::invalid_argument("Tried creating circle from not scare transform; size mismatch");
        if (tr.size.x == 0) {
            throw std::invalid_argument("Tried creating circle with zero radius");
        }
        radius_ = tr.size.x / 2;
    }

    Vector2 ColliderCircle::supportPoint(const Vector2 direction) {
        Vector2 normalizedDir = Vector2Normalize(direction);
        return center + Vector2Scale(normalizedDir, radius_);
    }

    Rectangle ColliderCircle::getPrimitiveBox() {
        return Rectangle(center.x - radius_, center.y - radius_,
            2 * radius_, 2 * radius_);
    }

    void ColliderPoly::setCenter(const Vector2 center) {
        for (auto vertex : vertices_) {
            vertex += center - center_;
        }

        center_ = center;
    }

    Vector2 ColliderPoly::supportPoint(Vector2 direction) {
        float maxDot = -INFINITY;
        Vector2 farthestPoint {0, 0};
        for (const auto& point : vertices_) {
            if (const float dot = Vector2DotProduct(point, direction);
                dot > maxDot) {
                maxDot = dot;
                farthestPoint = point;
                }
        }
        return farthestPoint;
    }

    Rectangle ColliderPoly::getPrimitiveBox() {
        float xMin = INFINITY,
        yMin = INFINITY, xMax = 0, yMax = 0;

        for (const auto&[x, y] : vertices_) {
            xMax = std::max(xMax, x);
            xMin = std::min(xMin, x);
            yMax = std::max(yMax, y);
            yMin = std::min(yMin, y);
        }

        return Rectangle(xMin, yMin, xMax - xMin, yMax - yMin);
    }
}
