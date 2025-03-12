#include <game/gameObjects.h>
#include <core/objectPool.h>

constexpr int screenWidth = 1040;
constexpr int screenHeight = 1040;
constexpr float deltaTimePhys = 0.002f;

using game::game_objects::Asteroid;
using core::object_pool::ObjectPool;

std::vector<components::DrawnObject*> drawnObjects;
std::vector<game::game_objects::CollidingObject*> collidingObjects;
static std::vector<Asteroid> asteroids;
static game::game_objects::Player player(components::Transform2D(100, 400, 50, 50), 100, 300, 10, 100);

void updatePhysics() {
    for (auto & asteroid : asteroids) {
        if (!asteroid.isActive()) continue;

        auto tr = asteroid.getTransform();
        if (tr.center.x + tr.scaledSize().x / 2 >= screenWidth) {
            tr.center.x = screenWidth - tr.scaledSize().x / 2;
            asteroid.bounceByNormal({-1, 0});
        }
        if (tr.center.x - tr.scaledSize().x / 2 <= 0) {
            tr.center.x = screenWidth + tr.scaledSize().x / 2;
            asteroid.bounceByNormal({1, 0});
        }
        if (tr.center.y + tr.scaledSize().y / 2 > screenHeight) {
            tr.center.y = screenHeight - tr.scaledSize().y / 2;
            asteroid.bounceByNormal({0, -1});
        }
        if (tr.center.y - tr.scaledSize().y / 2 < 0) {
            tr.center.y = screenHeight + tr.scaledSize().y / 2;
            asteroid.bounceByNormal({0, 1});
        }
        asteroid.updateCollider();
    }

    for (const auto& gameObject : game::game_objects::GameObject::s_allObjects) {
        gameObject->physUpdate(deltaTimePhys);
    }

    for (int i = 0; i < collidingObjects.size(); i++) {
        for (int j = i; j < collidingObjects.size(); j++) {
            if (!collidingObjects[i]->collider->checkCollision(*collidingObjects[j]->collider)) continue;

            collidingObjects[i]->onCollided(collidingObjects[j]);
            collidingObjects[j]->onCollided(collidingObjects[i]);
        }
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);

    asteroids.reserve(1);
    asteroids.emplace_back(
        components::Transform2D(200, 200, 50, 50),
                                     10, 50, 0);

    auto bullet = game::game_objects::Bullet(
        components::Transform2D(100, 200, 10, 10), 50);


    drawnObjects.push_back(&bullet);
    drawnObjects.push_back(&asteroids[0]);
    drawnObjects.push_back(&player);

    collidingObjects.push_back(&asteroids[0]);
    collidingObjects.push_back(&bullet);
    collidingObjects.push_back(&player);

    float DT = 0;

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
        }

        DT += GetFrameTime();
        while (DT > deltaTimePhys) {
            updatePhysics();
            DT -= deltaTimePhys;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (const auto& gameObject : game::game_objects::GameObject::s_allObjects) {
            if (!gameObject->isActive()) continue;
            gameObject->logicUpdate();
        }

        for (const auto& drawnObject : drawnObjects) {
            drawnObject->draw();
        }

        EndDrawing();
    }

    CloseWindow();

    return 0;
}
