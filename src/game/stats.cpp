//
// Created by nosokvkokose on 21.02.25.
//

#include "game/stats.h"

#include <stdexcept>

namespace game {
    Stat::Stat(const int value, const int border) {
        if (value < 0 || border < 0)
            throw std::invalid_argument("value must be non-negative");
        if (value > border)
            throw std::invalid_argument("value must be smaller than border");

        this->value = value;
        this->border = border;
    }

    Stat::Stat(const int v) {
        if (v < 0)
            throw std::invalid_argument("value must be non-negative");

        this->value = v;
        this->border = v;
    }

    void Stat::ChangeValue(const int value) {
        this->value += value;
        if (this->value > this->border) {
            this->value = this->border;
        }
        if (this->value < 0) {
            this->value = 0;
        }
    }
} // game