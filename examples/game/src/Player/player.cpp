#include "Player/player.h"

Player::Player(float x, float y, float acc, float jumpForce, float gravityForce, float fallMultiplier) : SineSprite(x, y),
    acc(acc),
    jumpForce(jumpForce),
    gravityForce(gravityForce),
    fallMultiplier(fallMultiplier)
{
}

void Player::update(float dt) {
    SineSprite::update(dt);
    
    // SetPlayerBounds();
    if(collisions["down"]) {
        SetGravity(0);
    }
    else {
        SetGravity(gravityForce);
        if(velocity.y > 0) {
            SetGravity(gravityForce*fallMultiplier);
        }
    }
    
    // Sets the camera zoom
    if(IsKeyPressed(KEY_Z)) {
        camera->zoom = 1.2f;
    }
    
    if(IsKeyDown(KEY_D)) {
        acceleration.x = acc;
    }
    else if(IsKeyDown(KEY_A)) {
        acceleration.x = -acc;
    }
    else {
        acceleration.x = 0;
    }
    
    if(IsKeyPressed(KEY_SPACE)) {
        velocity.y = jumpForce;
    }
}

void Player::draw() {
    SineSprite::draw();
}

void Player::SetPlayerBounds()
{
    // Setting bounds on the Y axis
    if(position.y + hitbox.height >= gameHeight) {
        acceleration.y = 0;
        velocity.y *= -1; // Change this to 0 to make it just stay on the ground
        position.y = gameHeight - hitbox.height;
    }
    else if(position.y <= 0) {
        acceleration.y = 0;
        velocity.y *= -1;
        position.y = 0;
    }
    else {
        SetGravity(gravityForce);
        if(velocity.y > 0) {
            SetGravity(gravityForce*fallMultiplier);
        }
    }
    
    // Setting bounds on the X axis
    if(position.x + hitbox.width >= gameWidth) {
        acceleration.x = 0;
        velocity.x *= -1;
        position.x = gameWidth - hitbox.width;
    }
    if(position.x <= 0) {
        acceleration.x = 0;
        velocity.x *= -1;
        position.x = 0;
    }
}

void Player::SetGravity(float g) {
    gravity = g;
}
