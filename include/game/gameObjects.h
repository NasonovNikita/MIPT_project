//
// Created by nosokvkokose on 21.02.25.
//

#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <list>
#include <raylib.h>
#include <unordered_set>
#include <vector>
#include <game/stats.h>
#include <components.h>


namespace game::game_objects {
    class GameObject {
        friend int generateId();
        static std::unordered_set<int> s_existing_ids;
        static std::list<GameObject*> s_allObjects;

    protected:
        int id_;
        components::Transform2D transform_;
        /// Zero is drawn first. Others are drawn above by order
        int worldLayer_ = 0;
        bool isActive_ = true;

        GameObject *parent_ = nullptr;
        std::vector<GameObject> children_ = {};

        explicit GameObject(const components::Transform2D &tr);
    public:
        GameObject(const GameObject& other);
        virtual ~GameObject();

        [[nodiscard]] bool isActive() const { return isActive_; }

        [[nodiscard]] GameObject *getParent() const { return parent_; }

        [[nodiscard]] components::Transform2D getTransform() const { return transform_; }

        void setActive(const bool active) { isActive_ = active; }

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


    class CollidingObject : public GameObject {
    protected:
        explicit CollidingObject(const components::Transform2D &tr, components::ColliderRect *collider=nullptr):
        GameObject(tr), collider(collider) {
            if (collider == nullptr)
                this->collider = new components::ColliderRect(Rectangle(tr));
        }
    public:
        components::Collider *collider;

        void updateCollider() { collider->setCenter(transform_.center); }
        void resolveCollision(CollidingObject &other);
    };


    class Unit : public CollidingObject, public components::DrawnObject {
        stats::Stat hp_ {0};
    protected:
        float maxSpeed_ = 0;
        float currentSpeed_ = 0;

        float acceleration_ = 0;


        void die();
    public:
        Vector2 movingDirection {1, 0};

        explicit Unit(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float acceleration):
        CollidingObject(tr), hp_(hp), maxSpeed_(maxSpeed), acceleration_(acceleration) {}

        void takeDamage(int value);

        void forceDie();

        /// Change direction of movement as if it bounced from surface with given normal
        void bounceByNormal(Vector2 normal);

        void bounceFromOther(Unit& other);

        void physUpdate(float deltaTime) override;
    };


    class Asteroid final : public Unit {
    public:
        Asteroid(const components::Transform2D &tr, const int hp, const float maxSpeed, const float currentSpeed=-1):
        Unit(tr, hp, maxSpeed, 0) {
            if (abs(currentSpeed + 1) < 0.01f)
                currentSpeed_ = maxSpeed;
            else currentSpeed_ = currentSpeed;
            
            collider = new components::ColliderCircle(tr);
        }

        void setCenter(const float x, const float y) {
            transform_.center = Vector2(x, y);
            collider->setCenter(Vector2(x, y));
        }

        void draw() override;
    };


    class Player final : public Unit {
        float maxRotationSpeed_;
        float currentRotationSpeed_ = 0;
        float rotationAcceleration_;
    public:
        static Player *s_instance;

        static Player *getInstance() { return s_instance; }

        explicit Player(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float acceleration,
            const float maxRotationSpeed,
            const float rotationAcceleration):
        Unit(tr, hp, maxSpeed, acceleration),
        maxRotationSpeed_(maxRotationSpeed), rotationAcceleration_(rotationAcceleration) {
            s_instance = this;
        }

        void draw() override;
        void logicUpdate() override;
    };
} // game

#endif //GAMEOBJECTS_H
