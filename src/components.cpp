//
// Created by nosokvkokose on 24.02.25.
//

#include "components.h"

#include <functional>
#include <iostream>
#include <ostream>
#include <stdexcept>

namespace components {
    Transform2D::Transform2D(const float x, const float y, const float width,
                             const float height, const float angle):
    center({x, y}), size({width, height}), angle(angle) {}

    Transform2D::Transform2D(const Vector2 center, const Vector2 size, const float angle):
    center(center), size(size), angle(angle) {}

#pragma region Collider math

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

    bool Collider::checkCollision(Collider &other, std::vector<Vector2>& simplex) {
        if (!CheckCollisionRecs(getCoveringBox(), other.getCoveringBox()))
            return false;

        // Initial direction
        Vector2 direction = {1, 0};

        // Initial support pointcheckC
        const Vector2 support = supportPoint(direction) - other.supportPoint(
                                    Vector2Negate(direction));

        // Simplex (initially contains one point)
        simplex.push_back(support);

        // New search direction
        direction = Vector2Negate(support);

        // GJK loop
        for (int i = 0; i < 1000; i++) {
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

        return false;
    }

    bool Collider::checkCollision(Collider &other) {
        if (CheckCollisionRecs(getInnerBox(), other.getInnerBox()))
            return true;

        std::vector<Vector2> simplex;
        return checkCollision(other, simplex);
    }


    struct Face {
        Vector2 normal;
        float distance;
        size_t index;
    };

    Face getClosestFace(const std::vector<Vector2>& polytope) {
        Face closestFace{};
        closestFace.distance = std::numeric_limits<float>::max();

        for (size_t i = 0; i < polytope.size(); ++i) {
            size_t j = (i + 1) % polytope.size();
            Vector2 a = polytope[i];
            Vector2 b = polytope[j];

            const Vector2 edge = b - a;
            const Vector2 normal = Vector2Normalize(Vector2{-edge.y, edge.x});
            const float distance = Vector2DotProduct(normal, a);

            if (distance < closestFace.distance) {
                closestFace.distance = distance;
                closestFace.normal = normal;
                closestFace.index = j;
            }
        }

        return closestFace;
    }

    Vector2 EPA(Collider& colliderA, Collider& colliderB, const std::vector<Vector2>& simplex) {
        std::vector<Vector2> polytope = simplex;
        constexpr float tolerance = 0.0001f;

        while (true) {
            // Find the closest face to the origin
            const Face face = getClosestFace(polytope);

            // Get the support point in the direction of the face's normal
            Vector2 support = colliderA.supportPoint(face.normal) - colliderB.
                              supportPoint(Vector2Negate(face.normal));

            // Check if the support point is close enough to the face
            float distance = Vector2DotProduct(support, face.normal);
            if (distance - face.distance < tolerance) {
                // The closest face is the collision normal
                return face.normal;
            }

            // Add the support point to the polytope
            polytope.insert(polytope.begin() + static_cast<int>(face.index), support);
        }
    }

    Vector2 Collider::getCollisionNormal(Collider &other) {
        // Run GJK to get the initial simplex
        std::vector<Vector2> simplex;
        checkCollision(other, simplex);

        // Run EPA to find the collision normal
        return EPA(*this, other, simplex);
    }

#pragma endregion

#pragma region ColliderRect
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


    void ColliderRect::rotate(float angle) {
        throw std::bad_function_call();  // No possible rotation. Use ColliderPoly for rotating rect
    }
#pragma endregion

#pragma region ColliderCircle
    ColliderCircle::ColliderCircle(const Transform2D &tr):
    radius_(0), center_(tr.center) { // Set readius(0) for Clang-Tidy to shut up about non itialized member
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
        return center_ + Vector2Scale(normalizedDir, radius_);
    }

    Rectangle ColliderCircle::getCoveringBox() {
        return Rectangle(center_.x - radius_, center_.y - radius_,
            2 * radius_, 2 * radius_);
    }

    Rectangle ColliderCircle::getInnerBox() {
        const auto sqrt2 = static_cast<float>(sqrt(2));
        return Rectangle(center_.x - radius_ / sqrt2, center_.y - radius_ / sqrt2,
                         sqrt2 * radius_, sqrt2 * radius_);
    }

    void ColliderCircle::rotate(float angle) {
        // No need. Leave as pure virtual or not?
    }
#pragma endregion

#pragma region ColliderPoly
    Vector2 ColliderPoly::supportPoint(Vector2 direction) {
        float maxDot = -INFINITY;
        Vector2 farthestPoint {0, 0};
        for (const auto& point : getVertices()) {
            if (const float dot = Vector2DotProduct(point, direction);
                dot > maxDot) {
                maxDot = dot;
                farthestPoint = point;
                }
        }
        return farthestPoint;
    }

    Rectangle ColliderPoly::getCoveringBox() {
        float xMin = INFINITY, yMin = INFINITY, xMax = -INFINITY, yMax = -INFINITY;

        for (const auto&[x, y] : getVertices()) {
            xMax = std::max(xMax, x);
            xMin = std::min(xMin, x);
            yMax = std::max(yMax, y);
            yMin = std::min(yMin, y);
        }

        return Rectangle(xMin, yMin, xMax - xMin, yMax - yMin);
    }

    Rectangle ColliderPoly::getInnerBox() {
        if (getVertices().empty()) {
            return Rectangle{0, 0, 0, 0};
        }

        // First get the outer bounding box
        Rectangle outer = getCoveringBox();

        const float shrinkRatio = 0.25f;
        const float width = outer.width * shrinkRatio;
        const float height = outer.height * shrinkRatio;

        return Rectangle{
            outer.x + (outer.width - width) / 2.0f,  // Center horizontally
            outer.y + (outer.height - height) / 2.0f, // Center vertically
            width,
            height
        };
    }


    void ColliderPoly::rotate(const float angle) {
        // Early exits
        if (offsets_.empty() || fabsf(angle) < std::numeric_limits<float>::epsilon()) {
            return;
        }

        // Convert angle and precompute trig
        const float rad = angle;
        const float cos_r = cosf(rad);
        const float sin_r = sinf(rad);

        // Rotate each vertex
        for (auto& offset : offsets_) {
            auto point = center_ + offset;
            const float dx = point.x - center_.x;
            const float dy = point.y - center_.y;

            point.x = center_.x + (dx * cos_r - dy * sin_r);
            point.y = center_.y + (dx * sin_r + dy * cos_r);

            offset = point - center_;
        }
    }
#pragma endregion



}
