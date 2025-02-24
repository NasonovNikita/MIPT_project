//
// Created by nosokvkokose on 21.02.25.
//

#include <algorithm>
#include <ranges>
#include <stdexcept>
#include <unordered_map>
#include <game/gameObjects.h>


namespace game::game_objects {
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

    void Unit::die() {
        setActive(false);
        delete this;
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

    void Asteroid::draw() {
        // TODO Asteroid::Draw by Maks
        // Just a circle will be fine for now
        // Hint: size of transform can be used as a diameter
    }

    void Player::draw() {
        // TODO Player::Draw by Maks
        // A narrow triangle pointing to current
        // pointing position (rotation in transform)
    }

    Player *Player::s_instance;
} // game