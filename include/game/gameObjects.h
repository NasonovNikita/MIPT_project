//
// Created by nosokvkokose on 21.02.25.
//

#ifndef GAMEOBJECTS_H
#define GAMEOBJECTS_H

#include <raylib.h>
#include <game/shapes.h>

#include "stats.h"

namespace game {
    /// Info about position and sizes
    struct Transform2D final {
        Vector2 position;
        Vector2 size;
        Vector2 scale;

        /// Angle in degrees from 0 to 360
        float angle;

        Vector2 corner();

        explicit operator Rectangle() const;
    };

    struct Collider final {
        Shape shape;
        /// Points required to define the shape. E.g. 2 points for rect,
        /// n points for poly
        Vector2 *points;

        Collider(Shape shape, Vector2 *points);
        explicit Collider(Rectangle rect);
        ~Collider();

        bool CheckCollision(Collider &other);
        /// Vector showing direction of force for the collision
        Vector2 GetCollisionNormal(Collider &other);
    };

    class GameObject {
        static GameObject *allObjects;

    protected:
        long id;
        Transform2D transform;
        /// Zero is drawn first. Others are drawn above by order
        int worldLayer;
        bool isActive;

        GameObject* parent;

        GameObject();
    public:
        virtual ~GameObject();

        [[nodiscard]] bool IsActive() const;
        [[nodiscard]] GameObject* GetParent() const;
        [[nodiscard]] Transform2D GetTransform() const;

        void SetActive(const bool active) {
            isActive = active;
            // Possible further event handlings
        }

        void SetParent(GameObject *parent) { this->parent = parent; }

        void Destroy();

        /// Copy an instance of an object
        void Instantiate(GameObject gameObject);
        /// Is invoked every frame
        virtual void Update() {}
        /// Is invoked on start of object's life
        virtual void Start() {}
        virtual void Draw();

        bool operator==(const GameObject &) const;
        bool operator!=(const GameObject &) const;

        /// IsActive
        explicit operator bool() const { return isActive; }
    };

    class CollidingObject : GameObject {
    protected:
        explicit CollidingObject(): collider(Rectangle(transform)) {}
    public:
        Collider collider;
    };

    class Unit : CollidingObject {
        Stat hp;
    };

    class Asteroid final : Unit {
    public:
        Asteroid();
        ~Asteroid() override;
    };

    class Player final : Unit {
    public:
        static Player* instance;

        static Player* GetInstance() { return instance; }
    };
} // game

#endif //GAMEOBJECTS_H
