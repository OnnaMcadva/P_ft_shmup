#pragma once
#include "GameEntity.hpp"

class Bullet : public GameEntity {
    bool is_player_bullet;
public:
    Bullet(int x_, int y_, bool player_bullet);
    void update([[maybe_unused]] int input) override;
    bool is_from_player() const;
};