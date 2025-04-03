//
// Created by nosokvkokose on 21.02.25.
//

#include "game/gameObjects.h"


#include "game/entities/units.h"


namespace game::game_objects {
#pragma region GameObject
    int generateId() {
        int id = GetRandomValue(0, std::numeric_limits<int>::max());
        while (GameObject::s_existing_ids.contains(id))
            id = GetRandomValue(0, std::numeric_limits<int>::max());

        return id;
    }

    std::unordered_set<int> GameObject::s_existing_ids;
    std::list<GameObject*> GameObject::s_allObjects;

    GameObject::GameObject(const components::Transform2D &tr): transform_(tr) {
        if (s_existing_ids.size() >= std::numeric_limits<int>::max() - 1)
            throw std::overflow_error("Too many gameobjects");

        id_ = generateId();

        s_existing_ids.insert(id_);
        s_allObjects.push_back(this);
    }

    GameObject::GameObject(const GameObject& other):
    id_(generateId()),
    transform_(other.transform_) {}

    GameObject::~GameObject() {
        s_existing_ids.erase(id_);
        s_allObjects.remove(this);
    }

    GameObject* GameObject::instantiate(GameObject *gameObject) {
        const auto copiedGameObject = new GameObject(*gameObject);
        copiedGameObject->id_ = GetRandomValue(0, std::numeric_limits<int>::max());
        while (s_existing_ids.contains(copiedGameObject->id_))
            copiedGameObject->id_ = GetRandomValue(0, std::numeric_limits<int>::max());

        return copiedGameObject;
    }
#pragma endregion

    CollidingObject::~CollidingObject() {
        GameObject::~GameObject();
        delete collider;
    }

    void CollidingObject::resolveCollision(CollidingObject &other) {
        const auto collisionNormal = collider->
                getCollisionNormal(*other.collider);
        while (collider->checkCollision(*other.collider)) {
            transform_.center -= collisionNormal * 0.1;
            other.transform_.center += collisionNormal * 0.1;

            updateCollider();
            other.updateCollider();
        }
    }

    void CollidingObject::physUpdate(float deltaTime) {
        if (!isActive_) return;

        updateCollider();
    }


    MovingObject::~MovingObject() { GameObject::~GameObject(); }

    void MovingObject::bounceByNormal(const Vector2 normal) {
        const auto mirrored = Vector2Normalize(normal) * Vector2DotProduct(
                                  currentSpeed_, Vector2Normalize(normal));

        currentSpeed_ -= mirrored * 2;
    }

    void MovingObject::bounceFromOther(MovingObject &other, const Vector2 collisionNormal) {

        const Vector2 relativeSpeed = currentSpeed_ - other.currentSpeed_;

        const Vector2 newSpeed = currentSpeed_ - collisionNormal *
                                 Vector2DotProduct(collisionNormal, relativeSpeed);

        currentSpeed_ = newSpeed;  // x == y

        const Vector2 otherNewSpeed = other.currentSpeed_ -
                                      collisionNormal * Vector2DotProduct(
                                          collisionNormal,
                                          Vector2Negate(relativeSpeed));
        other.currentSpeed_ = otherNewSpeed;
    }

    void MovingObject::physUpdate(const float deltaTime) {
        if (!isActive_) return;

        currentSpeed_ = currentSpeed_ + accelerationDirection * acceleration_ * deltaTime;
        if (Vector2Length(currentSpeed_) > maxSpeed_) {
            currentSpeed_ = Vector2Normalize(currentSpeed_) * maxSpeed_;
        }

        transform_.center += currentSpeed_ * deltaTime;
    }
} // game
