#include "core/animation.h"


namespace core::animation {
    std::unordered_map<std::string, AnimationSystem::Animation> AnimationSystem::animations;
    std::vector<std::pair<std::string, components::Transform2D>> AnimationSystem::activeAnimations;

    void AnimationSystem::Load(const std::string& name,
        const char* spriteSheetPath,
        std::pair<int, int> framesCount,
        float frameDuration,
        bool looping) {
        Texture2D spriteSheet = LoadTexture(spriteSheetPath);

        int frameWidth = spriteSheet.width / framesCount.second;
        int frameHeight = spriteSheet.height / framesCount.first;

        Animation anim;
        anim.frameDuration = frameDuration;
        anim.looping = looping;


        // Extract frames from sprite sheet
        for (int i = 0; i < framesCount.first; i++) {
            for (int j = 0; j < framesCount.second; j++) {
                Image frameImage = LoadImageFromTexture(spriteSheet);
                ImageCrop(&frameImage,
                    {
                    (float)(j * frameWidth), (float)(i * frameHeight),
                        (float)frameWidth, (float)frameHeight
                    });
                anim.frames.push_back(LoadTextureFromImage(frameImage));
                UnloadImage(frameImage);
            }
        }

        UnloadTexture(spriteSheet);
        animations[name] = anim;
    }

    void AnimationSystem::Play(const std::string& name, components::Transform2D transform) {
        if (animations.find(name) != animations.end()) {
            animations[name].currentFrame = 0;
            animations[name].frameTime = 0;
            activeAnimations.emplace_back(name, transform);
        }
    }

    void AnimationSystem::SetFlip(const std::string& name, bool flipX, bool flipY) {
        if (animations.find(name) != animations.end()) {
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
                        activeAnimations.erase(
                            std::remove_if(activeAnimations.begin(), activeAnimations.end(),
                                [&](const auto& item) {
                                    return item.first == name;
                                }),
                            activeAnimations.end());
                        anim.currentFrame = (int)anim.frames.size() - 1;
                    }
                }
            }
        }
    }

void AnimationSystem::Draw() {
    // Use indices for safe removal
    for (size_t i = 0; i < activeAnimations.size(); ) {
        auto& [name, transform] = activeAnimations[i];
        
        if (animations.find(name) == animations.end()) {
            activeAnimations.erase(activeAnimations.begin() + i);
            continue;
        }

        Animation& anim = animations[name];
        if (anim.currentFrame >= anim.frames.size()) {
            activeAnimations.erase(activeAnimations.begin() + i);
            continue;
        }

        Texture2D frame = anim.frames[anim.currentFrame];
        Vector2 position = transform.center;
        Vector2 size = transform.scaledSize();
        float rotation = transform.angle;

        Rectangle source = {
            0.0f, 0.0f,
            (float)frame.width * (anim.flipX ? -1.0f : 1.0f),
            (float)frame.height * (anim.flipY ? -1.0f : 1.0f)
        };

        Rectangle dest = {
            position.x, position.y,
            size.x, size.y
        };

        Vector2 origin = { size.x / 2, size.y / 2 };

        DrawTexturePro(frame, source, dest, origin, rotation, WHITE);
        i++; // Only increment if we didn't erase

        for (auto& [name, transform] : activeAnimations) {
            if (animations.find(name) == animations.end()) continue;

            Animation& anim = animations[name];
            if (anim.currentFrame >= anim.frames.size()) continue;
        }
    }
}

    bool AnimationSystem::IsPlaying(const std::string& name) {
        return std::find_if(activeAnimations.begin(), activeAnimations.end(),
            [&](const auto& item) { return item.first == name; }) != activeAnimations.end();
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