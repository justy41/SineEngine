# SineEngine - C++ raylib based engine
Sine = ***SI***mple engi***NE***
###
A simple non-editor engine made for game development in C++ with [raylib](https://github.com/raysan5/raylib), [LDtkLoader](https://github.com/Madour/LDtkLoader) and [ImGui](https://github.com/ocornut/imgui) (also [rlImGui](https://github.com/raylib-extras/rlImGui)).
Its API is inspired by the game engine [HaxeFlixel](https://haxeflixel.com/).

## Features
- Simple InitSineWindow() function to initialize your game with custom resolution, config flags, and FPS settings
- ```SineBasic```: Base class with update/draw/destroy interface
- ```SineEntity```: Adds physics-like properties such as velocity, acceleration, gravity, drag, hitbox, and more
- ```SineSprite```: Extends SineEntity with texture rendering, scaling, tinting, and hitbox visualization
- ```SineGroup```: Manages collections of objects (scene graphs)
###
- ```SineState```: Represents a game screen or scene with built-in camera and virtual mouse support
- ```SineStateManager```: Hot-swappable and recreatable state containers via unique pointers and factory lambdas
- Virtual mouse coordinates scale perfectly with resolution changes
###
- Built-in ```Camera2D``` support per state
- Automatic screen scaling and offset calculation
- Support for ```BeginMode2D/EndMode2D``` encapsulated in state rendering
###
- Manual object lifetime control with new and delete managed by ```SineGroup```
- Smart pointers ```(std::unique_ptr)``` used for safe state recreation
###
- 🔧 Dear ImGui: Integrated in the static library for in-game UI overlays and debugging tools
- 🧱 LDtkLoader: Integrated for seamless loading of LDtk level design data

## Example
***main.cpp***
```c
#include <iostream>
#include <cmath>
#include <memory>
#include "sine.h"
#include "TemplateState.h"
#include "SecondState.h"

int main(){
    InitSineWindow(1280, 720, 640, 360, "game", FLAG_WINDOW_RESIZABLE, FLAG_WINDOW_TRANSPARENT);
    RenderTexture2D target = LoadRenderTexture(gameWidth, gameHeight);
    float scale = 0;
    
    // ======================================================== START ========================================================= //
    SineStateManager manager = SineStateManager();
    manager.add(std::make_unique<TemplateState>());
    manager.add(std::make_unique<SecondState>());
    
    manager.start();
    
    while(!WindowShouldClose()) {
    // ======================================================== UPDATE ======================================================== //
        scale = std::min((float)GetScreenWidth()/gameWidth, (float)GetScreenHeight()/gameHeight);
        manager.update(GetFrameTime());
        
    // ========================================================= DRAW ========================================================= //
        BeginTextureMode(target);
            ClearBackground(RAYWHITE);
            manager.draw();
        EndTextureMode();
        
        DrawLetterBox(target, scale, gameWidth, gameHeight);
    }
    
    manager.UnloadStates();
    CloseWindow();
}
```

***TemplateState.h***

```c
#pragma once
#include <iostream>
#include <vector>
#include "sine.h"

class TemplateState : public SineState
{
private:
    
public:
    SineSprite* player;
    std::vector<Vector2> balls_positions;
    
    void start() override {
        SineState::start();
        
        player = new SineSprite(100, 100);
        player->loadTexture(RESOURCES_PATH "button.png");
        add(player);
    }
    
    void update(float dt) override {
        SineState::update(dt);
        
        if(IsKeyPressed(KEY_P)) {
            manager->SwitchState(2);
        }
        
        if(IsMouseButtonPressed(0)) {
            balls_positions.push_back(GetVirtualMousePosition());
        }
    }
    
    void draw() override {
        SineState::draw();
        
        for(int i = 0; i<balls_positions.size(); i++) {
            DrawCircle(balls_positions[i].x, balls_positions[i].y, 5, PURPLE);
        }
    }
};
```

***SecondState.h***

The Player code can be found in the examples folder.
```c
#pragma once
#include <iostream>
#include "sine.h"
#include "Player/player.h"

class SecondState : public SineState
{
private:
    
public:
    Player* player;
    
    void start() override {
        SineState::start();
        
        camera.zoom = 1;
        
        player = new Player(100, 100, 1200, -400);
        player->loadTexture(RESOURCES_PATH "circle.png");
        player->tint = BLUE;
        player->drag = Vector2{500, 200};
        add(player);
    }
    
    void update(float dt) override {
        SineState::update(dt);
        
        if(IsKeyPressed(KEY_P)) {
            manager->SwitchState(1);
        }
    }
    
    void draw() override {
        SineState::draw();
    }
};
```

## Build and installation
**For now the engine has only been tested on Windows!** Everything you need is inside the ```core```, ```include```, and ```thirdParty``` folders.

If you are using CMake there also is a CMakeLists.txt file which automatically builds the project as a static library for windows. Even though it looks like an only-header library, there also is a ```sine.cpp``` file that allows for static library building.
