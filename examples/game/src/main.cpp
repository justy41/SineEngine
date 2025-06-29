#include <iostream>
#include <cmath>
#include <memory>
#include "sine.h"
#include "TemplateState.h"
#include "SecondState.h"

int main(){
    InitSineWindow(1280, 720, 640, 360, "game", FLAG_WINDOW_RESIZABLE, FLAG_WINDOW_TRANSPARENT);
    RenderTexture2D target = LoadRenderTexture(gameWidth, gameHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
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
