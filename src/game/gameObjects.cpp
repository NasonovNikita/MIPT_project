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
                                  currentSpeed_, Vector2Normalize(normal));

        currentSpeed_ -= mirrored * 2;
    }

    void Unit::bounceFromOther(Unit &other) {
        const auto collisionNormal =
            collider->getCollisionNormal(*other.collider);

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


    void Unit::physUpdate(const float deltaTime) {
        currentSpeed_ = currentSpeed_ + accelerationDirection * acceleration_ * deltaTime;
        if (Vector2Length(currentSpeed_) > maxSpeed_) {
            currentSpeed_ = Vector2Normalize(currentSpeed_) * maxSpeed_;
        }

        transform_.center += currentSpeed_ * deltaTime;

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

    std::vector<Vector2> Player::getVertices() {
        return vertices;
    }

    Player::Player(const components::Transform2D& tr, const int hp, const float maxSpeed,
        const float maxRotationSpeed, const float currentSpeed = -1) : Unit(tr, hp, maxSpeed),
        maxRotationSpeed_(maxRotationSpeed) {

        if (abs(currentSpeed + 1) < 0.01f)
            currentSpeed_ = Vector2Normalize(currentSpeed_) * maxSpeed;
        else currentSpeed_ = Vector2Normalize(currentSpeed_) * currentSpeed;

        vertices = { tr.corner(), tr.corner() + Vector2 {0, tr.scaledSize().y}, tr.center + Vector2 {tr.scaledSize().x / 2, 0}};
        collider = new components::ColliderPoly(tr.center, vertices);
        
    };

    void Player::draw(){

        DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

        DrawTriangleLines(vertices[0], vertices[1], vertices[2], BLUE);


        // TODO Player::Draw by Maks
        // A narrow triangle pointing to current
        // pointing position (rotation in transform)
    }

    void Player::physUpdate(float deltaTime) {
        Unit::physUpdate(deltaTime);

        if (!(IsKeyDown(KEY_UP) or IsKeyDown(KEY_DOWN))) {
            if (Vector2Length(currentSpeed_) > 0) {
                currentSpeed_ -= accelerationDirection * acceleration_ * deltaTime;
            }
        }

        vertices[0] += currentSpeed_ * deltaTime;
        vertices[1] += currentSpeed_ * deltaTime;
        vertices[2] += currentSpeed_ * deltaTime;

        if (IsKeyDown(KEY_RIGHT) or IsKeyDown(KEY_LEFT)) {
            currentRotationSpeed_ += rotationAcceleration_ * deltaTime;
            if (currentRotationSpeed_ > maxRotationSpeed_) {
                currentRotationSpeed_ = maxRotationSpeed_;
            }
            if (-currentRotationSpeed_ > maxRotationSpeed_) {
                currentRotationSpeed_ = -maxRotationSpeed_;
            }
        }

        else {
            if (std::abs(currentRotationSpeed_) - std::abs(rotationAcceleration_) * deltaTime > 0) {
                if (currentRotationSpeed_ * rotationAcceleration_ > 0) {
                    currentRotationSpeed_ -= rotationAcceleration_ * deltaTime;
                }
                else {
                    currentRotationSpeed_ += rotationAcceleration_ * deltaTime;
                }
            }
            else {
                currentRotationSpeed_ = 0;
            }
        }


        auto dAngle_ = currentRotationSpeed_ * deltaTime;

        angle_ += dAngle_;
        if (angle_ > 180) angle_ -= 180;
        if (angle_ < -180) angle_ += 180;

        transform_.angle = angle_;

        vertices = { transform_.center + Vector2Rotate(vertices[0] - transform_.center, dAngle_),
                         transform_.center + Vector2Rotate(vertices[1] - transform_.center, dAngle_),
                         transform_.center + Vector2Rotate(vertices[2] - transform_.center, dAngle_) };



        collider->rotate(dAngle_);


    }

    void Player::logicUpdate() {

        if (!(IsKeyDown(KEY_UP) or IsKeyDown(KEY_DOWN))) {
            accelerationDirection = Vector2Negate(Vector2Normalize(currentSpeed_));
        }

        if (IsKeyDown(KEY_UP)) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = 100;
        }

        if (IsKeyDown(KEY_DOWN)) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = -100;
        };
        
        if (IsKeyDown(KEY_RIGHT)) {
            rotationAcceleration_ = 10;
        }

        if (IsKeyDown(KEY_LEFT)) {
            rotationAcceleration_ = -10;
        }

        // TODO controls
    }
} // game
