//
// Created by nosokvkokose on 21.02.25.
//

#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H
#include <list>
#include <unordered_set>

#include "components.h"


namespace game::game_objects {
    class GameObject {
        friend int generateId();
        static std::unordered_set<int> s_existing_ids;

    protected:
        int id_;
        components::Transform2D transform_;
        bool toDestroy_ = false;
        bool isActive_ = true;

        GameObject *parent_ = nullptr;
        std::vector<GameObject> children_ = {};

        explicit GameObject(const components::Transform2D &tr);
    public:
        static std::list<GameObject*> s_allObjects;

        GameObject(const GameObject& other);
        virtual ~GameObject();

        [[nodiscard]] bool isActive() const { return isActive_; }
        [[nodiscard]] bool isToDestroy() const { return toDestroy_; }

        [[nodiscard]] GameObject *getParent() const { return parent_; }

        [[nodiscard]] components::Transform2D& getTransform() { return transform_; }

        void setActive(const bool active) { isActive_ = active; }
        void destroy() {
            setActive(false);
            toDestroy_ = true;
        }

        void setParent(GameObject *parent) { parent_ = parent; }

        /// Copy an instance of an object
        virtual GameObject* instantiate(GameObject *gameObject);

        /// Is invoked every deltaTimePhys or similar (physics)
        virtual void physUpdate(float deltaTime) {
        }

        /// Called once per frame
        virtual void logicUpdate() {
        }

        /// Is invoked on start of object's life
        virtual void start() {
        }

        bool operator==(const GameObject &other) const { return id_ == other.id_; }

        bool operator!=(const GameObject &other) const { return id_ != other.id_; }

        /// IsActive
        explicit operator bool() const { return isActive_; }
    };


    class CollidingObject : public virtual GameObject {
    protected:
        explicit CollidingObject(components::Collider *collider=nullptr):
        collider(collider) {
            if (collider == nullptr)
                this->collider = new components::ColliderRect(Rectangle());
        }
    public:
        components::Collider *collider;
        ~CollidingObject() override = 0;

        void updateCollider() const { collider->setCenter(transform_.center); }
        void resolveCollision(CollidingObject &other);

        void physUpdate(float deltaTime) override;

        void virtual onCollided(CollidingObject *other) {};
    };

    class MovingObject : public virtual GameObject {
    protected:
        float maxSpeed_ = 0;
        Vector2 currentSpeed_ = { 1, 0 };

        float acceleration_ = 0;

    public:
        Vector2 movingDirection {1, 0};
        Vector2 accelerationDirection = { 0, 0 };
        ~MovingObject() override = 0;

        explicit MovingObject(const float maxSpeed):
        maxSpeed_(maxSpeed) {}

        [[nodiscard]] float getSpeedModule() const {
            return sqrt(currentSpeed_.x * currentSpeed_.x + currentSpeed_.y * currentSpeed_.y);
        }

        /// Change direction of movement as if it bounced from surface with given normal
        void bounceByNormal(Vector2 normal);

        void bounceFromOther(MovingObject& other, Vector2 collisionNormal);

        void physUpdate(float deltaTime) override;
    };

    class DrawnGameObject: public components::DrawnObject, public virtual GameObject {
    public:
        DrawnGameObject() {}
    };
} // game

#endif //GAMEOBJECTS_H
