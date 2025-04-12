#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include <vector>
#include <unordered_map>
#include <string>

namespace core::animation {
    class AnimationSystem {
    private:
        struct Animation {
            std::vector<Texture2D> frames;
            float frameDuration;
            int currentFrame = 0;
            float frameTime = 0;
            bool looping = true;
        };

        static std::unordered_map<std::string, Animation> animations;
        static std::vector<std::pair<std::string, Rectangle>> activeAnimations;

    public:
        // Load animation from sprite sheet
        static void Load(const std::string& name,
            const char* spriteSheetPath,
            int frameWidth, int frameHeight,
            int framesCount,
            float frameDuration,
            bool looping = true);

        // Play animation at position
        static void Play(const std::string& name, Rectangle position);

        // Update all animations
        static void Update(float deltaTime);

        // Draw all animations
        static void Draw();

        // Cleanup resources
        static void UnloadAll();
    };
}

#endif