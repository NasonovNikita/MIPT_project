#include <game/gameObjects.h>

static constexpr int screenWidth = 800;
static constexpr int screenHeight = 800;
constexpr float deltaTimePhys = 0.002f;

std::vector<components::DrawnObject*> drawnObjects;
std::vector<game::game_objects::CollidingObject*> collidingObjects;
static std::vector<game::game_objects::Asteroid> asteroids;

void updatePhysics() {
    for (auto & asteroid : asteroids) {
        if (!asteroid.isActive()) continue;

        asteroid.physUpdate(deltaTimePhys);

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

    for (int i = 0; i < collidingObjects.size(); i++) {
        for (int j = i; j < collidingObjects.size(); j++) {
            if (!collidingObjects[i]->collider->checkCollision(*collidingObjects[j]->collider)) continue;

            collidingObjects[i]->onCollided(*collidingObjects[j]);
        }
    }
}

int main() {
    InitWindow(screenWidth, screenHeight, "test");
    SetTargetFPS(60);

    asteroids.reserve(18);
    for (int i = 0; i < 18; i++) {
        asteroids.emplace_back(
            components::Transform2D(40.f * (i + 1), 40.f * (i + 1), 50, 50), 20,
            10000, GetRandomValue(100, 300));
        drawnObjects.push_back(&asteroids[i]);
        collidingObjects.push_back(&asteroids[i]);

        const auto angle = GetRandomValue(0, 360);
        asteroids[i].movingDirection = Vector2(static_cast<float>(cos(angle)),
                                               static_cast<float>(sin(angle)));
    }

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
