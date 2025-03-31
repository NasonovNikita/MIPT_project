//
// Created by nosokvkokose on 21.02.25.
//

#include <algorithm>
#include <iostream>
#include <ranges>
#include <stdexcept>
#include <game/gameObjects.h>
#include <game/objectManager.h>


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

    void Unit::die() {
        setActive(false);
    }

    void Unit::takeDamage(const int value) {
        hp_.ChangeValue(-value);

        if (hp_.getValue() == 0) {
            die();
        }
    }

    void Asteroid::draw() {
        if (!isActive()) return;

        DrawCircle(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, GRAY);
        DrawCircleLines(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, BLACK);
    }

    void Asteroid::onCollided(CollidingObject *other) {
        if (other == this) return;
        const auto otherAsteroid = dynamic_cast<Asteroid*>(other);

        if (otherAsteroid) {
            const Vector2 collisionNormal = collider->getCollisionNormal(*other->collider);
            bounceFromOther(*otherAsteroid, collisionNormal);

            resolveCollision(*other);
        }

        if (other == Player::getInstance()) {
            takeDamage(5);
        }
    }

    std::vector<Vector2> Player::getVertices() {
        return vertices;
    }


    Player *Player::s_instance;

    void Player::draw() {
        DrawTriangle(vertices[0], vertices[1], vertices[2], GREEN);

        DrawTriangleLines(vertices[0], vertices[1], vertices[2], BLUE);
    }

    void Player::physUpdate(float deltaTime) {
        Unit::physUpdate(deltaTime);

        // if (!(IsKeyDown(KEY_UP) or IsKeyDown(KEY_DOWN))) {
        //     if (Vector2Length(currentSpeed_) > 0) {
        //         currentSpeed_ -= accelerationDirection * acceleration_ * deltaTime;
        //     }
        // }

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
            // accelerationDirection = Vector2Negate(Vector2Normalize(currentSpeed_));
            acceleration_ = 0;
        }

        if (IsKeyDown(KEY_UP)) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = 500;
        }

        if (IsKeyDown(KEY_DOWN)) {
            accelerationDirection = Vector2Normalize(vertices[2] - transform_.center);
            acceleration_ = -500;
        };

        if (IsKeyDown(KEY_RIGHT)) {
            rotationAcceleration_ = 20;
        }

        if (IsKeyDown(KEY_LEFT)) {
            rotationAcceleration_ = -20;
        }

        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            management::ObjectManager::getInstance().createObject<Bullet>(
        components::Transform2D(vertices[2].x, vertices[2].y, 10, 10), 300, angle_);
        }
    }

    void Bullet::draw() {
        if (!isActive()) return;

        DrawCircle(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, RED);
        DrawCircleLines(static_cast<int>(transform_.center.x),
                   static_cast<int>(transform_.center.y),
                   transform_.scaledSize().x / 2, YELLOW);
    }

    void Bullet::onCollided(CollidingObject *other) {
        const auto asteroid = dynamic_cast<Asteroid*>(other);

        if (!asteroid) return;
        asteroid->takeDamage(10);
        setActive(false);
    }
} // game
