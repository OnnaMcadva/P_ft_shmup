#include "Bullet.hpp"
#include "Constants.hpp"
#include <ncursesw/ncurses.h>

    Bullet::Bullet(int x_, int y_, bool player_bullet) : GameEntity(x_, y_, [player_bullet] {
        cchar_t s;
        setcchar(&s, player_bullet ? L"|" : L"*", 0, 0, nullptr);
        return s;
    }()), is_player_bullet(player_bullet) {}
    void Bullet::update([[maybe_unused]] int input) {
        if (is_player_bullet) {
            x++;
            if (x >= WIDTH) active = false;
        } else {
            x--;
            if (x < 0) active = false;
        }
    }
    bool Bullet::is_from_player() const { return is_player_bullet; }