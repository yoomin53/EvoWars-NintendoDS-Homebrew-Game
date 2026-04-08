//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "TitleScreen.hpp"
#include <nds.h>
#include <math.h>
#include <stdio.h>
#include <nf_lib.h>
#include <NEMain.h>
#include <filesystem.h>
#include "bgm.h"

// --- TitleScreen class ---

bool TitleScreen::isFinished() const { return finished; }

TitleScreen::TitleScreen() : finished(false), textY(0), textDir(1) {} // constructor

void TitleScreen::init() {
    //init NitroFS
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");
    // -------------------------
    // 3D INITIALIZATION
    // -------------------------
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);
    NE_Init3D();
    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    //lcdSwap();

    // Create 3D entities
    model = NE_ModelCreate(NE_Static);
    camera = NE_CameraCreate();
    material = NE_MaterialCreate();

    // Load model
    NE_ModelLoadStaticMeshFAT(model, "Lina.bin");
    NE_MaterialTexLoadFAT(material, NE_A1RGB5, 256, 256, NE_TEXGEN_TEXCOORD, "Lina.img.bin");
    NE_ModelSetMaterial(model, material);
    NE_ClearColorSet(NE_Black, 31, 63);
    NE_LightSet(0, NE_White, -0.5, -0.5,-0.5);
    
    // Set up camera
    NE_CameraSet(camera,
                4, 4, 4,    // Eye position
                 0, 3, 0,    // Target
                 0, 1, 0);   // Up vector
    
    // -------------------------
    // 2D INITIALIZATION
    // -------------------------

    NF_Set2D(1, 0); //Bottom screen but will be swapped to top
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(1); 
    NF_InitTextSys(1);
    NF_LoadTiledBg("bg/title_bg", "title", 256, 256);
    NF_CreateTiledBg(1, 3, "title"); //screen 1, bg 3
    NF_ScrollBg(1, 3, 0, 40); //scroll bg to up
    NF_HideBg(1, 3); 
    NF_LoadTextFont16("fnt/font16", "topfont", 256, 256, 0);
    NF_CreateTextLayer16(1, 0, 0, "topfont");
    NF_WriteText16(1, 0, 11, 10, "PRESS START");
    NF_LoadTextFont16("fnt/font16", "verFont", 256, 256, 0);
    NF_CreateTextLayer16(1, 1, 0, "verFont");
    NF_WriteText16(1, 1, 0, 11, "ver.1.0");
    NF_UpdateTextLayers();
    NE_SwapScreens();  // Swap 3D buffers
    // sound 
    Audio_Init(); 
    //Audio_PlayBGM("nitro:/audio/motivation.wav", true); 
    Audio_PlayBGM("nitro:/audio/viacheslavstarostin-game-gaming-video-game-music.wav", true); 

}

void TitleScreen::update() { 
    NE_WaitForVBL(NE_UPDATE_GUI);
    Audio_Update();
    NF_ShowBg(1, 3); 
    scanKeys();
    if (keysDown() & (KEY_START | KEY_TOUCH | KEY_A))
        finished = true;
    seed++; 
    // Rotate model slightly each frame
    NE_ModelRotate(model, 0, 2, 0);
    textY += textDir;
    if (textY > 10)
        textDir = -1;
    else if (textY < -10)
        textDir = 1;
    NF_ScrollBg(1, 0, 0, textY); 
    
}
void TitleScreen::drawScene(void* arg) {
    // Convert back from void* to TitleScreen*
    TitleScreen* self = static_cast<TitleScreen*>(arg);

    NE_CameraUse(self->camera);
    NE_ModelDraw(self->model);
}

void TitleScreen::render() {
    // Update text layers first (2D)
    NF_UpdateTextLayers();
    // Run one 3D frame using Nitro Engine's pipeline
    NE_ProcessArg(TitleScreen::drawScene, this);
}

void TitleScreen::cleanUp() {
    // Free 3D entities
    //NE_MainScreenSetOnTop();
    //NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    Audio_StopBGM(); 
    NE_ModelFreeMeshWhenDeleted(model);
    NE_ModelDelete(model);
    NE_ModelSystemEnd();
    NE_CameraDelete(camera);
    NE_CameraSystemEnd();
    NE_MaterialDelete(material);
    NE_End();
    NF_DeleteTiledBg(1, 3);
   // NF_DeleteTextLayer(1, 0);
    NF_ClearTextLayer16(1, 0);
    NF_ClearTextLayer16(1, 1);
    NF_UnloadTextFont("topfont");
    NF_UnloadTextFont("verFont");
    NF_UnloadTiledBg("title");
    NF_ResetTiledBgBuffers();
    NE_WaitForVBL(NE_UPDATE_ANIMATIONS);  // force one full frame
    lcdSwap();  // now swap happens cleanly
    //NE_SwapScreens();  // Swap back to normal screens does not work Idk why ?? so use lcdSwap instead
    
}