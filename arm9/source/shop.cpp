//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "Shop.hpp"
#include "bgm.h"
#include "soundbank.h"

Shop::Shop() {
    selectedIndex = 0;
}

Shop::~Shop() {}

void Shop::Init(SaveData* data) {
    // Initialize 2D systems
    NF_Set2D(0, 0); //Top screen
    NF_Set2D(1, 0); //Bottom screen
    swiWaitForVBlank();
    // Initialize NitroFS
    nitroFSInit(NULL);
    //Set root folder for NitroFS
    NF_SetRootFolder("NITROFS");
    //storage buffers
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);
    LoadBackgrounds();

    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0); // init sprite system on the top screen 
    //NF_InitSpriteSys(1);
    LoadSprites();
    UpdateSpriteSizes();

    gold = data->gold; 
    attack_level = data->attack_level;
    accuracy_level = data->accuracy_level;  
    agility_level = data->accuracy_level; 
    critical_chance_level = data->critical_chance_level; 
    //audio
    Audio_Init(); 
    Audio_Load_SFX();
    Audio_PlayBGM("nitro:/audio/viacheslavstarostin-jazz-lounge-elevator-music.wav", true); 


    //--- Text16 Initialization ---
    NF_InitTextSys(0);
    NF_InitTextSys(1);
    NF_LoadTextFont16("fnt/font16", "topfont", 256, 256, 0);    
    NF_CreateTextLayer16(0, 0, 0, "topfont");
    NF_CreateTextLayer16(1, 0, 0, "topfont");//screen, layer, rotation, name 
    //NF_DefineTextColor(1, 0, 1, 31, 0, 0); // Red
    //NF_SetTextColor(1, 0, 1);
    NF_UpdateTextLayers();
    UpdateSelection(); 
    UpdateBackground(attack_level); 
}

void Shop::LoadBackgrounds() {
    NF_LoadTiledBg("bg/shopTop", "Top", 256, 256);
    NF_LoadTiledBg("bg/shopBottomUpgrade", "BottomUp", 256, 256);
    NF_LoadTiledBg("bg/shopBottomMax", "BottomMax", 256, 256);

    //NF_CreateTiledBg(1, 2, "BottomMax"); //screen 1, bg 3
    NF_CreateTiledBg(1, 3, "BottomUp");
    NF_CreateTiledBg(0, 3, "Top"); 
    NF_ScrollBg(1, 3, 0, 64);
}

void Shop::LoadSprites() {

    // Load sprite graphics (must be in /sprite folder)
    NF_LoadSpriteGfx("sprite/Shop/attack", 0, 64, 64);
    NF_LoadSpritePal("sprite/Shop/attack", 0);

    NF_LoadSpriteGfx("sprite/Shop/agility", 1, 64, 64);
    NF_LoadSpritePal("sprite/Shop/agility", 1);

    NF_LoadSpriteGfx("sprite/Shop/accuracy", 2, 64, 64);
    NF_LoadSpritePal("sprite/Shop/accuracy", 2);

    NF_LoadSpriteGfx("sprite/Shop/criticalChance", 3, 64, 64);
    NF_LoadSpritePal("sprite/Shop/criticalChance", 3);
    /*
    NF_LoadSpriteGfx("sprite/test", 5, 64, 64); 
    NF_LoadSpritePal("sprite/test", 5);
    //Send to Vram 
    NF_VramSpriteGfx(1, 5, 5, false);
    NF_VramSpritePal(1, 5, 5);

    NF_CreateSprite(1, 5, 5, 5, 150, 0);

    NF_SpriteFrame(1, 5, 1);
    */

    //NF_LoadSpriteGfx("sprite/Shop/edge", 3, 64, 64);
    //NF_LoadSpritePal("sprite/Shop/edge", 3);

    for (int i = 0; i < SHOP_SPRITE_COUNT; i++) {
        NF_VramSpriteGfx(0, i, i, false);
        NF_VramSpritePal(0, i, i);

        spriteIds[i] = i;

        spriteX[i] = -27 + (i * 60);
        spriteY[i] = 40;

        NF_CreateSprite(0, spriteIds[i], i, i, spriteX[i], spriteY[i]);
        NF_ShowSprite(0, spriteIds[i], true);
        NF_SpriteLayer(0, spriteIds[i], 0); 
        NF_EnableSpriteRotScale(0, spriteIds[i], i, true);
    }
}

#define MAX_ATTACK 10 
#define MAX_AGILITY 2
#define MAX_ACCURACY 10 
#define MAX_CRITICAL 10
#define MAXLEVEL 10 
#define BTN_UPGRADE_X1  0
#define BTN_UPGRADE_Y1  195-50 //adjust for scroll (-56)
#define BTN_UPGRADE_X2  255
#define BTN_UPGRADE_Y2  195

#define BTN_BACK_X1  180
#define BTN_BACK_Y1  0 //adjust for scroll (-56)
#define BTN_BACK_X2  255
#define BTN_BACK_Y2  30
void Shop::Update() {

    scanKeys();
    int keys = keysDown();
    touchPosition touch;
    touchRead(&touch);
    Audio_Update();

    if (keys & KEY_RIGHT) {
        selectedIndex++;
        if (selectedIndex >= SHOP_SPRITE_COUNT)
            selectedIndex = 0;
        Audio_PlaySFX(SFX_SELECT);
        UpdateSelection();
    }
    else if (keys & KEY_LEFT) {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = SHOP_SPRITE_COUNT - 1;
        Audio_PlaySFX(SFX_SELECT);
        UpdateSelection(); 
    }
    if (keys & KEY_TOUCH) {
        if (touch.px >= BTN_UPGRADE_X1 && touch.px <= BTN_UPGRADE_X2 &&
            touch.py >= BTN_UPGRADE_Y1 && touch.py <= BTN_UPGRADE_Y2) {
            switch (selectedIndex){
                case 0:
                    if(gold >= attack_level*200 && attack_level <= MAX_ATTACK){
                        gold -= attack_level*200;
                        attack_level++;   
                        Audio_PlaySFX(SFX_SUCCESSFUL);
                    } else Audio_PlaySFX(SFX_FAIL);
                    UpdateBackground(attack_level);
                    break; 
                case 1: 
                    if(gold >= agility_level*1000 && agility_level <= MAX_AGILITY){
                        gold -= agility_level*1000;
                        agility_level++;
                        Audio_PlaySFX(SFX_SUCCESSFUL);
                    } else Audio_PlaySFX(SFX_FAIL);
                    UpdateBackground(agility_level);
                    break; 
                case 2: 
                    if(gold >= accuracy_level*200 && accuracy_level <= MAX_ACCURACY){
                        gold -= accuracy_level*200;
                        accuracy_level++;
                        Audio_PlaySFX(SFX_SUCCESSFUL);
                    } else Audio_PlaySFX(SFX_FAIL);
                    UpdateBackground(accuracy_level);  
                    break;
                case 3: 
                    if (gold >= critical_chance_level*200 && critical_chance_level <= MAX_CRITICAL){
                        gold -= critical_chance_level*200;
                        critical_chance_level++;
                        Audio_PlaySFX(SFX_SUCCESSFUL);
                    } else Audio_PlaySFX(SFX_FAIL);
                    UpdateBackground(critical_chance_level);  
                    break;
            }
        }
        if (touch.px >= BTN_BACK_X1 && touch.px <= BTN_BACK_X2 &&
            touch.py >= BTN_BACK_Y1 && touch.py <= BTN_BACK_Y2) {
            Audio_PlaySFX(SFX_FAIL);
            shopFinished = true; 
        }
        UpdateSelection(); 
    }
    //NF_SpriteOamSet(1);
    NF_SpriteOamSet(0);
    swiWaitForVBlank();
    //oamUpdate(&oamSub);
    oamUpdate(&oamMain);
    //NF_UpdateTextLayers();

}

void Shop::UpdateSelection() {
    UpdateSpriteSizes();
    UpdateText(); 
}

void Shop::UpdateSpriteSizes() {

    for (int i = 0; i < SHOP_SPRITE_COUNT; i++) {

        if (i == selectedIndex) {
            NF_SpriteRotScale(0, spriteIds[i], i, LARGE_SCALE, LARGE_SCALE);
        } else {
            NF_SpriteRotScale(0, spriteIds[i], i, SMALL_SCALE, SMALL_SCALE);
        }
    }
}

void Shop::UpdateText() {
    NF_ClearTextLayer16(0, 0);
    char goldString[256]; 
    sprintf(goldString, "GOLD: %d", gold); 
    NF_WriteText16(0, 0, 22, 1, goldString); //screen, layer x position, y postion
    NF_UpdateTextLayers();
    NF_ClearTextLayer16(1, 0);

    char description[256];
    switch (selectedIndex){
    case 0: 
        if (attack_level < MAXLEVEL){ 
            sprintf(description, "Increase Attack, current level: %d, max level %d, cost: %d", attack_level, MAX_ATTACK, attack_level*200); 
            NF_WriteText16(1, 0, 3, 2, description); 
        }
        else
            NF_WriteText16(1, 0, 3, 2, "max attack level reached");
        UpdateBackground(attack_level); 
        break; 
    case 1: 
        if (agility_level < MAX_AGILITY){
            sprintf(description, "Increase Speed, current level: %d, max level %d, cost: %d", agility_level, MAX_AGILITY, agility_level*1000); 
            NF_WriteText16(1, 0, 3, 2, description); 
        } 
        else
            NF_WriteText16(1, 0, 3, 2, "max agility level reached");
        UpdateBackground(agility_level); 
        break; 
    case 2: 
        if (accuracy_level < MAXLEVEL){
            sprintf(description, "Increase AIM, current level: %d, max level %d, cost: %d", accuracy_level, MAX_ACCURACY, accuracy_level*200); 
            NF_WriteText16(1, 0, 3, 2, description); 
        }
        else
            NF_WriteText16(1, 0, 3, 2, "max AIM level reached");
        UpdateBackground(accuracy_level); 
        break; 
    case 3:
        if (critical_chance_level < MAXLEVEL){ 
            sprintf(description, "Increase Critical Chance & attack, current level: %d, max level %d, cost: %d", critical_chance_level, MAX_CRITICAL, critical_chance_level*200); 
            NF_WriteText16(1, 0, 3, 2, description); 
        }
        else
            NF_WriteText16(1, 0, 3, 2, "max critical level reached");
        UpdateBackground(critical_chance_level); 
        break; 
    }
    NF_UpdateTextLayers();
}
void Shop::UpdateBackground(int level) {
    swiWaitForVBlank(); 
    if(level == MAXLEVEL){ 
        NF_DeleteTiledBg(1, 3);
        NF_CreateTiledBg(1, 3, "BottomMax");
    } 
    else{
        NF_DeleteTiledBg(1, 3);
        NF_CreateTiledBg(1, 3, "BottomUp");
    }
    NF_ScrollBg(1, 3, 0, 64);
} 
void Shop::Draw() {
    // Reserved if you want custom drawing later
}

bool Shop::isFinished() const { return shopFinished;}

void Shop::cleanUp(SaveData* data) {
    data->gold = gold;  
    data->attack_level = attack_level;
    data->accuracy_level = accuracy_level;  
    data->agility_level = accuracy_level; 
    data->critical_chance_level = critical_chance_level;
    Audio_StopBGM();  
    Audio_Unload_SFX();
    swiWaitForVBlank();
    shopFinished = false; 
    NF_ClearTextLayer16(0, 0);
    NF_ClearTextLayer16(1, 0);
    NF_InitTextSys(0);
    NF_InitTextSys(1);
    NF_DeleteTiledBg(0, 3);
    NF_DeleteTiledBg(1, 3);
    NF_UnloadTextFont("topfont");
    NF_UnloadTiledBg("Top");
    NF_UnloadTiledBg("BottomUp");
    NF_UnloadTiledBg("BottomMax");
    NF_ResetSpriteBuffers(); 
    NF_ResetTiledBgBuffers();
    NF_InitSpriteSys(0); 
    NF_InitSpriteSys(1); 
    //vramSetBankA(VRAM_A_LCD);
    //vramSetBankB(VRAM_B_LCD);
    //vramSetBankC(VRAM_C_LCD);
    //vramSetBankD(VRAM_D_LCD);
    //dmaFillHalfWords(0, (void*)0x06000000, 128 * 1024 * 4); // clear VRAM A–D region
} 
