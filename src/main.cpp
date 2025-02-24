#include <game/gameObjects.h>


int main() {
    auto player = game::game_objects::Player(
        components::Transform2D(), game::stats::Stat(10),
        0, 0, 0, 0);

    player.takeDamage(5);
    // I fought my life with linker for it to see all implementations
    return 0;
}
