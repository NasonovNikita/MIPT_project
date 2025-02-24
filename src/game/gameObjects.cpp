//
// Created by nosokvkokose on 21.02.25.
//

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <game/gameObjects.h>


namespace game::game_objects {
    int GenerateId() {
        int id = GetRandomValue(0, std::numeric_limits<int>::max());
        while (GameObject::s_existing_ids.contains(id))
            id = GetRandomValue(0, std::numeric_limits<int>::max());

        return id;
    }


    GameObject::GameObject(const Transform2D &tr): transform_(tr) {
        if (s_existing_ids.size() >= std::numeric_limits<int>::max() - 1)
            throw std::overflow_error("Too many gameobjects");

        id_ = GenerateId();

        s_existing_ids.insert(id_);
        s_allObjects.push_back(this);
    }

    GameObject::GameObject(const GameObject& other):
    id_(GenerateId()),
    transform_(other.transform_),
    worldLayer_(other.worldLayer_),
    children_(other.children_) {}

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

    bool ColliderRect::checkCollision(Collider &other) {
        // TODO by maks
    }

    Vector2 ColliderRect::getCollisionNormal(Collider &other) {
        // TODO by maks
    }

    void Asteroid::Draw() {

    }

    void Player::Draw() {

    }

} // game