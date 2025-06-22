#pragma once
#include <iostream>
#include "sine.h"

class SecondState : public SineState
{
private:
    
public:
    SineSprite* player;
    
    void start() override {
        player = new SineSprite(300, 150);
        player->loadTexture(RESOURCES_PATH "circle.png");
        player->tint = RED;
        player->drag.x = 400;
        add(player);
        
        SineState::start();
    }
    
    void update(float dt) override {
        SineState::update(dt);
        
        if(IsKeyDown(KEY_D)) {
            player->acceleration.x = 1200;
        }
        else if(IsKeyDown(KEY_A)) {
            player->acceleration.x = -1200;
        }
        else {
            player->acceleration.x = 0;
        }
        
        if(IsKeyPressed(KEY_P)) {
            manager->SwitchState(1);
        }
    }
    
    void draw() override {
        SineState::draw();
    }
};
