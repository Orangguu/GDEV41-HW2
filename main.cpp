#include <raylib.h>
#include <string>
#include <fstream>
#include <cctype>
using namespace std;

const int FPS = 60;
const int SPRITE_FPS = 4;

struct Player{
    float x;
    float y;
    float width, height; 
    int speed = 300;
    bool is_attacking = false;
    bool is_idle = true;
    int skill_cd = 5;
    string direction = "";
};

struct Bullet{
    int speed = 10;
    int radius = 5;
    float x, y;
    string direction = "";
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

    while(!WindowShouldClose()){
        float delta_time = GetFrameTime();
        skill.currentCooldown += delta_time;

        Vector2 move = {0, 0};
        if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) move.y -= 1;
        if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) move.x -= 1;
        if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) move.y += 1;
        if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) move.x += 1;

        // Handles player movement
        p.x += move.x * p.speed * delta_time;
        p.y += move.y * p.speed * delta_time;

        // Handles player skill
        if(IsKeyPressed(KEY_SPACE) && skill.currentCooldown > skill.maxCooldown) {
            skill.isBeingCast = true;
            castSkill(&skill, &p, delta_time, frame_width, sprite_scale);
        }
        else if (skill.isBeingCast) {
            skill.currentDuration += delta_time;
            castSkill(&skill, &p, delta_time, frame_width, sprite_scale);
        }

        // Set direction of player
        if (move.x < 0 && move.y < 0) p.direction = "up-left";
        else if (move.x > 0 && move.y < 0) p.direction = "up-right";
        else if (move.x < 0 && move.y > 0) p.direction = "down-left";
        else if (move.x > 0 && move.y > 0) p.direction = "down-right";
        else if (move.x < 0) p.direction = "left";
        else if (move.x > 0) p.direction = "right";
        else if (move.y < 0) p.direction = "up";
        else if (move.y > 0) p.direction = "down";
        p.is_idle = (move.x == 0 && move.y == 0);

        // Sprite handling 
        if(p.direction == "down") frame_coord.y = 0;
        else if(p.direction == "down-right") frame_coord.y = 1 ;
        else if(p.direction == "right") frame_coord.y = 2;
        else if(p.direction == "up-right") frame_coord.y = 3;
        else if(p.direction == "up") frame_coord.y = 4;
        else if(p.direction == "up-left") frame_coord.y = 5;
        else if(p.direction == "left") frame_coord.y = 6;
        else if(p.direction == "down-left") frame_coord.y = 7;

        //Handle sprite animations
        anim_timer += delta_time;
        if(anim_timer >= 1.0f / SPRITE_FPS){
            anim_timer = 0;
            if (p.is_idle) {
                // Idle cycle (columns 0–1)
                frame_coord.x = (frame_coord.x == 0) ? 1 : 0;
            } else {
                // Walk cycle (columns 2–3)
                frame_coord.x = (frame_coord.x == 2) ? 3 : 2;
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        Rectangle orig_frame = {frame_coord.x * frame_width, frame_coord.y * frame_height, frame_width, frame_height};
        Rectangle scaled_frame = {p.x, p.y, frame_width * sprite_scale, frame_height * sprite_scale};
        DrawTexturePro(mage_walk, orig_frame, scaled_frame, {0, 0}, 0, WHITE);
        DrawCircle(skill.x, skill.y, skill.currentRadius, skill.color);
        EndDrawing();
    }
    UnloadTexture(mage_walk);
    CloseWindow();
    return 0;
}