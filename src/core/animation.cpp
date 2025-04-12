#include "core/animation.h"

namespace core::animation {
    std::unordered_map<std::string, AnimationSystem::Animation> AnimationSystem::animations;
    std::vector<std::pair<std::string, Rectangle>> AnimationSystem::activeAnimations;

    void AnimationSystem::Load(const std::string& name,
        const char* spriteSheetPath,
        int frameWidth, int frameHeight,
        int framesCount,
        float frameDuration,
        bool looping) {
        Texture2D spriteSheet = LoadTexture(spriteSheetPath);

        Animation anim;
        anim.frameDuration = frameDuration;
        anim.looping = looping;

        // Extract frames from sprite sheet
        for (int i = 0; i < framesCount; i++) {
            Image frameImage = LoadImageFromTexture(spriteSheet);
            ImageCrop(&frameImage,
                {
                (float)(i * frameWidth), 0,
                    (float)frameWidth, (float)frameHeight
            });
            anim.frames.push_back(LoadTextureFromImage(frameImage));
            UnloadImage(frameImage);
        }

        UnloadTexture(spriteSheet);
        animations[name] = anim;
    }

    void AnimationSystem::Play(const std::string& name, Rectangle position) {
        if (animations.find(name) != animations.end()) {
            animations[name].currentFrame = 0;
            animations[name].frameTime = 0;
            activeAnimations.emplace_back(name, position);
        }
    }

    void AnimationSystem::Update(float deltaTime) {
        for (auto& [name, anim] : animations) {
            anim.frameTime += deltaTime;

            if (anim.frameTime >= anim.frameDuration) {
                anim.frameTime = 0;
                anim.currentFrame++;

                if (anim.currentFrame >= anim.frames.size()) {
                    if (anim.looping) {
                        anim.currentFrame = 0;
                    }
                    else {
                        anim.currentFrame = (int)anim.frames.size() - 1;
                    }
                }
            }
        }
    }

    void AnimationSystem::Draw() {
        for (auto& [name, position] : activeAnimations) {
            Animation& anim = animations[name];
            DrawTexture(anim.frames[anim.currentFrame],
                (int)position.x, (int)position.y, WHITE);
        }
    }

    void AnimationSystem::UnloadAll() {
        for (auto& [name, anim] : animations) {
            for (auto& frame : anim.frames) {
                UnloadTexture(frame);
            }
        }
        animations.clear();
        activeAnimations.clear();
    }
}