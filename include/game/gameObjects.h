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

    protected:
        int id_;
        components::Transform2D transform_;
        bool isActive_ = true;

        GameObject *parent_ = nullptr;
        std::vector<GameObject> children_ = {};

        explicit GameObject(const components::Transform2D &tr);
    public:
        static std::list<GameObject*> s_allObjects;

        GameObject(const GameObject& other);
        virtual ~GameObject();

        [[nodiscard]] bool isActive() const { return isActive_; }

        [[nodiscard]] GameObject *getParent() const { return parent_; }

        [[nodiscard]] components::Transform2D& getTransform() { return transform_; }

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

    class Unit : public CollidingObject, public DrawnGameObject, public MovingObject {
        stats::Stat hp_ {0};
    protected:

        void die();
    public:

        explicit Unit(const int hp, const float maxSpeed):
        MovingObject(maxSpeed), hp_(hp) {}

        void takeDamage(int value);

        void forceDie() { die(); }

        void physUpdate(const float deltaTime) override {
            MovingObject::physUpdate(deltaTime);
            CollidingObject::physUpdate(deltaTime);
        }
    };


    class Asteroid final : public Unit {
    public:
        Asteroid(const components::Transform2D &tr, const int hp, const float maxSpeed, const float currentSpeed=-1):
        GameObject(tr), Unit(hp, maxSpeed) {
            if (abs(currentSpeed + 1) < 0.01f)
                currentSpeed_ = Vector2Normalize(currentSpeed_) * maxSpeed;
            else currentSpeed_ = Vector2Normalize(currentSpeed_) * currentSpeed;
            
            collider = new components::ColliderCircle(tr);
        }

        void setCenter(const float x, const float y) {
            transform_.center = Vector2(x, y);
            updateCollider();
        }

        void setDirectionOfSpeed(Vector2 const sp) {
            currentSpeed_ = sp * Vector2Length(currentSpeed_);
        }

        void draw() override;

        void onCollided(CollidingObject *other) override;
    };


    class Player final : public Unit {
        float angle_ = 0;
        float maxRotationSpeed_;
        float currentRotationSpeed_ = 0;
        float rotationAcceleration_ = 0;
        std::vector<Vector2> vertices = { Vector2 (0, 0), Vector2 (0, 0), Vector2 (0, 0)};
        static Player *s_instance;

        explicit Player(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float maxRotationSpeed):
        GameObject(tr), Unit(hp, maxSpeed),
        maxRotationSpeed_(maxRotationSpeed) {
            vertices = { tr.corner(), tr.corner() + Vector2 {0, tr.scaledSize().y}, tr.center + Vector2 {tr.scaledSize().x / 2, 0}};
            collider = new components::ColliderPoly(tr.center, vertices);
        }

    public:
        static Player *getInstance() { return s_instance; }

        static Player* SpawnPlayer(const components::Transform2D &tr, const int hp,
            const float maxSpeed, const float maxRotationSpeed) {
            s_instance = new Player(tr, hp, maxSpeed, maxRotationSpeed);
            return s_instance;
        }

        std::vector<Vector2> getVertices();

        void draw() override;
        void physUpdate(float deltaTime) override;
        void logicUpdate() override;
    };

    class Bullet final : public CollidingObject, public MovingObject, public DrawnGameObject {
    public:
        Bullet(const components::Transform2D &tr, const float maxSpeed, const float angle = 0):
        GameObject(tr), CollidingObject(new components::ColliderCircle(tr)),
        MovingObject(maxSpeed) {
            currentSpeed_ = {maxSpeed * cos(angle), maxSpeed * sin(angle)};
        }

        void draw() override;
        void physUpdate(const float deltaTime) override {
            CollidingObject::physUpdate(deltaTime);
            MovingObject::physUpdate(deltaTime);
        }
        void onCollided(CollidingObject *other) override;
    };
} // game

#endif //GAMEOBJECTS_H
