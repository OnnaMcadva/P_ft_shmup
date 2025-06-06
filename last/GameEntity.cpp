#include "GameEntity.hpp"
#include "Constants.hpp"
#include <ncursesw/ncurses.h>

    GameEntity::GameEntity(int x_, int y_, const cchar_t& s) : x(x_), y(y_), symbol(s), active(true) {}
    void GameEntity::update([[maybe_unused]] int input) {}
    void GameEntity::render() const {
        if (active) {
            mvadd_wch(y, x, &symbol);
        }
    }
    bool GameEntity::is_active() const { return active; }
    int GameEntity::get_x() const { return x; }
    int GameEntity::get_y() const { return y; }
    void GameEntity::deactivate() { active = false; }