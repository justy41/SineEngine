#pragma once
#include <iostream>
#include <vector>
#include <string>
#include <cstring>
#include <cmath>
#include <memory>
#include <typeindex>
#include <functional>
#include <algorithm>
#include <unordered_map>
#include <utility>
#include <LDtkLoader/Project.hpp>
#include "raylib.h"
#include "raymath.h"

inline int gameWidth = 640, gameHeight = 360;

inline void InitSineWindow(int screenW, int screenH, int gameW, int gameH, const char* window_name, unsigned int configFlags = 0, unsigned int windowStateFlags = 0, int fps = 60){
    SetConfigFlags(configFlags);
    InitWindow(screenW, screenH, window_name);
    SetWindowState(windowStateFlags);
    
    gameWidth = gameW;
    gameHeight = gameH;
    
    SetTargetFPS(fps);
}

// Stupid std::pair<float, float> hash function because C++ is too stupid to process anything...
struct FloatPairHash {
    std::size_t operator()(const std::pair<float, float>& p) const {
        std::size_t h1 = std::hash<float>{}(p.first);
        std::size_t h2 = std::hash<float>{}(p.second);
        // Combine hashes (e.g., using boost-style hash combine)
        return h1 ^ (h2 << 1);
    }
};

// Stupid Rectangle hash function because C++ is too stupid to process anything... again
struct RectangleHash {
    std::size_t operator()(const Rectangle& rect) const {
        std::size_t hx = std::hash<float>{}(rect.x);
        std::size_t hy = std::hash<float>{}(rect.y);
        std::size_t hw = std::hash<float>{}(rect.width);
        std::size_t hh = std::hash<float>{}(rect.height);

        // Combine hashes using a simple hash combining technique
        return ((hx ^ (hy << 1)) >> 1) ^ (hw << 1) ^ (hh << 2);
    }
};

class SineState;

class SineBasic
{
private:
    
public:
    bool active = true;
    bool visible = true;
    Camera2D* camera;
    SineState* parent_state;
    
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

inline std::vector<Vector2> NEIGHBOUR_OFFSETS = {
    Vector2{-1, 1},
    Vector2{0, 1},
    Vector2{1, 1},
    Vector2{-1, 0},
    Vector2{0, 0},
    Vector2{1, 0},
    Vector2{-1, -1},
    Vector2{0, -1},
    Vector2{1, -1}
};

class SineGroup : public SineBasic
{
private:
    
public:
    std::vector<SineBasic*> members;

    SineGroup() {
        
    }
    
    // Adds a heap allocated object in a std::vector<SineBasic*>
    //
    // NOTE: always create objects with 'new' when adding to a Group
    virtual void add(SineBasic* obj) {
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
        // std::cout<<"\n\n CLEARING MEMBERS \n\n";
    }
};

class SineStateManager;

class SineState : public SineGroup
{
private:
    ldtk::IntRect r;
    std::unordered_map<std::string, Texture2D> tilesets;
    bool ldtk_debug;
public:
    SineStateManager* manager;
    int stateIndex;
    Vector2 VirtualMousePosition;
    float scale = 0;
    float offsetX, offsetY;
    
    Camera2D camera;
    
    ldtk::Project ldtkProject;
    const ldtk::World* world;
    const ldtk::Level* level_0;
    const ldtk::Layer* ground_layer;
    float tile_size = 0;
    std::unordered_map<std::pair<float, float>, bool, FloatPairHash> collisions_layer;
    std::unordered_map<std::string, Rectangle> entities;
    
    // Adds a heap allocated object in a std::vector<SineBasic*>
    //
    // NOTE: always create objects with 'new' when adding to a Group
    virtual void add(SineBasic* obj) {
        obj->camera = &camera;
        obj->parent_state = this;
        SineGroup::add(obj);
    }
    
    // Runs once when the State is loaded
    //
    // NOTE: put this as the first line if the function is overriden as SineState::start()
    virtual void start() {
        camera = {0};
        camera.target = Vector2{gameWidth/2.f, gameHeight/2.f};
        camera.offset = Vector2{gameWidth/2.f, gameHeight/2.f};
        camera.rotation = 0;
        camera.zoom = 1;
    }
    
    // Runs every frame
    //
    // NOTE: put this as the first line if the function is overriden as SineState::update(dt)
    virtual void update(float dt) {
        scale = std::min((float)GetScreenWidth()/gameWidth, (float)GetScreenHeight()/gameHeight);
        offsetX = ((float)GetScreenWidth() - (gameWidth * scale)) * 0.5f;
        offsetY = ((float)GetScreenHeight() - (gameHeight * scale)) * 0.5f;
        VirtualMousePosition.x = ((GetMouseX() - offsetX) / scale);
        VirtualMousePosition.y = ((GetMouseY() - offsetY) / scale);
        
        SineGroup::update(dt);
    }
    
    // Runs every frame
    //
    // NOTE: put this depending of the draw order as SineState::draw()
    virtual void draw() {
        SineGroup::draw();
    }
    
    // Make the camera follow a position
    void CameraFollow(Vector2 pos) {
        camera.target = Vector2{std::round(pos.x), std::round(pos.y)};
    }
    
    // Loads a LDtk map
    //
    // NOTE: tilesets are loaded relative to the file path of the .ldtk file. File paths can be found in the .ldtk file.
    // The tileset paths are printed in the command line to see.
    //
    // NOTE: in ldtk the entities need to have a CUSTOM FIELD called "Name" <- exactly written like this for it to work
    void LoadLDtkMap(const char* tilemap_path, float fixed_tile_size, std::vector<std::string> collision_layer_names) {
        ldtkProject.loadFromFile(tilemap_path);
        world = &ldtkProject.getWorld();
        
        tile_size = fixed_tile_size;
        for(const auto& level : world->allLevels()) {
            if(!collision_layer_names.empty()) {
                for(auto name : collision_layer_names) {
                    for(const auto& tile : level.getLayer(name).allTiles()) {
                        collisions_layer.insert({std::make_pair(tile.getGridPosition().x + level.position.x/tile_size, tile.getGridPosition().y + level.position.y/tile_size), true});
                    }
                }
            }
            else break;
        }
        
        // This block of code takes the tilemap_path and erases the map.ldtk part.
        // After that, the tileset path from the ldtk layer is appended to it, forming the path to the tileset.
        // EXAMPLE: tilemaps/map.ldtk + ../tilesets/tileset_1.png = tilemaps/../tilesets/tileset_1.png
        //
        // The entities are also extracted in an unordered_map for later use.
        for(const auto& level : world->allLevels()) {
            for(const auto& layer : level.allLayers()) {
                if(layer.getType() != ldtk::LayerType::Entities) {
                    if(tilesets.find(layer.getTileset().name) == tilesets.end()) {
                        std::string texture_file_name = layer.getTileset().path; // Load file path relative to the .ldtk file
                        std::string map_path = tilemap_path;
                        for(int i = map_path.size()-1; i>=0; i--) {
                            if(map_path[i-1] == '/') {      // When the '/' is next...
                                map_path.erase(i);          // ...then erase.
                                break;
                            }
                        }
                        
                        map_path.append(texture_file_name); // Combine the two file paths.
                        tilesets.insert({layer.getTileset().name, LoadTexture(map_path.c_str())}); // Insert the name and load the tileset.
                        std::cout<<"\nTILESET PATH:\n"<<map_path<<"\n\n";
                    }
                }
                else {
                    // Saving ldtk entities as an element with Name, Position and Size in an unordered_map
                    for(const auto& ent : layer.allEntities()) {
                        entities.insert({
                            ent.getField<std::string>("Name").value(),
                            Rectangle{
                                (float)ent.getPosition().x + level.position.x,
                                (float)ent.getPosition().y + level.position.y,
                                (float)ent.getSize().x,
                                (float)ent.getSize().y
                            }
                        });
                    }
                }
            }
        }
    }
    
    Rectangle getLDtkEntity(std::string Name_field) {
        Rectangle rect = Rectangle{0, 0, 0, 0};
        if(entities[Name_field].width != 0) {
            rect = Rectangle{entities[Name_field].x, entities[Name_field].y, entities[Name_field].width, entities[Name_field].height};
            return rect;
        }
        return rect;
    }
    
    // Draws the entire LDtk map
    void DrawLDtkMap() {
        for(const auto& level : world->allLevels()) {
            for(int i = level.allLayers().size()-1; i>=0; i--) { // Reversed order because LDtkLoader takes the layers inverted.
                if(level.allLayers()[i].getType() != ldtk::LayerType::Entities) {
                    std::string ts_name = level.allLayers()[i].getTileset().name; // Get the tileset name of the current layer.
                    if(level.allLayers()[i].isVisible()) {
                        for(const auto& tile : level.allLayers()[i].allTiles()) { // Draw all tiles based on their source rect.
                            r = tile.getTextureRect(); // source rect
                            DrawTextureRec(
                                tilesets[ts_name],
                                Rectangle{(float)r.x, (float)r.y, (float)r.width, (float)r.height},
                                Vector2{(float)tile.getPosition().x + level.position.x, (float)tile.getPosition().y + level.position.y},
                                WHITE
                            );
                        }
                    }
                }
            }
        }
    }
    
    // Draws only the named level
    void DrawLDtkLevel(const char* level_name) {
        for(int i = world->getLevel(level_name).allLayers().size()-1; i>=0; i--) {
            if(world->getLevel(level_name).allLayers()[i].getType() != ldtk::LayerType::Entities) {
                std::string ts_name = world->getLevel(level_name).allLayers()[i].getTileset().name;
                if(world->getLevel(level_name).allLayers()[i].isVisible()) {
                    for(const auto& tile : world->getLevel(level_name).allLayers()[i].allTiles()) {
                        r = tile.getTextureRect();
                        DrawTextureRec(
                            tilesets[ts_name],
                            Rectangle{(float)r.x, (float)r.y, (float)r.width, (float)r.height},
                            Vector2{(float)tile.getPosition().x, (float)tile.getPosition().y},
                            WHITE
                        );
                    }
                }
            }
        }
    }
    
    // Draws a layer from all levels (exception is entities layer)
    void DrawLDtkLayer(const char* layer_name) {
        for(const auto& level : world->allLevels()) {
            std::string ts_name = level.getLayer(layer_name).getTileset().name;
            if(level.getLayer(layer_name).isVisible()) {
                for(const auto& tile : level.getLayer(layer_name).allTiles()) {
                    r = tile.getTextureRect();
                    DrawTextureRec(
                        tilesets[ts_name],
                        Rectangle{(float)r.x, (float)r.y, (float)r.width, (float)r.height},
                        Vector2{(float)tile.getPosition().x + level.position.x, (float)tile.getPosition().y + level.position.y},
                        WHITE
                    );
                }
            }
        }
    }
    
    // Draw the LDtk map collision hitboxes if F6 key is pressed
    void DrawLDtkCollisionLayers() {
        if(IsKeyPressed(KEY_F6)) {
            ldtk_debug = !ldtk_debug;
        }
        
        if(ldtk_debug && !collisions_layer.empty()) {
            for(auto tile : collisions_layer) {
                DrawRectangleLinesEx(Rectangle{tile.first.first*tile_size, tile.first.second*tile_size, tile_size, tile_size}, 2, RED);
            }
        }
    }
    
    // ===================================================== LDTK MAP COLLISIONS ===================================================== //
    std::vector<Vector2> tiles_around(Vector2 pos, float tile_size, std::unordered_map<std::pair<float, float>, bool, FloatPairHash> collisions_layer) {
        std::vector<Vector2> tiles;
        Vector2 tile_loc = Vector2{std::floor(pos.x / tile_size), std::floor(pos.y / tile_size)};
        for(auto offset : NEIGHBOUR_OFFSETS) {
            Vector2 check_loc = Vector2{tile_loc.x + offset.x, tile_loc.y + offset.y};
            if(collisions_layer[std::make_pair(check_loc.x, check_loc.y)] == true) {
                tiles.push_back(check_loc);
            }
        }
        
        return tiles;
    }
    
    std::vector<Rectangle> physics_rects_around(Vector2 pos) {
        std::vector<Rectangle> rects;
        for(auto tile : tiles_around(pos, tile_size, collisions_layer)) {
            rects.push_back(Rectangle{tile.x*tile_size, tile.y*tile_size, tile_size, tile_size});
        }
        return rects;
    }
    
    /// @brief Gets the virtual X mouse position, dependend on the scale of the window
    /// @return 
    float GetVirtualMouseX() {
        return VirtualMousePosition.x;
    }
    /// @brief Gets the virtual Y mouse position, dependend on the scale of the window
    /// @return 
    float GetVirtualMouseY() {
        return VirtualMousePosition.y;
    }
    /// @brief Gets the virtual mouse position, dependend on the scale of the window
    /// @return 
    Vector2 GetVirtualMousePosition() {
        return VirtualMousePosition;
    }
    
    ~SineState() {
        
    }
};

class SineEntity : public SineBasic
{
private:
    
public:
    Vector2 position;
    Vector2 velocity;
    Vector2 acceleration;
    // Deceleration of the entity
    Vector2 drag;
    Vector2 offset;
    float gravity;
    Rectangle hitbox;
    float rotation = 0;
    bool solid = true;
    
    std::unordered_map<std::string, bool> collisions;
    
    SineEntity(float x = 0, float y = 0, float width = 16, float height = 16) {
        position = Vector2{x, y};
        velocity = Vector2{0, 0};
        acceleration = Vector2{0, 0};
        drag = Vector2{0, 0};
        offset = Vector2{0, 0};
        gravity = 0;
        hitbox = Rectangle{x, y, width, height};
        
        collisions.insert({{"right", false}, {"left", false}, {"down", false}, {"up", false}});
    }
    
    void update(float dt) override {
        collisions["right"] = false;
        collisions["left"] = false;
        collisions["down"] = false;
        collisions["up"] = false;
        
        applyDrag(dt);
        
        velocity.x += acceleration.x * dt;
        position.x += velocity.x * dt; // Update position.x based on velocity.x
        
        // ================================================ COLLISION RESOLUTION X ================================================ //
        hitbox.x = position.x + offset.x;
        if(solid) {
            for(Rectangle rect : parent_state->physics_rects_around(position)) {
                if(CheckCollisionRecs(hitbox, rect)) {
                    if(velocity.x > 0) {
                        hitbox.x = rect.x - hitbox.width;
                        collisions["right"] = true;
                    }
                    if(velocity.x < 0) {
                        hitbox.x = rect.x + rect.width;
                        collisions["left"] = true;
                    }
                    position.x = hitbox.x - offset.x;
                }
            }
        }
        
        if(collisions["right"] || collisions["left"]) {
            velocity.x = 0;
        }
        // ======================================================================================================================== //
        
        acceleration.y = gravity; // Y acceleration is basically gravity
        velocity.y += acceleration.y * dt;
        position.y += velocity.y * dt; // Update position.y based on velocity.y
        
        // ================================================ COLLISION RESOLUTION Y ================================================ //
        hitbox.y = position.y + offset.y;
        if(solid) {
            for(Rectangle rect : parent_state->physics_rects_around(position)) {
                if(CheckCollisionRecs(hitbox, rect)) {
                    if(velocity.y > 0) {
                        hitbox.y = rect.y - hitbox.height;
                        collisions["down"] = true;
                    }
                    if(velocity.y < 0) {
                        hitbox.y = rect.y + rect.height;
                        collisions["up"] = true;
                    }
                    position.y = hitbox.y - offset.y;
                }
            }
        }
        
        if(collisions["down"] || collisions["up"]) {
            velocity.y = 0;
        }
        // ======================================================================================================================== //
    }
    
    void draw() override {
        DrawCircle((int)hitbox.x + (int)hitbox.width/2, (int)hitbox.y + (int)hitbox.height/2, 3, GREEN);
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
            DrawCircle(position.x, position.y, 2, GREEN);
            SineEntity::draw();
        }
    }
    
    ~SineSprite() {
        UnloadTexture(texture);
    }
};

inline bool overlap(SineEntity* entA, SineEntity* entB) {
    if(CheckCollisionRecs(entA->hitbox, entB->hitbox)) {
        return true;
    }
    return false;
}

inline bool overlap(SineEntity* ent, SineGroup* group) {
    for(auto entity : group->members) {
        if(entity->active && ent->active) {
            SineEntity* e = dynamic_cast<SineEntity*>(entity);
            if(CheckCollisionRecs(ent->hitbox, e->hitbox)) {
                return true;
            }
        }
    }
    return false;
}

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
    
    void start() {
        if(states[0].instance) states[0].instance->start();
    }
    
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
        // TODO: not really necessary
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
