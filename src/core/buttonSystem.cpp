#include "core/buttonSystem.h"
#include <utility>

namespace core::button {
    std::unordered_map<std::string, Button> ButtonSystem::buttons;

    //Button::Button(Button&& other) noexcept :
    //    frames(std::exchange(other.frames, {})),
    //    bounds(other.bounds),
    //    btnState(other.btnState),
    //    onClick(std::move(other.onClick)) {
    //}

    //// Реализация оператора перемещения
    //Button& Button::operator=(Button&& other) noexcept {
    //    if (this != &other) {
    //        // Освобождаем текущие ресурсы
    //        for (const auto& frame : frames) {
    //            UnloadTexture(frame);
    //        }

    //        // Перемещаем ресурсы
    //        frames = std::exchange(other.frames, {});
    //        bounds = other.bounds;
    //        btnState = other.btnState;
    //        onClick = std::move(other.onClick);
    //    }
    //    return *this;
    //}

    //Button::Button(const char* texturePath,
    //    const std::pair<int, int>& framesCount,
    //    const Rectangle& bounds,
    //    std::function<void()> onClick):
    //    bounds(bounds),
    //    onClick(std::move(onClick)) {

    //    const Texture2D spriteSheet = LoadTexture(texturePath);

    //    const int frameWidth = spriteSheet.width / framesCount.second;
    //    const int frameHeight = spriteSheet.height / framesCount.first;

    //    for (int i = 0; i < framesCount.first; i++) {
    //        for (int j = 0; j < framesCount.second; j++) {
    //            Image frameImage = LoadImageFromTexture(spriteSheet);
    //            ImageCrop(&frameImage,
    //                {
    //                static_cast<float>(j * frameWidth), static_cast<float>(i * frameHeight),
    //                    static_cast<float>(frameWidth), static_cast<float>(frameHeight)
    //                });
    //            frames.push_back(LoadTextureFromImage(frameImage));
    //            UnloadImage(frameImage);
    //        }
    //    }

    //    UnloadTexture(spriteSheet);
    //}

/*    Button::~Button() {
        for (const auto& frame : frames) {
            UnloadTexture(frame);
        }
    } */ 


    void ButtonSystem::Load(const std::string& name,
        const char* texturePath,
        const std::pair<int, int>& framesCount,
        const Rectangle& bounds,
        std::function<void()> onClick,
        bool is_Invisible) {

        Button btn;

        const Texture2D spriteSheet = LoadTexture(texturePath);

        const int frameWidth = spriteSheet.width / framesCount.second;
        const int frameHeight = spriteSheet.height / framesCount.first;

        for (int i = 0; i < framesCount.first; i++) {
            for (int j = 0; j < framesCount.second; j++) {
                Image frameImage = LoadImageFromTexture(spriteSheet);
                ImageCrop(&frameImage,
                    {
                    static_cast<float>(j * frameWidth), static_cast<float>(i * frameHeight),
                        static_cast<float>(frameWidth), static_cast<float>(frameHeight)
                    });
                btn.frames.push_back(LoadTextureFromImage(frameImage));
                UnloadImage(frameImage);
            }
        }
        btn.bounds = bounds;
        btn.onClick = onClick;
        btn.is_Invisible = is_Invisible;

        
        UnloadTexture(spriteSheet);
        buttons[name] = std::move(btn);
    }

    void ButtonSystem::Update() {
        Vector2 mousePoint = GetMousePosition();

        for (auto& [name, button] : buttons) {
            if (button.is_Invisible) continue;
            button.btnState = 0; // Reset to normal

            if (CheckCollisionPointRec(mousePoint, button.bounds)) {
                if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
                    button.btnState = 2; // Pressed
                }
                else {
                    button.btnState = 1; // Hover
                }

                if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
                    button.onClick();
                }
            }

        }
    }

    void ButtonSystem::Draw() {
        for (const auto& [name, button] : buttons) {
            if (button.is_Invisible) continue;
            const Texture2D frame = button.frames[button.btnState];

            Rectangle sourceRec = {
                0.0f,
                0.0f,
                (float)frame.width,
                (float)frame.height
            };
            DrawTextureRec(frame, sourceRec,
                { button.bounds.x, button.bounds.y }, WHITE);
        }
    }

    bool ButtonSystem::IsButtonPressed(const std::string& name) {
        if (buttons.find(name) != buttons.end()) {
            return buttons[name].btnState == 2;
        }
        return false;
    }

    void ButtonSystem::UnloadAll() {
        for (auto& [name, button] : buttons) {
            for (const auto& frame : button.frames) {
                UnloadTexture(frame);
            }
        }

        buttons.clear();
    }

    void ButtonSystem::setInvisibility(const std::string& name, bool is_Invisible) {
        buttons[name].is_Invisible = is_Invisible;
    }

    void ButtonSystem::setBounds(const std::string& name, Rectangle bounds) {
        buttons[name].bounds = bounds;
    }
}
