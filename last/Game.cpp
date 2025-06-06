#include "Game.hpp"
#include "Constants.hpp"
#include "Boss.hpp"
#include <ncursesw/ncurses.h>
#include <cstdlib>
#include <ctime>
#include <thread>
#include <chrono>
#include <algorithm>

    Game::Game() : player(5, HEIGHT / 2), score(0), game_over(false) {
        setlocale(LC_ALL, "");
        srand(time(nullptr));
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
        start_time = std::chrono::steady_clock::now();
    }
    
    Game::~Game() {
        cleanup();
        endwin();
    }

    void Game::run() {
        while (!game_over) {
            int input = getch();
            handle_input(input);
            update(input);
            render(input);
            check_collisions();
            std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
        }
        clear();
        mvprintw(HEIGHT / 2, WIDTH / 2 - 5, "Game Over! Score: %d", score);
        refresh();
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }

    void Game::cleanup() {
        bullets.clear();
        bullets.shrink_to_fit();
        enemies.clear();
        enemies.shrink_to_fit();
        bosses.clear();
        bosses.shrink_to_fit();
    }

    void Game::handle_input(int input) {
        if (input == ' ') {
            bullets.emplace_back(player.get_x() + 1, player.get_y(), true);
        } else if (input == 'q') {
            game_over = true;
        }
    }

    void Game::update(int input) {
        if (rand() % 100 < 15) {
            bool scripted = (score >= 20 && rand() % 2);
            enemies.emplace_back(WIDTH - 1, rand() % (HEIGHT - 2) + 1, scripted);
        }
        if (score >= BOSS_SCORE_THRESHOLD && bosses.empty()) {
            bosses.emplace_back(WIDTH - 2, HEIGHT / 2);
            score += 10;
        }
        player.update(input);
        for (auto& bullet : bullets) bullet.update(input);
        for (auto& enemy : enemies) {
            enemy.update(input);
            if (enemy.is_active() && enemy.can_shoot()) {
                bullets.emplace_back(enemy.get_x() - 1, enemy.get_y(), false);
            }
        }
        for (auto& boss : bosses) boss.update(input);
        bullets.erase(std::remove_if(bullets.begin(), bullets.end(),
            [](const Bullet& b) { return !b.is_active(); }), bullets.end());
        enemies.erase(std::remove_if(enemies.begin(), enemies.end(),
            [](const Enemy& e) { return !e.is_active(); }), enemies.end());
        bosses.erase(std::remove_if(bosses.begin(), bosses.end(),
            [](const Boss& b) { return !b.is_active(); }), bosses.end());
    }

    void Game::render(int input) {
        clear();
        for (int y = 1; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++) {
                cchar_t dot;
                setcchar(&dot, L".", 0, 0, nullptr);
                mvadd_wch(y, x, &dot);
            }
        player.render();
        for (const auto& bullet : bullets) bullet.render();
        for (const auto& enemy : enemies) enemy.render();
        for (const auto& boss : bosses) boss.render();
        auto now = std::chrono::steady_clock::now();
        int time = std::chrono::duration_cast<std::chrono::seconds>(now - start_time).count();
        mvprintw(0, 0, "Score: %d | Lives: %d | Time: %d", score, player.get_lives(), time);
        mvprintw(0, WIDTH - 20, "Key: %d", input);
        int player_bullets = 0;
        for (const auto& bullet : bullets) if (bullet.is_from_player()) player_bullets++;
        mvprintw(1, WIDTH - 20, "Bullets: %d", player_bullets);
        refresh();
    }

    void Game::check_collisions() {
        for (auto& enemy : enemies) {
            if (enemy.is_active() && enemy.get_x() == player.get_x() && enemy.get_y() == player.get_y()) {
                player.take_damage();
                if (player.get_lives() <= 0) game_over = true;
            }
        }
        for (auto& boss : bosses) {
            if (boss.is_active() && boss.collides(player.get_x(), player.get_y())) {
                player.take_damage();
                if (player.get_lives() <= 0) game_over = true;
            }
        }
        for (auto& bullet : bullets) {
            if (bullet.is_active() && !bullet.is_from_player() &&
                bullet.get_x() == player.get_x() && bullet.get_y() == player.get_y()) {
                player.take_damage();
                bullet.deactivate();
                if (player.get_lives() <= 0) game_over = true;
            }
        }
        for (auto& bullet : bullets) {
            if (bullet.is_active() && bullet.is_from_player()) {
                for (auto& enemy : enemies) {
                    if (enemy.is_active() &&
                        abs(bullet.get_x() - enemy.get_x()) <= 1 &&
                        bullet.get_y() == enemy.get_y()) {
                        enemy.deactivate();
                        bullet.deactivate();
                        score += 1;
                        mvprintw(1, 0, "Hit Enemy at (%d, %d)!", bullet.get_x(), bullet.get_y());
                    }
                }
                for (auto& boss : bosses) {
                    if (boss.is_active() && boss.collides(bullet.get_x(), bullet.get_y())) {
                        boss.take_damage();
                        bullet.deactivate();
                        score += 5;
                        mvprintw(2, 0, "Hit Boss at (%d, %d)!", bullet.get_x(), bullet.get_y());
                    }
                }
            }
        }
    }
