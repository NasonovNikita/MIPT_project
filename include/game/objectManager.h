//
// Created by nosokvkokose on 29.03.25.
//

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <algorithm>
#include <vector>
#include <memory>
#include <game/gameObjects.h>

namespace game::management {
    class ObjectManager {
        ObjectManager() = default;
        ~ObjectManager() = default;

        void RegisterInterfaces(game_objects::GameObject* obj) {
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
        static ObjectManager& Instance() {
            static ObjectManager instance;
            return instance;
        }

        // Object creation
        template<typename T, typename... Args>
        std::shared_ptr<T> CreateObject(Args&&... args) {
            static_assert(std::is_base_of_v<game_objects::GameObject, T>,
                          "T must inherit from GameObject");

            auto obj = std::make_shared<T>(std::forward<Args>(args)...);
            RegisterInterfaces(obj.get());
            ownedObjects_.push_back(obj);
            return obj;
        }

        // Object registration (for externally created objects like Player)
        void RegisterExternalObject(game_objects::GameObject* obj) {
            RegisterInterfaces(obj);
        }

        // Accessors
        [[nodiscard]] static const std::list<game_objects::GameObject*>& GetAllObjects() {
            return game_objects::GameObject::s_allObjects;
        }

        [[nodiscard]] const std::vector<game_objects::DrawnGameObject*>& GetDrawnObjects() const {
            return drawnObjects_;
        }

        [[nodiscard]] const std::vector<game_objects::CollidingObject*>& GetCollidingObjects() const {
            return collidingObjects_;
        }

        // Cleanup
        void DestroyAll() {
            ownedObjects_.clear(); // Automatically removes from s_allObjects via GameObject destructor
            drawnObjects_.clear();
            collidingObjects_.clear();
        }

        void DestroyInactive() {
            // Phase 1: Mark inactive objects
            std::vector<game_objects::GameObject*> toRemove;
            for (auto& obj : ownedObjects_) {
                if (!obj->isActive()) {
                    toRemove.push_back(obj.get());
                }
            }

            // Phase 2: Remove from interface lists first
            auto RemoveFromVector = [&](auto& vec) {
                vec.erase(std::remove_if(vec.begin(), vec.end(),
                    [&](auto ptr) {
                        return std::find(toRemove.begin(), toRemove.end(), ptr) != toRemove.end();
                    }),
                    vec.end()
                );
            };

            RemoveFromVector(drawnObjects_);
            RemoveFromVector(collidingObjects_);

            // Phase 3: Finally erase owned objects (will auto-remove from s_allObjects via destructor)
            std::erase_if(ownedObjects_,
                          [&](const auto& obj) { return !obj->isActive(); });
        }

        // Prevent copying
        ObjectManager(const ObjectManager&) = delete;
        void operator=(const ObjectManager&) = delete;
    };
}

#endif //OBJECTMANAGER_H
