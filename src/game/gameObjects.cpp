//
// Created by nosokvkokose on 21.02.25.
//

#include "game/gameObjects.h"

namespace game {
    Vector2 Transform2D::corner() {
    }

    Transform2D::operator Rectangle() const {
    }

    Collider::Collider(Shape shape, Vector2 *points) {
    }

    Collider::Collider(Rectangle rect) {
    }

    Collider::~Collider() {
    }

    bool Collider::CheckCollision(Collider &other) {
    }

    Vector2 Collider::GetCollisionNormal(Collider &other) {
    }

    GameObject::GameObject() {
    }

    GameObject::~GameObject() {
    }

    bool GameObject::IsActive() const {
    }

    GameObject * GameObject::GetParent() const {
    }

    Transform2D GameObject::GetTransform() const {
    }

    void GameObject::Destroy() {
    }

    void GameObject::Instantiate(GameObject gameObject) {
    }

    void GameObject::Draw() {
    }

    bool GameObject::operator==(const GameObject &) const {
    }

    bool GameObject::operator!=(const GameObject &) const {
    }

    Asteroid::Asteroid() {
    }

    Asteroid::~Asteroid() {
    }
} // game