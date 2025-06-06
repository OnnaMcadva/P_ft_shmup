#include "Enemy.hpp"
#include "Constants.hpp"
#include <cstdlib>

    Enemy::Enemy(int x_, int y_, bool scripted) : GameEntity(x_, y_, [] {
        cchar_t s;
        setcchar(&s, L"E", 0, 0, nullptr);
        return s;
    }()), is_scripted(scripted) {}
    void Enemy::update([[maybe_unused]] int input) {
        x--;
        if (is_scripted && rand() % 10 < 3) {
            int player_y = 12;
            if (y < player_y && y < HEIGHT - 1) y++;
            else if (y > player_y && y > 1) y--;
        }
        if (x < 0) active = false;
    }
    bool Enemy::can_shoot() const { return rand() % 100 < 5; }
