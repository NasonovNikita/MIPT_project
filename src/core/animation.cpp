#include "core/animation.h"

#include <algorithm>


namespace core::animation {
    std::unordered_map<std::string, AnimationSystem::Animation> AnimationSystem::animations;
    std::vector<std::pair<std::string, components::Transform2D>> AnimationSystem::activeAnimations;

    void AnimationSystem::Load(const std::string& name,
        const char* spriteSheetPath,
        const std::pair<int, int> &framesCount,
        const float frameDuration,
        const bool looping) {
        const Texture2D spriteSheet = LoadTexture(spriteSheetPath);

        const int frameWidth = spriteSheet.width / framesCount.second;
        const int frameHeight = spriteSheet.height / framesCount.first;

        Animation anim;
        anim.frameDuration = frameDuration;
        anim.looping = looping;


        // Extract frames from sprite sheet
        for (int i = 0; i < framesCount.first; i++) {
            for (int j = 0; j < framesCount.second; j++) {
                Image frameImage = LoadImageFromTexture(spriteSheet);
                ImageCrop(&frameImage,
                    {
                    static_cast<float>(j * frameWidth), static_cast<float>(i * frameHeight),
                        static_cast<float>(frameWidth), static_cast<float>(frameHeight)
                    });
                anim.frames.push_back(LoadTextureFromImage(frameImage));
                UnloadImage(frameImage);
            }
        }

        UnloadTexture(spriteSheet);
        animations[name] = anim;
    }

    void AnimationSystem::Play(const std::string& name, components::Transform2D transform) {
        if (animations.contains(name)) {
            animations[name].currentFrame = 0;
            animations[name].frameTime = 0;
            activeAnimations.emplace_back(name, transform);
        }
    }

    void AnimationSystem::SetFlip(const std::string& name, bool flipX, bool flipY) {
        if (animations.contains(name)) {
            animations[name].flipX = flipX;
            animations[name].flipY = flipY;
        }
    }

    void AnimationSystem::Update(float deltaTime) {
        // Update animation frames
        for (auto& [name, anim] : animations) {
            if (anim.frames.empty()) continue;

            anim.frameTime += deltaTime;

            if (anim.frameTime >= anim.frameDuration) {
                anim.frameTime = 0;
                anim.currentFrame++;

                if (anim.currentFrame >= anim.frames.size()) {
                    if (anim.looping) {
                        anim.currentFrame = 0;
                    }
                    else {
                        // Remove non-looping animations that finished
                        std::erase_if(activeAnimations,
                                      [&](const auto& item) {
                                          return item.first == name;
                                      });
                        anim.currentFrame = static_cast<int>(anim.frames.size()) - 1;
                    }
                }
            }
        }
    }

void AnimationSystem::Draw() {
    // Use indices for safe removal
    for (size_t i = 0; i < activeAnimations.size(); ) {
        auto& [name, transform] = activeAnimations[i];
        
        if (!animations.contains(name)) {
            activeAnimations.erase(activeAnimations.begin() + i);
            continue;
        }

        Animation& anim = animations[name];
        if (anim.currentFrame >= anim.frames.size()) {
            activeAnimations.erase(activeAnimations.begin() + i);
            continue;
        }

        const Texture2D frame = anim.frames[anim.currentFrame];
        const Vector2 position = transform.center;
        const Vector2 size = transform.scaledSize();
        const float rotation = transform.angle;

        const Rectangle source = {
            0.0f, 0.0f,
            static_cast<float>(frame.width) * (anim.flipX ? -1.0f : 1.0f),
            static_cast<float>(frame.height) * (anim.flipY ? -1.0f : 1.0f)
        };

        const Rectangle dest = {
            position.x, position.y,
            size.x, size.y
        };

        Vector2 origin = { size.x / 2, size.y / 2 };

        DrawTexturePro(frame, source, dest, origin, rotation, WHITE);
        i++; // Only increment if we didn't erase

        for (auto& [name, transform] : activeAnimations) {
            if (!animations.contains(name)) continue;

            anim = animations[name];
            if (anim.currentFrame >= anim.frames.size()) continue;
        }
    }
}

    bool AnimationSystem::IsPlaying(const std::string& name) {
        return std::ranges::find_if(activeAnimations,
                                    [&](const auto& item) { return item.first == name; }) != activeAnimations.end();
    }

    void AnimationSystem::UnloadAll() {
        for (auto& [name, anim] : animations) {
            for (const auto& frame : anim.frames) {
                UnloadTexture(frame);
            }
        }
        animations.clear();
        activeAnimations.clear();
    }
}