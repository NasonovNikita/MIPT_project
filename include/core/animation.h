#ifndef ANIMATION_H
#define ANIMATION_H

#include "raylib.h"
#include "components.h" // For Transform2D
#include <vector>
#include <unordered_map>
#include <string>

namespace core::animation {
    class AnimationSystem {
        struct Animation {
            std::vector<Texture2D> frames;
            float frameDuration;
            int currentFrame = 0;
            float frameTime = 0;
            bool looping = true;
            bool flipX = false;
            bool flipY = false;
        };

        static std::unordered_map<std::string, Animation> animations;
        static std::vector<std::pair<std::string, components::Transform2D>> activeAnimations;

    public:
        // Load animation from sprite sheet
        static void Load(const std::string& name,
            const char* spriteSheetPath,
            const std::pair<int, int> &framesCount,
            float frameDuration,
            bool looping = true);

        // Play animation with Transform2D
        static void Play(const std::string& name, components::Transform2D transform);

        // Set flip options for an animation
        static void SetFlip(const std::string& name, bool flipX, bool flipY);

        // Update all animations
        static void Update(float deltaTime);

        // Draw all animations
        static void Draw();

        static void LoadAll();

        // Cleanup resources
        static void UnloadAll();



        // Check if animation is playing
        static bool IsPlaying(const std::string& name);

        static int GetActiveCount() { return static_cast<int>(activeAnimations.size()); }
    };
}

#endif