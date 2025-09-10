#include <raylib.h>
#include <string>
#include <fstream>
#include <cctype>

const int FPS = 60;
const int SPRITE_FPS = 12;

struct Player{
    float x;
    float y;
    int width, height;
};

struct Bullet{
    int speed, radius;
    float x, y;
};

struct Controls{
    int up, down, left, right;
};


Controls loadControls(){
    Controls c = {KEY_W, KEY_S, KEY_A, KEY_D}; 
    std::ifstream config("config.ini");
    std::string direction, eq;
    char key;
    while(config >> direction >> eq >> key){
        key = toupper(key);
        if(direction == "UP") c.up = KeyboardKey(key);
        if(direction == "DOWN") c.down = KeyboardKey(key);
        if(direction == "LEFT") c.left = KeyboardKey(key);
        if(direction == "RIGHT") c.right = KeyboardKey(key);
    }
    return c;
}

int main(){
    Player p = {50, 50, 32, 32};
    float speed = 300;
    int index = 0;
    bool isKeyBoardMode = true;
    Vector2 mousePos;
    SetTargetFPS(FPS);
    Controls controls = loadControls();
    InitWindow(800, 600, "Input handling and Timing");
    Texture mage_sprite = LoadTexture("mage_walk.png");


    while(!WindowShouldClose()){
        float delta_time = GetFrameTime();
        if(IsKeyDown(controls.up) || IsKeyDown(KEY_UP)){
            p.y -= speed * delta_time;
        }
        if(IsKeyDown(controls.left) || IsKeyDown(KEY_LEFT)){
            p.x -= speed * delta_time;
        }
        if(IsKeyDown(controls.down) || IsKeyDown(KEY_DOWN)){
            p.y += speed * delta_time;
        }
        if(IsKeyDown(controls.right) || IsKeyDown(KEY_RIGHT)){
            p.x += speed * delta_time;
        }
        /*
            mousePos = GetMousePosition();
            circles[index].x = mousePos.x;
            circles[index].y = mousePos.y;
        */
        BeginDrawing();
        ClearBackground(BLACK);
        DrawTextureEx(mage_sprite, {p.x, p.y}, 0, 5, WHITE);
        EndDrawing();
    }
    UnloadTexture(mage_sprite);
    CloseWindow();
    return 0;
}