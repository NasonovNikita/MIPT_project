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
}
#endif //TEXTUREPATHS_H
