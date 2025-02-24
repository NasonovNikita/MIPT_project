//
// Created by nosokvkokose on 21.02.25.
//

#ifndef STATS_H
#define STATS_H

namespace game::stats {
    class Stat {
        int value;
        int border;
    public:
        Stat(): value(0), border(0) {}
        Stat(int value, int border);
        explicit Stat(int v);

        [[nodiscard]] int getValue() const { return value; }
        void ChangeValue(int value);
    };
} // game

#endif //STATS_H
