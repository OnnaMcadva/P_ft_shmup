#include "Boss.hpp"
#include "Constants.hpp"
#include <cstdlib>
#include <ncursesw/ncurses.h>


    Boss::Boss(int x_, int y_) : GameEntity(x_, y_, [] {
        cchar_t s;
        setcchar(&s, L"B", 0, 0, nullptr);
        return s;
    }()), health(10), width(2), height(2) {}
    void Boss::update([[maybe_unused]] int input) {
        x--;
        if (x < WIDTH / 2) x = WIDTH / 2;
        if (rand() % 10 < 2) y += (rand() % 2) ? 1 : -1;
        if (y < 1) y = 1;
        if (y > HEIGHT - height) y = HEIGHT - height;
    }
    void Boss::render() const {
        if (active) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    if (y + i < HEIGHT && x + j < WIDTH)
                        mvadd_wch(y + i, x + j, &symbol);
        }
    }
    void Boss::take_damage() { health--; if (health <= 0) active = false; }
    bool Boss::collides(int px, int py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }