#include <ncurses.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <chrono>
#include <thread>
#include <string>
#include <algorithm>

const int WIDTH = 80;
const int HEIGHT = 24;
const int FPS = 30;
const int PLAYER_LIVES = 3;
const int BOSS_SCORE_THRESHOLD = 50;

class GameEntity {
protected:
    int x, y;
    char symbol;
    bool active;
public:
    GameEntity(int x_, int y_, char s) : x(x_), y(y_), symbol(s), active(true) {}
    virtual ~GameEntity() = default;
    virtual void update([[maybe_unused]] int input) {}
    virtual void render() const { if (active) mvaddch(y, x, symbol); }
    bool is_active() const { return active; }
    int get_x() const { return x; }
    int get_y() const { return y; }
    char get_symbol() const { return symbol; }
    void deactivate() { active = false; }
};

class Player : public GameEntity {
    int lives;
public:
    Player(int x_, int y_) : GameEntity(x_, y_, '^'), lives(PLAYER_LIVES) {}
    void update(int input) override {
        if ((input == KEY_UP || input == 'w') && y > 1) y--;
        if ((input == KEY_DOWN || input == 's') && y < HEIGHT - 1) y++;
        if ((input == KEY_LEFT || input == 'a') && x > 0) x--;
        if ((input == KEY_RIGHT || input == 'd') && x < WIDTH / 4) x++;
    }
    int get_lives() const { return lives; }
    void take_damage() { lives--; }
};

class Bullet : public GameEntity {
    bool is_player_bullet;
public:
    Bullet(int x_, int y_, bool player_bullet)
        : GameEntity(x_, y_, player_bullet ? '|' : '*'), is_player_bullet(player_bullet) {}
    void update([[maybe_unused]] int input) override {
        if (is_player_bullet) {
            x++;
            if (x >= WIDTH) active = false;
        } else {
            x--;
            if (x < 0) active = false;
        }
    }
    bool is_from_player() const { return is_player_bullet; }
};

class Enemy : public GameEntity {
    bool is_scripted;
public:
    Enemy(int x_, int y_, bool scripted = false)
        : GameEntity(x_, y_, 'E'), is_scripted(scripted) {}
    void update([[maybe_unused]] int input) override {
        x--;
        if (is_scripted && rand() % 10 < 3) {
            int player_y = 12;
            if (y < player_y && y < HEIGHT - 1) y++;
            else if (y > player_y && y > 1) y--;
        }
        if (x < 0) active = false;
    }
    bool can_shoot() const { return rand() % 100 < 5; }
};

class Boss : public GameEntity {
    int health;
    int width, height;
public:
    Boss(int x_, int y_) : GameEntity(x_, y_, 'B'), health(10), width(2), height(2) {}
    void update([[maybe_unused]] int input) override {
        x--;
        if (x < WIDTH / 2) x = WIDTH / 2;
        if (rand() % 10 < 2) y += (rand() % 2) ? 1 : -1;
        if (y < 1) y = 1;
        if (y > HEIGHT - height) y = HEIGHT - height;
    }
    void render() const override {
        if (active) {
            for (int i = 0; i < height; i++)
                for (int j = 0; j < width; j++)
                    if (y + i < HEIGHT && x + j < WIDTH)
                        mvaddch(y + i, x + j, symbol);
        }
    }
    void take_damage() { health--; if (health <= 0) active = false; }
    bool collides(int px, int py) const {
        return px >= x && px < x + width && py >= y && py < y + height;
    }
};

class Game {
    Player player;
    std::vector<Bullet> bullets;
    std::vector<Enemy> enemies;
    std::vector<Boss> bosses;
    int score;
    bool game_over;
    std::chrono::steady_clock::time_point start_time;

public:
    Game() : player(5, HEIGHT / 2), score(0), game_over(false) {
        srand(time(nullptr));
        initscr();
        cbreak();
        noecho();
        keypad(stdscr, TRUE);
        nodelay(stdscr, TRUE);
        curs_set(0);
        start_time = std::chrono::steady_clock::now();
    }
    ~Game() { endwin(); }

    void run() {
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

private:
    void handle_input(int input) {
        if (input == ' ') {
            bullets.emplace_back(player.get_x() + 1, player.get_y(), true);
        } else if (input == 'q') {
            game_over = true;
        }
    }

    void update(int input) {
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

    void render(int input) {
        clear();
        for (int y = 1; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++)
                mvaddch(y, x, '.');
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

    void check_collisions() {
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
};

int main() {
    Game game;
    game.run();
    return 0;
}

// sudo apt-get install libncurses5-dev