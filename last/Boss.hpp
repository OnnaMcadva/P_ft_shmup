#pragma once
#include "GameEntity.hpp"

class Boss : public GameEntity {
    int health;
    int width, height;
public:
    Boss(int x_, int y_);
    void update([[maybe_unused]] int input) override;
    void render() const override;
    void take_damage();
    bool collides(int px, int py) const;
};