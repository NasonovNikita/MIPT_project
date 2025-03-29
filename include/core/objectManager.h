//
// Created by nosokvkokose on 29.03.25.
//

#ifndef OBJECTMANAGER_H
#define OBJECTMANAGER_H

#include <algorithm>
#include <vector>
#include <memory>
#include <game/gameObjects.h>
#include <components.h>

namespace core::management {
    class ObjectManager {
    public:
        // Singleton access
        static ObjectManager& Instance() {
            static ObjectManager instance;
            return instance;
        }

        // Object creation
        template<typename T, typename... Args>
        T* CreateObject(Args&&... args) {
            static_assert(std::is_base_of_v<game::game_objects::GameObject, T>,
                          "T must inherit from GameObject");

            auto obj = std::make_unique<T>(std::forward<Args>(args)...);
            T* rawPtr = obj.get();
            RegisterInterfaces(rawPtr);
            ownedObjects_.push_back(std::move(obj));
            return rawPtr;
        }

        // Object registration (for externally created objects like Player)
        void RegisterExternalObject(game::game_objects::GameObject* obj) {
            RegisterInterfaces(obj);
        }

        // Accessors
        [[nodiscard]] const std::list<game::game_objects::GameObject*>& GetAllObjects() const {
            return game::game_objects::GameObject::s_allObjects;
        }

        [[nodiscard]] const std::vector<game::game_objects::DrawnGameObject*>& GetDrawnObjects() const {
            return drawnObjects_;
        }

        [[nodiscard]] const std::vector<game::game_objects::CollidingObject*>& GetCollidingObjects() const {
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
            std::vector<game::game_objects::GameObject*> toRemove;
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

    private:
        ObjectManager() = default;
        ~ObjectManager() = default;

        void RegisterInterfaces(game::game_objects::GameObject* obj) {
            if (auto drawn = dynamic_cast<game::game_objects::DrawnGameObject*>(obj)) {
                drawnObjects_.push_back(drawn);
            }
            if (auto colliding = dynamic_cast<game::game_objects::CollidingObject*>(obj)) {
                collidingObjects_.push_back(colliding);
            }
        }

        std::vector<std::unique_ptr<game::game_objects::GameObject>> ownedObjects_;
        std::vector<game::game_objects::DrawnGameObject*> drawnObjects_;
        std::vector<game::game_objects::CollidingObject*> collidingObjects_;
    };
}

#endif //OBJECTMANAGER_H
