//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#pragma once

#include <nds.h>
#include <NEMain.h>

class TitleScreen {
public:
    TitleScreen();

    void init();     // Initialize 2D + 3D systems
    void update();   // Handle input and logic
    void render();   // Render 2D + 3D
    bool isFinished() const;
    void cleanUp(); // Free resources
    int seed = 0; 

private:
    bool finished;
    int textY;
    int textDir; 

    NE_Camera* camera;
    NE_Model* model;
    NE_Material* material;
    static void drawScene(void* arg);  
}; 
