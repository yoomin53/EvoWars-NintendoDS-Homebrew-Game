//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#ifndef SHOP_HPP
#define SHOP_HPP

#include <nds.h>
#include <nf_lib.h>
#include <filesystem.h>
#include "Save.h"


class Shop {
public:
    Shop();
    ~Shop();//destructor
    int shopFinished = false; 

    void Init(SaveData* data);
    void Update();
    void Draw();
    bool isFinished() const; 
    void cleanUp(SaveData* data); 

    
private:
    void LoadBackgrounds();
    void LoadSprites();
    void UpdateSelection();
    void UpdateSpriteSizes();
    void UpdateText();
    void UpdateBackground(int level); 

private:
    int gold; 
    int attack_level;
    int accuracy_level;  
    int agility_level; 
    int critical_chance_level; 

    int selectedIndex;

    static const int SHOP_SPRITE_COUNT = 4;

    // Sprite IDs
    int spriteIds[SPRITE_COUNT];

    // Positions
    int spriteX[SPRITE_COUNT];
    int spriteY[SPRITE_COUNT];

    // Sizes
    static const int SMALL_SCALE = 128;   // 1x scale (fixed point 128 = normal)
    static const int LARGE_SCALE = 290;   // 
};

#endif
