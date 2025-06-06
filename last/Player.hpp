#pragma once

#include <ncursesw/ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <string>
#include <algorithm>
#include "GameEntity.hpp"

class Player : public GameEntity {
    int lives;
public:
    Player(int x_, int y_);
    void update(int input) override;
    int get_lives() const;
    void take_damage();
};