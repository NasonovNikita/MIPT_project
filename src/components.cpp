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

    void ColliderRect::setCenter(const Vector2 center) {
        rect.x = center.x + rect.width / 2;
        rect.y = center.y + rect.height / 2;
    }

    bool ColliderRect::checkCollision(Collider &other) {
        // TODO checkCollision_rect by maks
        // Maybe can be googled?
    }

    Vector2 ColliderRect::getCollisionNormal(Collider &other) {
        // TODO get collision normal by anyone
        // Idk how to do that on this stage,
        // there must be universal algorithm for all shapes...
    }

    bool ColliderCircle::checkCollision(Collider &other) {
        // TODO checkCollision_circle by Maks
        // Must be easy
    }

    Vector2 ColliderCircle::getCollisionNormal(Collider &other) {
        /*
            I feel like there mustn't be getCollisionNormal() for each class
        It must be that there is two types of collision: side to side, point to side or else
        For example side to side = normal to side
        Point to side = normal from the side
        However for circle it's vector from center to point...
        What a pain in the ass
            Maybe another mother-class layer? With finite amount of points and
        infinite (circle, ellipse, other)?
        */
    }

    ColliderCircle::ColliderCircle(const Transform2D &tr): radius_(0), center(tr.center) { // Set readius(0) for Clang-Tidy to shut up about non itialized member
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

    bool ColliderPoly::checkCollision(Collider &other) {
    }

    void ColliderPoly::setCenter(const Vector2 center) {
        for (auto vertex : vertices_) {
            vertex += center - center_;
        }

        center_ = center;
    }

    Vector2 ColliderPoly::getCollisionNormal(Collider &other) {
    }
}
