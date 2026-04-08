//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include <nds.h>
#include <nf_lib.h>
#include <NEMain.h>
#include <stdio.h>
#include "Settings.hpp"
#include <filesystem.h>

#include "font_fnt_bin.h"
#include "font_16.h"
#include "font_256.h"
#include "background.h"

// GUI objects (Nitro Engine)
//static NE_GUIObj *btnBack;
static NE_GUIObj *chkMuteAll;
static NE_GUIObj *chkdebug;
static NE_GUIObj *slideSFX;
static NE_GUIObj *radioDifficultyEasy;
static NE_GUIObj *radioDifficultyNormal;
static NE_GUIObj *radioDifficultyHard;
static NE_GUIObj *btnBack; 

// GUI textures (Nitro Engine)
static NE_Material *ButtonImg;
static NE_Material *TrueImg;
static NE_Material *EmptyImg;
static NE_Palette *ButtonPal;
static NE_Palette *TruePal;
static NE_Palette *EmptyPal;




void Settings::init(SaveData* data)
{
    swiWaitForVBlank();
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    mute = data->mute; 
    debug = data->debug; 
    SFX = data->SFX; 
    difficulty = data->difficulty; //0 easy, 1 medium, 2 hard

    // ==========================
    // Init Nitro Engine for GUI
    // ==========================
    irqSet(IRQ_VBLANK, NE_VBLFunc);
    irqSet(IRQ_HBLANK, NE_HBLFunc);

    NE_Init3D();

    NE_MainScreenSetOnTop(); // 3D top
    NE_SwapScreens();        // move 3D to bottom
    //lcdSwap();

    NE_TextureSystemReset(0, 0, NE_VRAM_AB);
    NE_GUISystemReset(20);

    // Init console in non-3D screen
    consoleDemoInit();

    // Set bg color for 3D screen
    NE_ClearColorSet(RGB15(15, 15, 15), 31, 63);

    
    // Load GUI textures
    ButtonImg = NE_MaterialCreate();
    TrueImg = NE_MaterialCreate();
    EmptyImg = NE_MaterialCreate();
    ButtonPal = NE_PaletteCreate();
    TruePal = NE_PaletteCreate();
    EmptyPal = NE_PaletteCreate();

    NE_MaterialTexLoadFAT(ButtonImg, NE_PAL256, 64, 64,
                          (NE_TextureFlags)(NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT),
                          "gui/button.img.bin");
    NE_PaletteLoadFAT(ButtonPal, "gui/button.pal.bin", NE_PAL256);
    NE_MaterialSetPalette(ButtonImg, ButtonPal);

    NE_MaterialTexLoadFAT(TrueImg, NE_PAL256, 64, 64,
                          (NE_TextureFlags)(NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT),
                          "gui/true.img.bin");
    NE_PaletteLoadFAT(TruePal, "gui/true.pal.bin", NE_PAL256);
    NE_MaterialSetPalette(TrueImg, TruePal);

    NE_MaterialTexLoadFAT(EmptyImg, NE_PAL256, 64, 64,
                          (NE_TextureFlags)(NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT),
                          "gui/empty.img.bin");
    NE_PaletteLoadFAT(EmptyPal, "gui/empty.pal.bin", NE_PAL256);
    NE_MaterialSetPalette(EmptyImg, EmptyPal);

    // ==========================
    // Init NightFoxLib backgrounds
    // ==========================
    
    NF_Set2D(1, 0); // top screen swapped to top 
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(1); 
    // Load backgrounds (converted with nfgrit)
    NF_LoadTiledBg("bg/settings", "settings", 256, 256);
    NF_CreateTiledBg(1, 3, "settings");
    NF_ScrollBg(1, 3, 0, 25);
    
    
    // ==========================
    // Create GUI buttons (Nitro Engine) on bottom screen
    // ==========================
    int x = 200, y = 40, width = 16, height = 16;
    // Difficulty radio buttons
    radioDifficultyEasy = NE_GUIRadioButtonCreate(x, y, x + width, y + height, 1, difficulty == 0);
    NE_GUIRadioButtonConfig(radioDifficultyEasy, TrueImg, EmptyImg, NE_White, 31, NE_Gray, 31);
    y += height + 4;
    radioDifficultyNormal = NE_GUIRadioButtonCreate(x, y, x + width, y + height, 1, difficulty == 1);
    NE_GUIRadioButtonConfig(radioDifficultyNormal, TrueImg, EmptyImg, NE_White, 31, NE_Blue, 31);
    y += height + 4;
    radioDifficultyHard = NE_GUIRadioButtonCreate(x, y, x + width, y + height, 1, difficulty == 2);
    NE_GUIRadioButtonConfig(radioDifficultyHard, TrueImg, EmptyImg, NE_White, 31, NE_Yellow, 31);
    // Checkboxes
    y += height + 4 + 10;
    chkMuteAll = NE_GUICheckBoxCreate(x, y, x + width, y + height, mute);
    NE_GUICheckBoxConfig(chkMuteAll, TrueImg, EmptyImg, NE_White, 31, NE_Yellow, 15);
    // Sliders
    x = 10; width = 236; y += height + 2 + 20; height = 10; 
    slideSFX = NE_GUISlideBarCreate(x, y, x + width, y + height, 0, 255, SFX);
    NE_GUISlideBarConfig(slideSFX, EmptyImg, EmptyImg, NULL, NE_White, NE_Yellow, NE_Black, 31, 25, 15);
    y += height + 4; x = 200; width = 16; height = 16;
    chkdebug = NE_GUICheckBoxCreate(x, y, x + width, y + height, debug);
    NE_GUICheckBoxConfig(chkdebug, TrueImg, EmptyImg, NE_White, 31, NE_Yellow, 15);

    //Back button
    btnBack = NE_GUIButtonCreate(255-30, 10, 255-10, 30);

    // ==========================
    // Init RichText (Nitro Engine)
    // ==========================
    NE_InitConsole(); 
    NE_RichTextInit(0); // font slot 0
    NE_RichTextMetadataLoadMemory(0, font_fnt_bin, font_fnt_bin_size);
    {
        NE_Material *Font256 = NE_MaterialCreate();
        NE_MaterialTexLoad(Font256, NE_PAL256, 256, 256,
                            (NE_TextureFlags) (NE_TEXGEN_TEXCOORD | NE_TEXTURE_COLOR0_TRANSPARENT),
                           font_256Bitmap);

        NE_Palette *Pal256 = NE_PaletteCreate();
        NE_PaletteLoad(Pal256, font_256Pal, 256, NE_PAL256);

        NE_MaterialSetPalette(Font256, Pal256);

        // The material and palette will be deleted when the rich text font is
        // deleted.
        NE_RichTextMaterialSet(0, Font256, Pal256);
    }

}

void Settings::update()
{   
    NE_WaitForVBL(NE_UPDATE_GUI);

    scanKeys();
    touchPosition touch;
    touchRead(&touch);

    NE_UpdateInput();
    NE_GUIUpdate();

    if (NE_GUIObjectGetEvent(btnBack) == NE_Clicked){ 
        settingsFinished = true;
        if (NE_GUIRadioButtonGetValue(radioDifficultyEasy)) difficulty = 0; 
        else if (NE_GUIRadioButtonGetValue(radioDifficultyNormal)) difficulty = 1; 
        else difficulty = 2; 
        mute = NE_GUICheckBoxGetValue(chkMuteAll); 
        debug = NE_GUICheckBoxGetValue(chkdebug); 
        SFX = NE_GUISlideBarGetValue(slideSFX); 
    }
    NE_Process(Settings::draw);
}

void Settings::draw()
{
    // Draw GUI buttons/sliders/checkboxes (Nitro Engine)
    NE_2DViewInit();
    
    // ---- Text labels ----
    NE_RichTextRender3D(0, "DIFFICULTY", 20, 22);
    NE_RichTextRender3D(0, "  Easy",       20, 40);
    NE_RichTextRender3D(0, "  Normal",     20, 60);
    NE_RichTextRender3D(0, "  Hard",       20, 80);

    NE_RichTextRender3D(0, "Mute",         20, 110);
    NE_RichTextRender3D(0, "SFX",         20, 130);
    NE_RichTextRender3D(0, "Debug Console",  20, 160);
    NE_RichTextRender3D(0, "EXIT:",  187, 10);
    NE_GUIDraw();
    

}

bool Settings::isFinished() const
{
    return settingsFinished;
}

void Settings::end(SaveData* data)
{   
    data->mute = mute; 
    data->debug = debug; 
    data->SFX = SFX; 
    data->difficulty = difficulty; //0 easy, 1 medium, 2 hard
    settingsFinished = false;
    lcdSwap(); 
    // Delete GUI
    NE_RichTextEnd(0); //slot number
    NE_GUIDeleteAll();
    NE_End();

    // Delete NightFoxLib backgrounds
    swiWaitForVBlank(); 
    NF_DeleteTiledBg(1, 3); 
    NF_UnloadTiledBg("settings"); 
}
