//
// Created by nosokvkokose on 07.05.25.
//

#ifndef TEXTUREPATHS_H
#define TEXTUREPATHS_H
#include <string>

namespace textures {
    inline std::string project_root_str(PROJECT_ROOT_PATH);
    inline std::string playerTexture = project_root_str + "/assets/textures/spaceship.png";
    inline std::string background = project_root_str + "/assets/textures/background3.png";
    inline std::string playerExplosionSheet_str = project_root_str + "/assets/textures/explosion.png";
    inline std::string asteroidExplosionSheet_str = project_root_str + "/assets/textures/asteroid-explosion-spritesheet.png";
}
#endif //TEXTUREPATHS_H
