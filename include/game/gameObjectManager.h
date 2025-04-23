//
// Created by nosokvkokose on 29.03.25.
//

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <algorithm>
#include <vector>
#include <memory>

#include "gameObjects.h"

namespace game::management {
    class GameObjectManager {
        GameObjectManager() = default;
        ~GameObjectManager() = default;

        void registerInterfaces(game_objects::GameObject* obj) {
            if (const auto drawn = dynamic_cast<game_objects::DrawnGameObject*>(obj)) {
                drawnObjects_.push_back(drawn);
            }
            if (const auto colliding = dynamic_cast<game_objects::CollidingObject*>(obj)) {
                collidingObjects_.push_back(colliding);
            }
        }

        std::vector<std::shared_ptr<game_objects::GameObject>> ownedObjects_;
        std::vector<game_objects::DrawnGameObject*> drawnObjects_;
        std::vector<game_objects::CollidingObject*> collidingObjects_;
    public:
        // Singleton access
        static GameObjectManager& getInstance() {
            static GameObjectManager instance;
            return instance;
        }

        // Object creation
        template<typename T, typename... Args>
        std::shared_ptr<T> createObject(Args&&... args) {
            static_assert(std::is_base_of_v<game_objects::GameObject, T>,
                          "T must inherit from GameObject");

            std::shared_ptr<T> obj = std::make_shared<T>(std::forward<Args>(args)...);
            registerInterfaces(obj.get());
            ownedObjects_.push_back(obj);
            obj.get()->start();
            return obj;
        }

        // Object registration (for externally created objects like Player)
        void registerExternalObject(game_objects::GameObject* obj) {
            registerInterfaces(obj);
        }

        // Accessors
        [[nodiscard]] static const std::list<game_objects::GameObject*>& getAllObjects() {
            return game_objects::GameObject::s_allObjects;
        }

        [[nodiscard]] const std::vector<game_objects::DrawnGameObject*>& getDrawnObjects() const {
            return drawnObjects_;
        }

        [[nodiscard]] const std::vector<game_objects::CollidingObject*>& getCollidingObjects() const {
            return collidingObjects_;
        }

        // Cleanup
        void destroyAll() {
            ownedObjects_.clear(); // Automatically removes from s_allObjects via GameObject destructor
            drawnObjects_.clear();
            collidingObjects_.clear();
        }

        void destroyObjectsToDestroy() {
            // Phase 1: Mark inactive objects
            std::vector<game_objects::GameObject*> toRemove;
            for (auto& obj : ownedObjects_) {
                if (!obj->isToDestroy()) {
                    toRemove.push_back(obj.get());
                }
            }

            // Phase 2: Remove from interface lists first
            auto removeFromVector = [&](auto& vec) {
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [&](auto ptr) {
                        return std::find(toRemove.begin(), toRemove.end(), ptr) != toRemove.end();
                    }),
                    vec.end()
                );
            };

            removeFromVector(drawnObjects_);
            removeFromVector(collidingObjects_);

            // Phase 3: Finally erase owned objects (will auto-remove from s_allObjects via destructor)
            std::erase_if(ownedObjects_,
                          [&](const auto& obj) { return !obj->isActive(); });
        }

        // Prevent copying
        GameObjectManager(const GameObjectManager&) = delete;
        void operator=(const GameObjectManager&) = delete;
    };
}

#endif //OBJECTMANAGER_H
