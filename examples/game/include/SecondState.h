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
        
        LoadLDtkMap(RESOURCES_PATH "tilemaps/map_0.ldtk");
        camera.zoom = 1;
        
        player = new Player(100, 100, 1200, -400);
        player->loadTexture(RESOURCES_PATH "circle.png");
        player->tint = BLUE;
        player->drag = Vector2{500, 200};
        add(player);
    }
    
    void update(float dt) override {
        SineState::update(dt);
        
        if(IsKeyPressed(KEY_C)) {
            camera.zoom -= 0.4f;
        }
        if(IsKeyPressed(KEY_X)) {
            camera.zoom += 0.4f;
        }
        
        if(IsKeyPressed(KEY_P)) {
            manager->SwitchState(1);
        }
    }
    
    void draw() override {
        BeginMode2D(camera);
            SineState::draw();
            DrawLDtkMap();
        EndMode2D();
    }
};
