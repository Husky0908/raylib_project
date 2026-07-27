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
        state = "STANDING";
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
    
public:
    virtual ~Player() {}
    virtual void move() = 0;
    virtual void dash() = 0;
    virtual void small_hit() = 0;
    virtual void big_hit() = 0;
    virtual void drink_potion() = 0;
    virtual void draw() = 0;
    virtual void get_input() = 0;
    void get_vector() {
        while(sqrt(left_stick_X * left_stick_X + left_stick_Y * left_stick_Y) > 1.0) {
            if (left_stick_X > 0) {
                left_stick_X = left_stick_X - 0.1;
            } else { left_stick_X = left_stick_X + 0.1; }
            if (left_stick_Y > 0) {
                left_stick_Y = left_stick_Y - 0.1;
            } else { left_stick_Y = left_stick_Y + 0.1; }
        }
    }
};

class Knight : public Player {
public:
    Knight() : Player() {}

    void move() override {
        get_vector();
        
        x = x + GetFrameTime() * speed * left_stick_X;
        y = y + GetFrameTime() * speed * left_stick_Y;
    }
    void dash() override {
        x = x + GetFrameTime() * speed * left_stick_X;
        y = y + GetFrameTime() * speed * left_stick_Y;
        dash_time = dash_time + GetFrameTime();
        if (dash_time >= 0.35) { state = "STANDING"; }
    }
    void small_hit() override {}
    void big_hit() override {}
    void drink_potion() override {}
    void draw() override {
        DrawCircleV({x, y}, 40, BLUE);
    }
    void get_input() override {
        if (!(state == "DASH")) {
            speed = 250.0;
            left_stick_X = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_X);
            left_stick_Y = GetGamepadAxisMovement(0, GAMEPAD_AXIS_LEFT_Y);
            if (left_stick_X > -stick_deadzone && left_stick_X < stick_deadzone) { left_stick_X = 0.0; }
            if (left_stick_Y > -stick_deadzone && left_stick_Y < stick_deadzone) { left_stick_Y = 0.0; }
            if (!(left_stick_X == 0.0) || !(left_stick_Y == 0.0)) { move(); }
        }
            
        if (IsGamepadButtonDown(0, GAMEPAD_BUTTON_RIGHT_FACE_DOWN) && !(state == "DASH")) {
            get_vector();
            state = "DASH";
            dash_time = 0.0;
            speed = 575.0;
        }
        if (state == "DASH") {
            dash();
        }
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
