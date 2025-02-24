//
// Created by nosokvkokose on 21.02.25.
//

#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <list>
#include <raylib.h>
#include <raymath.h>
#include <unordered_set>
#include <vector>
#include <game/stats.h>


namespace game::game_objects {
    /// Info about position and sizes
    struct Transform2D final {
        /// Centertr
        Vector2 position;
        Vector2 size;
        Vector2 scale;

        /// Angle in degrees from 0 to 360
        float angle;

        [[nodiscard]] Vector2 scaledSize() const { return size * scale; }

        [[nodiscard]] Vector2 corner() const { return position - scaledSize() / 2; }

        explicit operator Rectangle() const {
            return {corner().x, corner().y, scaledSize().x, scaledSize().y};
        }
    };


    struct Collider {
        virtual ~Collider() = 0;

        virtual bool checkCollision(Collider &other) = 0;

        /// Vector showing direction of force for the collision
        virtual Vector2 getCollisionNormal(Collider &other) = 0;
    };

    struct ColliderRect final : Collider {
        // ReSharper disable once CppDFANotInitializedField
        Rectangle rect_;

        bool checkCollision(Collider &other) override;

        Vector2 getCollisionNormal(Collider &other) override;

        explicit ColliderRect(const Rectangle rect): rect_(rect) {}
        ColliderRect(const Vector2 corner, const Vector2 size):
        rect_(corner.x, corner.y, size.x, size.y) {}

        ~ColliderRect() override = default;
    };

    class DrawnObject {
        public:
        virtual ~DrawnObject() = default;
        virtual void Draw() = 0;
    };


    class GameObject {
        friend int GenerateId();
        static std::unordered_set<int> s_existing_ids;
        static std::list<GameObject*> s_allObjects;

    protected:
        int id_;
        Transform2D transform_;
        /// Zero is drawn first. Others are drawn above by order
        int worldLayer_ = 0;
        bool isActive_ = true;

        GameObject *parent_ = nullptr;
        std::vector<GameObject> children_ = {};

        explicit GameObject(const Transform2D &tr);
        GameObject(const GameObject& other);

    public:
        virtual ~GameObject();

        [[nodiscard]] bool isActive() const { return isActive_; }

        [[nodiscard]] GameObject *getParent() const { return parent_; }

        [[nodiscard]] Transform2D getTransform() const { return transform_; }

        void setActive(const bool active) { isActive_ = active; }

        void setParent(GameObject *parent) { parent_ = parent; }

        /// Copy an instance of an object
        virtual GameObject* instantiate(GameObject *gameObject);

        /// Is invoked every frame
        virtual void update() {
        }

        /// Is invoked on start of object's life
        virtual void start() {
        }

        bool operator==(const GameObject &other) const { return id_ == other.id_; }

        bool operator!=(const GameObject &other) const { return id_ != other.id_; }

        /// IsActive
        explicit operator bool() const { return isActive_; }
    };


    class CollidingObject : GameObject {
    protected:
        explicit CollidingObject(const Transform2D &tr):
        GameObject(tr), collider(new ColliderRect(Rectangle(transform_))) {}

    public:
        Collider *collider;
    };

    class Unit : CollidingObject, DrawnObject {
        stats::Stat hp_;
    protected:
        float maxSpeed_;
        float currentSpeed_ = 0;

        float acceleration_;
    public:
        explicit Unit(const Transform2D &tr, const stats::Stat hp,
            const float maxSpeed, const float acceleration):
        CollidingObject(tr), hp_(hp), maxSpeed_(maxSpeed), acceleration_(acceleration) {}
    };


    class Asteroid final : Unit {
    public:
        Asteroid(const Transform2D &tr, const stats::Stat hp, const float maxSpeed):
        Unit(tr, hp, maxSpeed, 0) { currentSpeed_ = maxSpeed; }

        ~Asteroid() override;

        void Draw() override;
    };


    class Player final : Unit {
        float maxRotationSpeed_;
        float currentRotationSpeed_ = 0;
        float rotationAcceleration_;
    public:
        static Player *s_instance;

        static Player *getInstance() { return s_instance; }

        explicit Player(const Transform2D &tr, const stats::Stat hp, const float maxSpeed,
                        const float acceleration, const float maxRotationSpeed,
                        const float rotationAcceleration):
        Unit(tr, hp, maxSpeed, acceleration),
        maxRotationSpeed_(maxRotationSpeed), rotationAcceleration_(rotationAcceleration) {}

        void Draw() override;
    };
} // game

#endif //GAMEOBJECTS_H
