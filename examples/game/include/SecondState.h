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
        
        LoadLDtkMap(RESOURCES_PATH "tilemaps/map_0.ldtk", 16, {"Ground", "Snow"});
        
        player = new Player(500, 100, 1200, -400);
        player->loadTexture(RESOURCES_PATH "circle.png");
        player->tint = BLUE;
        player->drag = Vector2{500, 200};
        player->solid = true;
        add(player);
        
        camera.target = player->position;
    }
    
    void update(float dt) override {
        SineState::update(dt);
        CameraFollow(player->position);
        
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
            DrawLDtkCollisionLayers();
        EndMode2D();
    }
};
