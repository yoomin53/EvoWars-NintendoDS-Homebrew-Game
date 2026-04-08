//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#pragma once

#include <nds.h>
#include <NEMain.h>
#include "bgm.h"
#include "soundbank.h"

class MainMenu {
public:
    MainMenu();

    void init();     // Initialize 2D + 3D systems
    void update();   // Handle input and logic
    void render();   // Render 2D + 3D
    bool isFinished();
    void cleanUp(); // Free resources
    int selectedButton; //0: story, 1: endless, 2: multiplayer, 3: settings
    int seed = 0; 
    int randNum; 
private:
    bool finished; 
    bool confirmed = false; 
    bool showSavePrompt = false;
}; 
