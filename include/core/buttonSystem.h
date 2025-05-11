// buttonSystem.h
#pragma once

#include "raylib.h"
#include "components.h"
#include <unordered_map>
#include <functional>
#include <memory>

namespace core::button {
    struct Button {
        std::vector<Texture2D> frames;
        Rectangle bounds;
        int btnState = 0; // Button state: 0-NORMAL, 1-MOUSE_HOVER, 2-PRESSED
        std::function<void()> onClick = []() {};
        bool is_Invisible = true;

        //Button(const Button&) = delete;
        //Button& operator=(const Button&) = delete;

        //// Разрешаем только перемещение
        //Button(Button&& other) noexcept;
        //Button& operator=(Button&& other) noexcept;


        //Button(const char* texturePath,
        //    const std::pair<int, int>& framesCount,
        //    const Rectangle& bounds,
        //    std::function<void()> onClick);

        //~Button();

        //const int getButtonState() {
        //    return btnState;
        //}

        //void setButtonState(int state) {
        //    btnState = state;
        //}

        //const Rectangle getBounds() {
        //    return bounds;
        //}

        //void setBounds(Rectangle b) {
        //    bounds = b;
        //}

    };

	class ButtonSystem {
        static  std::unordered_map<std::string, Button> buttons;
	public:
        static void Load(const std::string& name,
            const char* texturePath,
            const std::pair<int, int>& framesCount,
            const Rectangle& bounds,
            std::function<void()> onClick,
            bool is_Invisible = true);

        static void Update();
        static void Draw(const components::Transform2D& relative);
        static void UnloadAll();
        static void setInvisibility(const std::string& name, bool is_Invisible);
        static void setBounds(const std::string& name, Rectangle bounds);

        static bool IsButtonPressed(const std::string& name);

	};
}
