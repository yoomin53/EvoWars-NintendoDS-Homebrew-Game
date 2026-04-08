//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>
#include "Menu.hpp"

bool MainMenu::isFinished() { if (finished){finished = false; return true;} else{return finished;}}

MainMenu::MainMenu() : finished(false), selectedButton(0) {} // constructor

void MainMenu::init() { 
    // Initialize 2D systems
    NF_Set2D(0, 0); //Top screen
    NF_Set2D(1, 0); //Bottom screen
    swiWaitForVBlank();
    // Initialize NitroFS
    nitroFSInit(NULL);
    //Set root folder for NitroFS
    NF_SetRootFolder("NITROFS");

    NF_Set2D(0, 5); //Top screen: mode 5: Bitmap 16 bit color, can rotate/zoom 
    NF_Set2D(1, 0); //Bottom screen mode 0: Tiled backgrounds + text
    //--- Top Screen Initialization ---
    //Initialize bitmap background systems
    NF_InitBitmapBgSys(0, 1);
    //storage buffers
    NF_Init16bitsBgBuffers();
    //--- Bottom Screen Initialization ---
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(1);
    //Random Menu theme
    srand(seed); 
    randNum = rand()%10; 
    if(randNum < 3){
        //Load bitmap TOP background files from NitroFS
        NF_Load16bitsBg("bmp/MainMenuAidenTop", 0);
        //Load Tiled Bottom background files 
        NF_LoadTiledBg("bg/MainMenuAidenBottom", "menu", 256, 256);
        NF_CreateTiledBg(1, 3, "menu"); //screen 1, bg 3
        NF_ScrollBg(1, 3, 0, 56); //scroll bg to up
    } else if(randNum < 6) {
        //Load bitmap TOP background files from NitroFS
        NF_Load16bitsBg("bmp/MainMenuLeiaTop", 0);
        //Load Tiled Bottom background files 
        NF_LoadTiledBg("bg/MainMenuLeiaBottom", "menu", 256, 256);
        NF_CreateTiledBg(1, 3, "menu"); //screen 1, bg 3
        NF_ScrollBg(1, 3, 0, 63); //scroll bg to up
    } else{
        //Load bitmap TOP background files from NitroFS
        NF_Load16bitsBg("bmp/MainMenuLinaTop", 0);
        //Load Tiled Bottom background files 
        NF_LoadTiledBg("bg/MainMenuLinaBottom", "menu", 256, 256);
        NF_CreateTiledBg(1, 3, "menu"); //screen 1, bg 3
        NF_ScrollBg(1, 3, 0, 63); //scroll bg to up
    } 
    //Transfer TOP image to VRAM 
    NF_Copy16bitsBuffer(0, 0, 0);
    //remove top screen it from Ram
    NF_Unload16bitsBg(0);
    //Load save game layer 
    NF_LoadTiledBg("bg/saveGame", "save", 256, 256);

    //--- Text16 Initialization ---
    NF_InitTextSys(1);
    NF_LoadTextFont16("fnt/font16", "topfont", 256, 256, 0);    
    NF_CreateTextLayer16(1, 0, 0, "topfont");
    //NF_DefineTextColor(1, 0, 1, 31, 0, 0); // Red
    //NF_SetTextColor(1, 0, 1);
    NF_UpdateTextLayers();
    Audio_Init(); 
    Audio_Load_SFX();
}

// Coordinates of buttons drawn in background
#define BTN_STORY_X1  0
#define BTN_STORY_Y1  76-56 //adjust for scroll (-56)
#define BTN_STORY_X2  255
#define BTN_STORY_Y2  115-56

#define BTN_ENDLESS_X1   0
#define BTN_ENDLESS_Y1   116-56
#define BTN_ENDLESS_X2   255
#define BTN_ENDLESS_Y2   155-56

#define BTN_MULTI_X1   0
#define BTN_MULTI_Y1   156-56
#define BTN_MULTI_X2   255
#define BTN_MULTI_Y2   195-56

#define BTN_SETT_X1   0
#define BTN_SETT_Y1   196-56
#define BTN_SETT_X2   255
#define BTN_SETT_Y2   235-56


void MainMenu::update() {
    swiWaitForVBlank();
    scanKeys();
    int pressed = keysDown();
    touchPosition touch;
    touchRead(&touch);

    // =========================
    // SAVE PROMPT ACTIVE
    // =========================
    if (showSavePrompt) {

        // Confirm save
        if (pressed & KEY_A | ((pressed & KEY_TOUCH) && 
        (touch.px >= 20 && touch.px <= 128 && 
            touch.py >= 70 && touch.py <= 128))) {
            // Call your save here (you need access to data pointer)
            // Save_Write(data);
            Audio_PlaySFX(SFX_SUCCESSFUL);

            showSavePrompt = false;
            NF_DeleteTiledBg(1, 2);
            NF_CreateTiledBg(1, 3, "menu");
            if (randNum < 3) NF_ScrollBg(1, 3, 0, 56);
            else NF_ScrollBg(1, 3, 0, 63);
            selectedButton = 4; // special "save exit"
            finished = true;
            return;
        }

        // Cancel
        if (pressed & (KEY_B | KEY_X) | ((pressed & KEY_TOUCH) && 
        (touch.px >= 130 && touch.px <= 248 && 
            touch.py >= 70 && touch.py <= 128))) {
            Audio_PlaySFX(SFX_FAIL);
            showSavePrompt = false;
            NF_DeleteTiledBg(1, 2); // delete save background
            NF_CreateTiledBg(1, 3, "menu");
            if (randNum < 3) NF_ScrollBg(1, 3, 0, 56);
            else NF_ScrollBg(1, 3, 0, 63);
            return;
        }

        return; // block ALL other input
    }

    // =========================
    // OPEN SAVE PROMPT
    // =========================
    if (pressed & (KEY_X | KEY_B)) {
        Audio_PlaySFX(SFX_SELECT);
        showSavePrompt = true;
        NF_DeleteTiledBg(1, 3);
        NF_CreateTiledBg(1, 2, "save");
        return; // prevent input leak
    }
    /*
    #define SFX_CRASH    29
    #define SFX_FAIL    30
    #define SFX_FIRE_EXPLOSION    31
    #define SFX_GUNSHOT    32
    #define SFX_SELECT    33
    #define SFX_SUCCESSFUL
    */
    // --- Touch input ---
    if (pressed & KEY_TOUCH && !showSavePrompt) {
        Audio_PlaySFX(SFX_SUCCESSFUL);
        if (touch.px >= BTN_STORY_X1 && touch.px <= BTN_STORY_X2 &&
            touch.py >= BTN_STORY_Y1 && touch.py <= BTN_STORY_Y2) {
            selectedButton = 0;
            confirmed = true;
            
        }
        else if (touch.px >= BTN_ENDLESS_X1 && touch.px <= BTN_ENDLESS_X2 &&
                 touch.py >= BTN_ENDLESS_Y1 && touch.py <= BTN_ENDLESS_Y2) {
            selectedButton = 1;
            confirmed = true;
            
        }
        else if (touch.px >= BTN_MULTI_X1 && touch.px <= BTN_MULTI_X2 &&
                 touch.py >= BTN_MULTI_Y1 && touch.py <= BTN_MULTI_Y2) {
            selectedButton = 2;
            confirmed = true;
            
        }
        else if (touch.px >= BTN_SETT_X1 && touch.px <= BTN_SETT_X2 &&
                 touch.py >= BTN_SETT_Y1 && touch.py <= BTN_SETT_Y2) {
            selectedButton = 3;
            confirmed = true;
            
        }
    }


    // --- D-Pad input ---
    if (pressed & (KEY_LEFT | KEY_UP)) {
        Audio_PlaySFX(SFX_SELECT);
        if (selectedButton > 0) selectedButton--;
        else selectedButton = 3;
    }
    if (pressed & (KEY_RIGHT | KEY_DOWN)){
        Audio_PlaySFX(SFX_SELECT);
        if (selectedButton < 3) selectedButton++;
        else selectedButton = 0;
    }

    // --- A button to confirm ---
    if (pressed & KEY_A && !showSavePrompt) {
        Audio_PlaySFX(SFX_SUCCESSFUL);
        confirmed = true;
    }
    // --- Handle confirmed selection ---
    if (confirmed && !showSavePrompt) {
        if (selectedButton == 0){
            finished = true; // Start
        }
        else if (selectedButton == 1){
            //NF_DeleteTiledBg(1, 3);
            //NF_ClearTextLayer16(1, 0);
            //consoleDemoInit();
            //printf("Endless not implemented yet!\n");
            finished = true; // Exit or return to title
        }
        else if (selectedButton == 2){
            finished = true; // Exit or return to title
        }
        else if (selectedButton == 3) {
            finished = true; // Exit or return to title
        }
    }

}

void MainMenu::render() {
    // Erase previous symbols
    NF_ClearTextLayer16(1, 0);
    if (!showSavePrompt) {
        if (selectedButton == 0) {
            NF_WriteText16(1, 0, 11, 2, ">");//Story Mode
            NF_ScrollBg(1, 0, 0, -1);
        } if (selectedButton == 1) {
            NF_WriteText16(1, 0, 12, 3, ">");//Endless
            NF_ScrollBg(1, 0, 0, -26);
        } if (selectedButton == 2) {
            NF_WriteText16(1, 0, 11, 7, ">");// Multiplayer
            NF_ScrollBg(1, 0, 0, -1);
        } if (selectedButton == 3) {
            NF_WriteText16(1, 0, 12, 8, ">");//Settings
            NF_ScrollBg(1, 0, 0, -26);
        } 
    }
    NF_UpdateTextLayers();
    swiWaitForVBlank();
}

void MainMenu::cleanUp() {
    swiWaitForVBlank();
    confirmed = false;
    showSavePrompt = false;
    NF_UnloadTiledBg("save");
    NF_DeleteTiledBg(1, 3);
    NF_ClearTextLayer16(1, 0);
    NF_Reset16bitsBgBuffers(); 
    NF_Disble16bitsBackBuffer(0);
    NF_UnloadTextFont("topfont");
    NF_UnloadTiledBg("menu");
    NF_InitTextSys(1);
    NF_ResetTiledBgBuffers();
    vramSetBankA(VRAM_A_LCD);
    vramSetBankB(VRAM_B_LCD);
    vramSetBankC(VRAM_C_LCD);
    vramSetBankD(VRAM_D_LCD);
    dmaFillHalfWords(0, (void*)0x06000000, 128 * 1024 * 4); // clear VRAM A–D region


    swiWaitForVBlank();
}

