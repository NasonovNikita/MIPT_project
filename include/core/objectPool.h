//
// Created by nosokvkokose on 12.03.25.
//

#ifndef OBJECTPOOL_H
#define OBJECTPOOL_H
#include <memory>
#include <queue>

namespace core::object_pool {
    template<typename T>
    class ObjectPool {
        std::queue<std::shared_ptr<T>> availableObjects;

    public:
        explicit ObjectPool() = default;

        [[nodiscard]] bool isEmpty() const { return availableObjects.empty(); }

        std::shared_ptr<T> acquire() {
            if (availableObjects.empty()) {
                return nullptr;
            }

            std::shared_ptr<T> obj = std::move(availableObjects.front());
            availableObjects.pop();
            return obj;
        }

        void release(std::shared_ptr<T> obj) {
            availableObjects.push(std::move(obj));
        }
    };
}

#endif //OBJECTPOOL_H
