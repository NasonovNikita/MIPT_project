#include <game/gameObjects.h>


int main() {
    constexpr int screenWidth = 800;
    constexpr int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "test");

    SetTargetFPS(60);

    std::vector<game::game_objects::Asteroid> asteroids;
    asteroids.reserve(10);

    for (int i = 0; i < 10; i++) {
        asteroids.emplace_back(
            components::Transform2D(GetRandomValue(100, 700),
                                    GetRandomValue(100, 350), 30, 30,
                                    GetRandomValue(0, 360)), 20,
            GetRandomValue(-200, 200));
    }

    while (!WindowShouldClose()) {
        if (IsKeyPressed(KEY_ESCAPE)) {
            CloseWindow();
        }

        BeginDrawing();

        for (auto & asteroid : asteroids) {
            asteroid.update();
            asteroid.draw();

            const auto tr = asteroid.getTransform();
            if (tr.center.x > screenWidth) {
                asteroid.setCenter(0, tr.center.y);
            }
            if (tr.center.x < 0) {
                asteroid.setCenter(screenWidth, tr.center.y);
            }
            if (tr.center.y > screenHeight) {
                asteroid.setCenter(tr.center.x, 0);
            }
            if (tr.center.y < 0) {
                asteroid.setCenter(tr.center.x, screenHeight);
            }
        }
        ClearBackground(RAYWHITE);


        EndDrawing();
    }

    CloseWindow();

    return 0;
}
