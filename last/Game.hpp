#pragma once
#include <vector>
#include <chrono>
#include "Player.hpp"
#include "Bullet.hpp"
#include "Enemy.hpp"
class Boss;

class Game {
public:
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Boss> bosses;
    int score;
    bool game_over;
    std::chrono::steady_clock::time_point start_time;

public:
    Game();
    ~Game();
    void run();

private:
    void cleanup();
    void handle_input(int input);
    void update(int input);
    void render(int input);
    void check_collisions();
};