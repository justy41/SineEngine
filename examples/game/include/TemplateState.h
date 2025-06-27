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
        BeginMode2D(camera);
            SineState::draw();
            
            for(int i = 0; i<balls_positions.size(); i++) {
                DrawCircle(balls_positions[i].x, balls_positions[i].y, 5, PURPLE);
            }
        EndMode2D();
    }
};
