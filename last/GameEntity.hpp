#pragma once
#include <ncursesw/ncurses.h>

class GameEntity {
protected:
    int x, y;
    cchar_t symbol;
    bool active;
public:
    GameEntity(int x_, int y_, const cchar_t& s);
    virtual ~GameEntity() = default;
    virtual void update([[maybe_unused]] int input);
    virtual void render() const;
    bool is_active() const;
    int get_x() const;
    int get_y() const;
    void deactivate();
};