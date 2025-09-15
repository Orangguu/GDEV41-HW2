#include <raylib.h>
#include <string>
#include <fstream>
#include <cctype>
#include <cmath>
using namespace std;

const int FPS = 60;
const int SPRITE_FPS = 4;
const float norm = sqrt(2);

enum Direction {
    DOWN,
    DOWN_RIGHT,
    RIGHT,
    UP_RIGHT,
    UP,
    UP_LEFT,
    LEFT,
    DOWN_LEFT
};

struct Player{
    float x;
    float y;
    float width, height;
    int speed = 300;
    bool is_attacking = false;
    bool is_idle = true;
    int skill_cd = 5;
    Direction direction = DOWN;
};

struct Bullet{
    int speed = 500;
    int radius = 5;
    float x = 0, y = 0;
    Vector2 dir = {0, 0};
    bool active = false;
};

struct Skill {
    float x, y;
    float currentRadius = 0.0;
    float maxRadius = 100.0;
    float currentDuration = 0.0; // in seconds
    float maxDuration = 0.5; // in seconds
    float currentCooldown = 5.0; // in seconds
    float maxCooldown = 5.0; // in seconds
    Color color = YELLOW;
    bool isBeingCast = false;
};

struct FrameCoord{
    int x = 0;
    int y = 0;
};

void castSkill(Skill *skill, Player *player, float delta, int frame_size, int sprite_scale) {
    if (skill->currentDuration < skill->maxDuration) {
        skill->x = player->x + ((frame_size * sprite_scale) / 2); // middle of player
        skill->y = player->y + ((frame_size * sprite_scale) / 2); // middle of player
        skill->currentRadius += skill->maxRadius * delta;
    }
    else {
        skill->currentRadius = 0.0;
        skill->currentDuration = 0.0;
        skill->currentCooldown = 0.0;
        skill->isBeingCast = false;
    }
}

int main(){

    Vector2 mousePos;
    SetTargetFPS(FPS);
    InitWindow(800, 600, "Pew Pew Pew");

    // Loading textures
    Texture mage_walk = LoadTexture("mage_walk.png");
    Texture mage_attack = LoadTexture("mage_attack.png");
    
    //attack is 8 by 3, row % 2 is diagonals
    //walk is 8 by 4, row % 2 is diagonals, column 1&2 is idles, column 3&4 are walks. 
    // For now, only walking frames r implemented.
    FrameCoord frame_coord;
    int sprite_scale = 5;
    float frame_width = 32;
    float frame_height = 32;
    float anim_timer = 0.0f;

    Player p = {50, 50, frame_width, frame_height};

    Skill skill;
    Bullet bullet;

    float bulletCooldown = 1.0f;
    float bulletTimer = bulletCooldown;

    while(!WindowShouldClose()){
        float delta_time = GetFrameTime();
        bulletTimer += delta_time;
        skill.currentCooldown += delta_time;

        Vector2 move = {0, 0};
        if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) move.y -= 1;
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) move.x -= 1;
        if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) move.y += 1;
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) move.x += 1;

        // Handles player movement

        float mag = sqrt(move.x * move.x + move.y * move.y);
        if (mag > 0) {
            move.x /= mag;  //normalise
            move.y /= mag;
        }
        // if moving strictly up, right, left, or down, then sqrt(±1^2 + 0^2) = 1
        // if moving diagonally, sqrt(±1^2 + ±1^2) = 1.414
        // ^^ requires normalisation. scale diagonals so movement speed is consistent

        p.x += move.x * p.speed * delta_time;
        p.y += move.y * p.speed * delta_time;

        // Handle player skill (spacebar)
        if(IsKeyPressed(KEY_SPACE) && skill.currentCooldown > skill.maxCooldown) {
            skill.isBeingCast = true;
            castSkill(&skill, &p, delta_time, frame_width, sprite_scale);
        }
        else if (skill.isBeingCast) {
            skill.currentDuration += delta_time;
            castSkill(&skill, &p, delta_time, frame_width, sprite_scale);
        }

        // Set direction of player
        if (move.x < 0 && move.y < 0) p.direction = UP_LEFT;
        else if (move.x > 0 && move.y < 0) p.direction = UP_RIGHT;
        else if (move.x < 0 && move.y > 0) p.direction = DOWN_LEFT;
        else if (move.x > 0 && move.y > 0) p.direction = DOWN_RIGHT;
        else if (move.x < 0) p.direction = LEFT;
        else if (move.x > 0) p.direction = RIGHT;
        else if (move.y < 0) p.direction = UP;
        else if (move.y > 0) p.direction = DOWN;
        p.is_idle = (move.x == 0 && move.y == 0);

        // handle bullets
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) && bulletTimer >= bulletCooldown) {
            if (!p.is_idle) {
                bullet.x = p.x + (p.width * sprite_scale) / 2;
                bullet.y = p.y + (p.height * sprite_scale) / 2;

                // Get mouse and compute direction
                Vector2 mouse = GetMousePosition();
                Vector2 dir = { mouse.x - bullet.x, mouse.y - bullet.y };
                float len = sqrt(dir.x * dir.x + dir.y * dir.y);
                if (len > 0) {
                    dir.x /= len;
                    dir.y /= len;
                }
                bullet.dir = dir;

                bullet.active = true;
                bulletTimer = 0.0f;
                p.is_attacking = true;
                frame_coord.x = 0;
            }
        }


        if (bullet.active) {
            bullet.x += bullet.dir.x * bullet.speed * delta_time;
            bullet.y += bullet.dir.y * bullet.speed * delta_time;

            if (bullet.x < 0 || bullet.x > GetScreenWidth() ||
                bullet.y < 0 || bullet.y > GetScreenHeight()) {
                bullet.active = false;
            }
        }

        // Handle sprite 
        if(p.direction == DOWN) frame_coord.y = 0;
        else if(p.direction == DOWN_RIGHT) frame_coord.y = 1;
        else if(p.direction == RIGHT) frame_coord.y = 2;
        else if(p.direction == UP_RIGHT) frame_coord.y = 3;
        else if(p.direction == UP) frame_coord.y = 4;
        else if(p.direction == UP_LEFT) frame_coord.y = 5;
        else if(p.direction == LEFT) frame_coord.y = 6;
        else if(p.direction == DOWN_LEFT) frame_coord.y = 7;

        // Handle sprite animations
        anim_timer += delta_time;
        if(anim_timer >= 1.0f / SPRITE_FPS){
            anim_timer = 0;

            if (p.is_attacking) {
                frame_coord.x++;
                if (frame_coord.x > 2) {
                    frame_coord.x = 0;
                    p.is_attacking = false;
                }
            } else if (p.is_idle) {
                // Idle cycle (columns 0–1)
                frame_coord.x = (frame_coord.x == 0) ? 1 : 0;
            } else {
                // Walk cycle (columns 2–3)
                frame_coord.x = (frame_coord.x == 2) ? 3 : 2;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);

        Texture currentTexture = p.is_attacking ? mage_attack : mage_walk;
        Rectangle orig_frame = {frame_coord.x * frame_width, frame_coord.y * frame_height, frame_width, frame_height};
        Rectangle scaled_frame = {p.x, p.y, frame_width * sprite_scale, frame_height * sprite_scale};
        DrawTexturePro(currentTexture, orig_frame, scaled_frame, {0, 0}, 0, WHITE);

        // draw cast skill
        DrawCircle(skill.x, skill.y, skill.currentRadius, skill.color);

        // draw bullet
        if (bullet.active) {
            DrawCircle(bullet.x, bullet.y, bullet.radius, Color{240, 216, 225, 255});
        }

        EndDrawing();
    }

    UnloadTexture(mage_walk);
    UnloadTexture(mage_attack);
    CloseWindow();
    return 0;
}