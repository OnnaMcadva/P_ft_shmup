#pragma once
#include "GameEntity.hpp"

class Enemy : public GameEntity {
    bool is_scripted;
public:
    Enemy(int x_, int y_, bool scripted = false);
    void update([[maybe_unused]] int input) override;
    bool can_shoot() const;
};
