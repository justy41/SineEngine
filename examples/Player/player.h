#pragma once
#include <iostream>
#include "sine.h"

class Player : public SineSprite
{
private:
    /* data */
public:
    float acc, jumpForce, gravityForce, fallMultiplier;
    
    Player(float x, float y, float acc, float jumpForce, float gravityForce = 640, float fallMultiplier = 1.5f);
    
    void update(float dt) override;
    void draw() override;
    void SetPlayerBounds();
    void SetGravity(float g);
    
    ~Player() {}
};
