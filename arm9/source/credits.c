//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "Credits.h"
#include "bgm.h"
#include "soundbank.h"

void playCredits(void){
    initCredits(); 
    loadCredits(); 
    int frame = 0; 
    char name[256];
    bool end = false; 
    int textScroll = 0; 

    NF_UpdateTextLayers();
    while(textScroll < 1000){
        Audio_Update();
        for(int i = 0; i<4; i++){
            swiWaitForVBlank(); 
            scanKeys();
            int pressed = keysDown();
            touchPosition touch;
            touchRead(&touch);
            if (pressed & KEY_A | ((pressed & KEY_TOUCH))) {end = true; break;}
        } 
        if (textScroll==0){
            NF_ClearTextLayer16(0, 0);
            NF_WriteText16(0, 0, 0, 10, "Thank you for playing,");
            NF_WriteText16(0, 0, 0, 11, "Evowars: Alien Assault");
            NF_UpdateTextLayers();
        } else if (textScroll == 225){
            NF_ClearTextLayer16(0, 0);
            NF_WriteText16(0, 0, 0, 10, "See you in the next game");
            NF_WriteText16(0, 0, 0, 11, "created, Designed, Programmed, and Produced by");
            NF_WriteText16(0, 0, 0, 13, "Jung, Yoomin");
            NF_UpdateTextLayers();
        } else if (textScroll == 470) {
            NF_ClearTextLayer16(0, 0);
            NF_UpdateTextLayers();
        } else if (textScroll == 495) {
            NF_ClearTextLayer16(0, 0);
            NF_WriteText16(0, 0, 0, 12, "Thanks to:");
            NF_WriteText16(0, 0, 0, 13, "BlocksDS"); 
            NF_WriteText16(0, 0, 13, 13, "ElevenLabs.io"); 
            NF_WriteText16(0, 0, 0, 14, "ViacheslavStarostin from pixabay"); 
            NF_UpdateTextLayers();
            //NF_WriteText16(0, 0, 5, 16, "(c) 2026 Jung Yoomin");
            //NF_WriteText16(0, 0, 5, 17, "All Rights Reserved.");
        } else if(textScroll == 750) {
            NF_ClearTextLayer16(0, 0);
            NF_UpdateTextLayers();
        }
        if(end) break; 
        NF_ScrollBg(1, 3, frame, 56);
        NF_ScrollBg(0, 0, 0, frame); 
        frame+=1; 
        textScroll+=1; 
        if (frame==255) frame = 0; 
    }
    Audio_StopBGM(); 
    Audio_PlaySFX(SFX_SUCCESSFUL);
    cleanUpCredits(); 
} 

static void cleanUpCredits(void){
    //NF_DeleteTiledBg(0, 0); 
    NF_DeleteTiledBg(1, 3); 
    NF_ResetTiledBgBuffers(); 
    Audio_Unload_SFX();
}

static void initCredits(void){
    // --- Init NitroFS & NightFox Lib ---
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // --- Setup Dual-Screen Mode 0 (256x192 Tiled) ---
    NF_Set2D(0, 0);  // Screen 0: Mode 0
    NF_Set2D(1, 0);  // Screen 1: Mode 0

    // Initialize tiled backgrounds system
    NF_InitTiledBgBuffers();    // Initialize storage buffers
    NF_InitTiledBgSys(0);       // Top screen
    NF_InitTiledBgSys(1);       // Bottom screen
    // init text system
    NF_InitTextSys(0);
    NF_LoadTextFont16("fnt/font16", "topfont", 256, 256, 0);    
    NF_CreateTextLayer16(0, 0, 0, "topfont");
    NF_UpdateTextLayers();

    Audio_Init(); 
    Audio_Load_SFX();
    Audio_PlayBGM("nitro:/audio/viacheslavstarostin-jazz-lounge-elevator-music.wav", true); 
}

static void loadCredits(void){
    char topFolder[256] = "video/nightSky/HKNight";
    char bottomFolder[256] = "video/nightSky/HKNight";
    //NF_LoadTiledBg(topFolder, "top", 2048, 256);
    NF_LoadTiledBg(bottomFolder, "bottom", 512, 256);
    //NF_CreateTiledBg(0, 3, "top");
    NF_CreateTiledBg(1, 3, "bottom");
    NF_ScrollBg(1, 3, 0, 56); 
}