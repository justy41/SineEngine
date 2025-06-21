#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <memory>
#include <typeindex>
#include <functional>
#include "raylib.h"
#include "raymath.h"

inline void InitSineWindow(int screenW, int screenH, const char* window_name, unsigned int configFlags = 0, unsigned int windowStateFlags = 0, int fps = 60){
    SetConfigFlags(configFlags);
    InitWindow(screenW, screenH, window_name);
    SetWindowState(windowStateFlags);
    
    SetTargetFPS(fps);
}

class Entity
{
private:
    
public:
    Vector2 position;
    float rotation;
    Vector2 scale;
    
    char tag[256];
    
    Entity() {
        position = Vector2{0, 0};
        rotation = 0;
        scale = Vector2{1, 1};
        std::strcpy(tag, "");
    }
    
    Entity(float x, float y) {
        position = Vector2{x, y};
        rotation = 0;
        scale = Vector2{1, 1};
        std::strcpy(tag, "");
    }
    
    void SetTag(const char* tag) {
        std::strcpy(this->tag, tag);
    }
    
    virtual void Start() {
        
    }
    
    virtual void Update(float deltaTime) {
        
    }
    
    virtual void Draw() {
        DrawCircle(position.x, position.y, 2, GREEN);
    }
    
    ~Entity() {
        
    }
};



class Sprite : public Entity
{
private:
    Rectangle source;
    Vector2 origin;
public:
    Texture2D texture;
    Color tint;
    
    Sprite(const char* texture_path, float x, float y, Color tint = WHITE) : Entity(x, y) {
        texture = LoadTexture(texture_path);
        source = Rectangle{0, 0, (float)texture.width, (float)texture.height};
        origin = Vector2{texture.width/2.f, texture.height/2.f};
        this->tint = tint;
    }
    
    Sprite(const char* texture_path, float x, float y, Rectangle source, Color tint = WHITE) : Entity(x, y) {
        texture = LoadTexture(texture_path);
        this->source = source;
        origin = Vector2{source.width/2.f, source.height/2.f};
        this->tint = tint;
    }
    
    void Start() override {
        Entity::Start();
    }
    
    void Update(float deltaTime) override {
        Entity::Update(deltaTime);
    }
    
    void Draw() override {
        DrawTexturePro(
            texture,
            source,
            Rectangle{position.x, position.y, source.width, source.height},
            origin,
            rotation,
            tint
        );
        
        Entity::Draw();
    }
    
    ~Sprite() {
        UnloadTexture(texture);
    }
};







// PROTO:

class SineBasic
{
private:
    
public:
    bool active = true;
    bool visible = true;
    
    SineBasic() {
        
    }
    
    virtual void update(float dt) {
        
    }
    
    virtual void draw() {
        
    }
    
    virtual void destroy() {
        active = false;
    }
    
    virtual ~SineBasic() {
        
    }
};

class SineEntity : public SineBasic
{
private:
    /* data */
public:
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    // Deceleration of the entity
    Vector2 drag;
    Vector2 offset;
    Rectangle hitbox;
    float rotation = 0;
    bool solid = true;
    
    SineEntity(float x = 0, float y = 0, float width = 16, float height = 16) {
        position = Vector2{x, y};
        velocity = Vector2{0, 0};
        acceleration = Vector2{0, 0};
        drag = Vector2{0, 0};
        offset = Vector2{0, 0};
        hitbox = Rectangle{x, y, width, height};
    }
    
    void update(float dt) override {
        applyDrag(dt);
        
        velocity.x += acceleration.x * dt;
        velocity.y += acceleration.y * dt;
        
        position.x += velocity.x * dt;
        position.y += velocity.y * dt;
        
        hitbox.x = position.x + offset.x;
        hitbox.y = position.y + offset.y;
    }
    
    void draw() override {
        DrawCircle((int)hitbox.x + (int)hitbox.width/2, (int)hitbox.y + (int)hitbox.width/2, 3, GREEN);
    }
    
    void applyDrag(float dt) {
        if(drag.x != 0) {
            velocity.x = MoveTowards(velocity.x, 0, drag.x * dt);
        }
        if(drag.y != 0) {
            velocity.y = MoveTowards(velocity.y, 0, drag.y * dt);
        }
    }
    
    // Sets the offset for the hitbox
    void setOffset(float x, float y) {
        offset = Vector2{x, y};
    }
    
    // Increases or decreases a value until the end by an amount (usually with deltaTime)
    float MoveTowards(float start, float end, float amount) {
        if(start < end) return std::fmin(start + amount, end);
        if(start > end) return std::fmax(start - amount, end);
        return end;
    }
    
    // Sets the hibox width and height
    void setHitboxSize(float width, float height) {
        hitbox.width = width; hitbox.height = height;
    }
    
    ~SineEntity() {
        
    }
};

class SineSprite : public SineEntity
{
private:
    bool debug_mode = false;
public:
    Texture2D texture;
    Vector2 scale;
    Color tint;
    bool hasTexture = false;
    
    SineSprite(float x, float y) : SineEntity(x, y) {
        scale = Vector2{1, 1};
        tint = WHITE;
    }
    
    // Loads a texture and sets the hitbox width and height to the texture size
    void loadTexture(const char* texture_path) {
        texture = LoadTexture(texture_path);
        hitbox.width = texture.width;
        hitbox.height = texture.height;
        hasTexture = true;
    }
    
    void setScale(float x, float y) {
        scale = Vector2{x, y};
    }
    
    void draw() override {
        if(hasTexture) {
            Rectangle source = Rectangle{0, 0, (float)texture.width, (float)texture.height};
            Rectangle dest = Rectangle{position.x, position.y, (float)texture.width * scale.x, (float)texture.height * scale.y};
            Vector2 origin = Vector2{0, 0};
            
            DrawTexturePro(
                texture,
                source,
                dest,
                origin,
                rotation,
                tint
            );
        }
        
        if(IsKeyPressed(KEY_T)) {
            debug_mode = !debug_mode;
        }
        
        if(debug_mode) {
            // DrawRectangleRec(hitbox, RED);
            DrawRectangleLinesEx(hitbox, 1, RED);
        }
    }
    
    ~SineSprite() {
        UnloadTexture(texture);
    }
};

class SineGroup : public SineBasic
{
private:
    
public:
    std::vector<SineBasic*> members;

    SineGroup() {
        
    }
    
    // NOTE: always create objects with 'new' when adding to a Group
    void add(SineBasic* obj) {
        members.push_back(obj);
    }
    
    void remove(SineBasic* obj) {
        auto it = std::find(members.begin(), members.end(), obj);
        if(it != members.end()) {
            delete *it;
            members.erase(it);
        }
    }
    
    void update(float dt) override {
        for(auto* obj : members) {
            if(obj && obj->active) {
                obj->update(dt);
            }
        }
    }
    
    void draw() override {
        for(auto* obj : members) {
            if(obj && obj->active && obj->visible) {
                obj->draw();
            }
        }
    }
    
    ~SineGroup() {
        for(auto* m : members) delete m;
        members.clear();
    }
};

class SineStateManager;

class SineState : public SineGroup
{
private:
    
public:
    SineStateManager* manager;
    int stateIndex;
    
    virtual void start() {
        
    }
    
    virtual void update(float dt) {
        SineGroup::update(dt);
    }
    
    virtual void draw() {
        SineGroup::draw();
    }
    
    ~SineState() {
        
    }
};

// Stores a unique pointer and a factory function (lambda function)
struct StoredState {
    std::unique_ptr<SineState> instance;
    std::function<std::unique_ptr<SineState>()> recreate;
    int stateIndex;
};

class SineStateManager
{
private:
    std::vector<StoredState> states;
public:
    SineStateManager() {}
    int num_of_states = 0;
    
    void update(float dt) {
        if(states[0].instance) states[0].instance->update(dt);
    }
    
    void draw() {
        if(states[0].instance) states[0].instance->draw();
    }
    
    template<typename T>
    void add(std::unique_ptr<T> state) {
        static_assert(std::is_base_of<SineState, T>::value, "T must inherit from State");

        num_of_states++;
        state->manager = this;

        StoredState stored;
        stored.stateIndex = num_of_states;
        state->stateIndex = num_of_states;
        stored.instance = std::move(state);
        
        // Here it assigns the lambda function to the StoredState (it tells the recreate to be a function that does what's between {})
        stored.recreate = []() {
            return std::make_unique<T>();
        };

        states.push_back(std::move(stored));

        if (num_of_states == 1)
            states[0].instance->start();
    }
    
    void SwitchState(int state_index) {
        if (!states[0].instance) return;

        // Refresh state[0] using its own factory
        states[0].instance = states[0].recreate();
        states[0].instance->manager = this;
        states[0].instance->stateIndex = states[0].stateIndex;

        for (int i = 0; i < states.size(); i++) {
            if (states[i].stateIndex == state_index) {
                std::swap(states[0], states[i]);
                break;
            }
        }

        states[0].instance->start();
    }
    
    void UnloadStates() {
        // TODO
    }
    
    ~SineStateManager() {}
};

inline void DrawLetterBox(RenderTexture2D target, float scale, float gameW, float gameH) {
    BeginDrawing();
        ClearBackground(BLACK);
        
        DrawTexturePro(target.texture, Rectangle{ 0.0f, 0.0f, (float)target.texture.width, (float)-target.texture.height },
            Rectangle{ (GetScreenWidth() - ((float)gameW*scale))*0.5f, (GetScreenHeight() - ((float)gameH*scale))*0.5f,
            (float)gameW*scale, (float)gameH*scale }, Vector2{ 0, 0 }, 0.0f, WHITE);
    EndDrawing();
}
