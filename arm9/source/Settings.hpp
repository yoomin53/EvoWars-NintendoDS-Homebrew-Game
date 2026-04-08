//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#pragma once
#include "Save.h"

class Settings {
public:
    bool settingsFinished = false;
    bool mute = false; 
    bool debug = false; 
    int SFX = 255; 
    int difficulty = 0; //0 easy, 1 medium, 2 hard

    void init(SaveData* data);
    void update();
    static void draw();
    void end(SaveData* data);
    bool isFinished() const;
};
