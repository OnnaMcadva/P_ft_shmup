#include "Player.hpp"
#include "Game.hpp"
#include "Constants.hpp"

    Player::Player(int x_, int y_) : GameEntity(x_, y_, [] {
        cchar_t s;
        setcchar(&s, L"🦚", 0, 0, nullptr);
        return s;
    }()), lives(PLAYER_LIVES) {}
    void Player::update(int input) {
        if ((input == KEY_UP || input == 'w') && y > 1) y--;
        if ((input == KEY_DOWN || input == 's') && y < HEIGHT - 1) y++;
        if ((input == KEY_LEFT || input == 'a') && x > 0) x--;
        if ((input == KEY_RIGHT || input == 'd') && x < WIDTH / 4) x++;
    }
    int Player::get_lives() const { return lives; }
    void Player::take_damage() { lives--; }