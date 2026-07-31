#include "raylib.h"
#include <string>
#include <iostream>
#include <cmath>

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

class Player {
protected:
    Player() {
        health = 10;
        energy = 10;
        state = "MOVE";
        x = 0.0;
        y = 50.0;
        speed = 250.0;
        stick_deadzone = 0.1;
    }
    
    int health;
    int energy;
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
    
public:
    virtual ~Player() {}
    void move() {
        get_vector();
        
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
    void get_vector() {
        vector = sqrt(left_stick_X * left_stick_X + left_stick_Y * left_stick_Y);
        left_stick_X = left_stick_X / vector;
        left_stick_Y = left_stick_Y / vector;
    }
    virtual void get_hit_degrees() = 0;
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
        if (press_button) {
            hit_time = hit_time + GetFrameTime();
            if (hit_time >= 1) { big_hit_fine = true; }
        } else if (big_hit_fine) {
            if (degrees == 0) {
                get_hit_degrees();
                hit_time = 0.0;
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
        if (state == "SMALL HIT") {
            DrawRectanglePro({x, y - 5, 80, 10}, {0, 5}, degrees, GRAY);
        }
        if (state == "BIG HIT") {
            if (!(degrees == 0)) {
                DrawRectanglePro({x, y - 5, 80, 10}, {0, 5}, degrees, GRAY);
            } else if (!(big_hit_fine)) {
                DrawRectangle(x - (30 * hit_time / 2), y - (30 * hit_time / 2), 30 * hit_time, 30 * hit_time, GRAY);
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
                hit_time = 0.0;
                state = "SMALL HIT";
                get_hit_degrees();
            }
            if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_UP) && state == "MOVE" && hit_time >= 0.25) {
                hit_time = 0.0;
                big_hit_fine = false;
                degrees = 0;
                state = "BIG HIT";
            }
        }

        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && state == "MOVE" && dash_time >= 1) {
            get_vector();
            state = "DASH";
            dash_time = 0.0;
            speed = 575.0;
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

int main()
{
    RaylibContext ralibcontext(1080, 620, 80);
    Player* player;

    player = new Knight();
    
    ralibcontext.init_window();
    
    while (!WindowShouldClose())
    {
        player->get_input();
        
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        player->draw();
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
