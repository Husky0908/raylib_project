#include "raylib.h"
#include <string>
#include <iostream>
#include <cmath>
#include <vector>

struct RaylibContext {
    RaylibContext(int widht, int height, int fps) {
        screen_widht = widht;
        screen_height = height;
        targetFPS = fps;
    }

    int targetFPS;
    int screen_widht;
    int screen_height;

    void init_window() {
        InitWindow(screen_widht, screen_height, "raylib project");
        SetTargetFPS(targetFPS);
    }
};

void get_vector(float &vec_x, float &vec_y) {
    float vector = sqrt(vec_x * vec_x + vec_y * vec_y);
    vec_x = vec_x / vector;
    vec_y = vec_y / vector;
}

class Player {
protected:
    Player() {
        health = 10;
        energy = 10;
        max_energy = 10;
        energy_time = 0.0;
        no_energy = false;
        state = "MOVE";
        x = 0.0;
        y = 50.0;
        speed = 250.0;
        stick_deadzone = 0.1;
    }
    
    int health;
    int energy;
    int max_energy;
    float energy_time;
    float x;
    float y;
    float speed;
    std::string state;
    float left_stick_X;
    float left_stick_Y;
    float stick_deadzone;
    float dash_time;
    float vector;
    float degrees;
    float hit_time;
    bool no_energy;
    float no_energy_time;
    
public:
    virtual ~Player() {}
    void move() {
        get_vector(&left_stick_X, &left_stick_Y);
        
        x = x + GetFrameTime() * speed * left_stick_X;
        y = y + GetFrameTime() * speed * left_stick_Y;
    }
    void dash() {
        x = x + GetFrameTime() * speed * left_stick_X;
        y = y + GetFrameTime() * speed * left_stick_Y;
        dash_time = dash_time + GetFrameTime();
        if (dash_time >= 0.35) { state = "MOVE"; }
    }
    virtual void small_hit(int plus_degrees, float hitting_time) = 0;
    virtual void big_hit(bool press_button) = 0;
    virtual void drink_potion() = 0;
    virtual void draw() = 0;
    virtual void get_input() = 0;
    virtual void get_hit_degrees() = 0;
    void get_energy() {
        if (energy < max_energy) {
            energy_time = energy_time + GetFrameTime();
            if (energy_time >= 1.5) {
                energy = energy + 1;
                energy_time = 0.0;
            }
        } else { energy_time = 0; }
    }
    float my_x_coordinate() {
        return x;
    }
    float my_y_coordinate() {
        return y;
    }
};

class Knight : public Player {
protected:
    bool big_hit_fine;

public:
    Knight() : Player() {}

    void small_hit(int plus_degrees, float hitting_time) override {
        hit_time = hit_time + GetFrameTime();
        degrees = degrees + plus_degrees * GetFrameTime() * hitting_time;
        if (hit_time >= (1 / hitting_time)) {
            state = "MOVE";
            hit_time = 0.0;
        }
    }
    void big_hit(bool press_button) override {
        if (press_button && degrees == 0) {
            hit_time = hit_time + GetFrameTime();
            if (hit_time >= 0.5) { big_hit_fine = true; }
        } else if (big_hit_fine) {
            if (degrees == 0) {
                get_hit_degrees();
                hit_time = 0.0;
                energy = energy - 2;
            }
            small_hit(360, 2);
        } else {
            state = "MOVE";
            hit_time = 0.0;
        }
    }
    void drink_potion() override {}
    void draw() override {
        DrawCircleV({x, y}, 40, BLUE);
        if (no_energy) {
            no_energy_time = no_energy_time + GetFrameTime();
            DrawText(TextFormat("E: %i/%i\nNO ENERGY", energy, max_energy), 20, 40, 30, PURPLE);
            if (no_energy_time >= 1) { no_energy = false; }
        } else { DrawText(TextFormat("E: %i/%i", energy, max_energy), 20, 40, 25, BLUE); }
        if (state == "SMALL HIT") {
            DrawRectanglePro({x, y - 5, 80, 10}, {0, 5}, degrees, GRAY);
        }
        if (state == "BIG HIT") {
            if (!(degrees == 0)) {
                DrawRectanglePro({x, y - 5, 80, 10}, {0, 5}, degrees, GRAY);
            } else if (!(big_hit_fine)) {
                DrawRectangle(x - (30 * hit_time), y - (30 * hit_time), 30 * hit_time * 2, 30 * hit_time * 2, GRAY);
            } else { DrawRectangle(x - 15, y - 15, 30, 30, BLACK); }
        }
    }
    void get_input() override {
        if (!(state == "DASH")) {
            speed = 250.0;
            dash_time = dash_time + GetFrameTime();
            left_stick_X = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            left_stick_Y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            if (left_stick_X > -stick_deadzone && left_stick_X < stick_deadzone) { left_stick_X = 0.0; }
            if (left_stick_Y > -stick_deadzone && left_stick_Y < stick_deadzone) { left_stick_Y = 0.0; }
            if (!(left_stick_X == 0.0) || !(left_stick_Y == 0.0)) { move(); }

            if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_LEFT) && state == "MOVE" && hit_time >= 0.25) {
                if (energy >= 1) {
                    hit_time = 0.0;
                    state = "SMALL HIT";
                    energy = energy - 1;
                    get_hit_degrees();
                } else {
                    no_energy = true;
                    no_energy_time = 0.0;
                }
            }
            if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) && state == "MOVE" && hit_time >= 0.25) {
                if (energy >= 2) {
                    hit_time = 0.0;
                    big_hit_fine = false;
                    degrees = 0;
                    state = "BIG HIT";
                } else {
                    no_energy = true;
                    no_energy_time = 0.0;
                }
            }
        }

        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && state == "MOVE" && dash_time >= 1) {
            if (energy >= 1) {
                get_vector(&left_stick_X, &left_stick_Y);
                energy = energy - 1;
                state = "DASH";
                dash_time = 0.0;
                speed = 575.0;
            } else {
                no_energy = true;
                no_energy_time = 0.0;
            }
        }
        if (state == "DASH") {
            dash();
        }
        if (state == "SMALL HIT") {
            small_hit(100, 8.0);
        } else if (state == "BIG HIT") {
            big_hit(IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP));
        } else { hit_time = hit_time + GetFrameTime(); }
    }
    void get_hit_degrees() override {
        degrees = atan(left_stick_Y / left_stick_X + 0.01) * (180  / 3.1415);
        if (left_stick_X < 0) {
            degrees = degrees + 180;
        }
        degrees = degrees - 50;
    }

};

class Enemy {
protected:
    Enemy() {
        health = 4;
        speed = 1.0;
        x = 300.0;
        y = 50.0;
        state = "MOVE";
    }
    
    int health;
    float speed;
    float x;
    float y;
    std::string state;
    float vector_x;
    float vector_y;
    
public:
    virtual ~Enemy() {}
    void move(Player* player) {
        vector_x = player->my_x_coordinate() - x;
        vector_y = player->my_y_coordinate() - y;
        get_vector(float* vector_x, float* vector_y);
        
        x = x + GetFrameTime() * speed * vector_x;
        y = y + GetFrameTime() * speed * vector_y;
    }
    virtual void attact() = 0;
    virtual void draw() = 0;
    virtual void ai(Player* player) = 0;
    virtual void get_hit_degrees() = 0;
};

class Small_Knight : public Enemy {
public:
    Small_Knight() : Enemy() {}
    void attact() override {}
    void draw() override {
        DrawRectangle(x - 15, y - 15, 30, 30, GREEN);
    }
    void ai(Player* player) override {
        if (state == "MOVE") {
            move(player);
        }
    }
    void get_hit_degrees() override {}
};

int main()
{
    RaylibContext ralibcontext(1080, 620, 80);
    Player* player;
    std::vector<Enemy*> enemies;

    player = new Knight();
    enemies.push_back(new Small_Knight());
    
    ralibcontext.init_window();
    
    while (!WindowShouldClose())
    {
        player->get_energy();
        player->get_input();
        for (Enemy* enemy : enemies) {
            enemy->ai(player);
            enemy->draw();
        }
        
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        player->draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
