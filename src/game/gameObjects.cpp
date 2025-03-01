//
// Created by nosokvkokose on 21.02.25.
//

#include <algorithm>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <game/gameObjects.h>


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
    transform_(other.transform_),
    worldLayer_(other.worldLayer_) {}

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

    void Unit::die() {
        setActive(false);
    }

    void Unit::takeDamage(const int value) {
        hp_.ChangeValue(-value);

        if (hp_.getValue() == 0) {
            die();
        }
    }

    void Unit::forceDie() {
        die();
    }

    void Unit::bounceByNormal(const Vector2 normal) {
        const auto mirrored = Vector2Normalize(normal) * Vector2DotProduct(
                                  movingDirection, Vector2Normalize(normal));

        movingDirection -= mirrored * 2;
        movingDirection = Vector2Normalize(movingDirection); // For safety if precision is low
    }

    void Unit::bounceFromOther(Unit &other) {
        const auto collisionNormal =
            collider->getCollisionNormal(*other.collider);

        const Vector2 relativeSpeed = movingDirection * currentSpeed_ - other.
                                      movingDirection * other.currentSpeed_;

        const Vector2 newSpeed = movingDirection * currentSpeed_ - collisionNormal *
                                 Vector2DotProduct(collisionNormal, relativeSpeed);
        movingDirection = Vector2Normalize(newSpeed);
        currentSpeed_ = (newSpeed / movingDirection).x;   // x == y

        const Vector2 otherNewSpeed = other.movingDirection * other.currentSpeed_ -
                                      collisionNormal * Vector2DotProduct(
                                          collisionNormal,
                                          Vector2Negate(relativeSpeed));
        other.movingDirection = Vector2Normalize(otherNewSpeed);
        other.currentSpeed_ = (otherNewSpeed / other.movingDirection).x;
    }


    void Unit::physUpdate(const float deltaTime) {
        currentSpeed_ += acceleration_ * deltaTime;
        if (currentSpeed_ > maxSpeed_) {
            currentSpeed_ = maxSpeed_;
        }

        transform_.center += movingDirection * currentSpeed_ * deltaTime;

        updateCollider();
    }


    void Asteroid::draw() {
        DrawCircle(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, GRAY);
        DrawCircleLines(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, BLACK);
    }

    Player *Player::s_instance;

    Player::Player(const components::Transform2D &tr, const int hp, const float maxSpeed,
        const float maxRotationSpeed): Unit(tr, hp, maxSpeed),
    maxRotationSpeed_(maxRotationSpeed) {}

    void Player::draw() {
        // TODO Player::Draw by Maks
        // A narrow triangle pointing to current
        // pointing position (rotation in transform)
    }

    void Player::physUpdate(float deltaTime) {
        Unit::physUpdate(deltaTime);

        currentRotationSpeed_ += acceleration_ * deltaTime;
        if (currentRotationSpeed_ > maxRotationSpeed_) {
            currentRotationSpeed_ = maxRotationSpeed_;
        }
        if (-currentRotationSpeed_ > maxRotationSpeed_) {
            currentRotationSpeed_ = -maxRotationSpeed_;
        }

        angle_ += currentRotationSpeed_ * deltaTime;
        if (angle_ > 180) angle_ -= 180;
        if (angle_ < -180) angle_ += 180;

        transform_.angle = angle_;
    }

    void Player::logicUpdate() {
        // TODO controls
    }
} // game