//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "video.h"
#include <stdio.h>

/* ------------------------------------------------------------------ */
/*  Helper: count the number of frame files in a sub-folder            */
/* ------------------------------------------------------------------ */
static int countFrames(const char *subFolder)
{
    char path[256];
    int  count = 0;

    /* Scan until a missing file is found */
    for (int i = 0; i < 9999; ++i) {               // safety cap
        sprintf(path, "%s/frame%d.img", subFolder, i);
        if (!NF_FileExists(path))                 // NightFox Lib helper
            break;
        ++count;
    }
    return count;
}

// === VIDEO PLAYER ===
int playVideoDual(const char *baseFolder) {//meant to work like void, seems like it's a good practise to keep it as int func.
    // --- Console for debug (optional) ---
    if (!baseFolder || baseFolder[0] == '\0')
        return -1;
    consoleDemoInit();

    // --- Init NitroFS & NightFox Lib ---
    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    // --- Setup Dual-Screen Mode 5 (256x192 bitmap) ---
    NF_Set2D(0, 5);  // Screen 0: Mode 5
    NF_Set2D(1, 5);  // Screen 1: Mode 5
    // Initialise DS audio hardware 
    soundEnable(); 
    //--- setup audio buffers ---
    NF_InitRawSoundBuffers(); 
    // --- Load Soundbank ---
    char path[256]; 
    sprintf(path, "sfx/%s", baseFolder);
    NF_LoadRawSound(path, 1, 11025, 1); 
    // --- Init 8-bit Bitmap BG System ---
    NF_InitBitmapBgSys(0, 0);  // Screen 0, layer 0
    NF_InitBitmapBgSys(1, 0);  // Screen 1, layer 0

    // --- Setup 8-bit Buffers (VRAM→WRAM copy) ---
    NF_Init8bitsBgBuffers();
    NF_Init8bitsBackBuffer(0);  // Screen 0 backbuffer
    NF_Init8bitsBackBuffer(1);  // Screen 1 backbuffer
    NF_Enable8bitsBackBuffer(0);
    NF_Enable8bitsBackBuffer(1);

    // --- Playback State ---
    // full sub-folder paths 
    char topFolder[256];
    char bottomFolder[256];
    sprintf(topFolder, "video/%s/Top", baseFolder);
    sprintf(bottomFolder, "video/%s/Bottom", baseFolder);
    // Count frames (they must be the same on both screens) 
    int frameCount = countFrames(topFolder);
    if (frameCount == 0|| countFrames(bottomFolder) != frameCount) {
        printf("ERROR: no frames or mismatch in %s\n", baseFolder);
        swiWaitForVBlank();// it must be here otherwise it would flicker. loading is slow
        // --- Input ---
        scanKeys();
        if (keysDown() & KEY_START) {
            return -2;
        }
        
    }
    
    //playback varaibles
    int frame      = 0;
    int frameTimer = 0;

    // --- Load FIRST Frame (prevents flash) ---
    sprintf(path, "%s/frame0", topFolder); 
    NF_Load8bitsBg(path, 0);
    NF_Copy8bitsBuffer(0, 2, 0);  // Buffer → VRAM (screen 0)
    sprintf(path, "%s/frame0", bottomFolder);
    NF_Load8bitsBg(path, 1);
    NF_Copy8bitsBuffer(1, 2, 1);  // Buffer → VRAM (screen 1)
    swiWaitForVBlank();
    u8 sound_id = NF_PlayRawSound(1, 127, 64, false, 0);
    NF_Flip8bitsBackBuffer(0, 1); 
    NF_Flip8bitsBackBuffer(1, 1);
    // --- MAIN LOOP ---
    while (1) {
        
        // --- Next Frame ---
        //frame = (frame + 1) % frameCount;   // advance
        frame++;  
        if (frame == frameCount) {               // we just wrapped back to 0
            break;                      // exit the while(1)
        } 
        
        // --- Build Filenames ---
        char topPath[256], botPath[256];
        snprintf(topPath, sizeof(topPath), "%s/frame%d", topFolder, frame);
        snprintf(botPath, sizeof(botPath),"%s/frame%d", bottomFolder, frame);

        // --- UNLOAD (Free VRAM for new frame) ---
        
        NF_Unload8bitsBg(0);       // Unload screen 0
        NF_Unload8bitsBg(1);       // Unload screen 1

        // --- LOAD New Frames ---
        NF_Load8bitsBg(topPath, 0);  // Load to screen 0 slot
        NF_Load8bitsBg(botPath, 1);  // Load to screen 1 slot

        // --- FLIP Buffers to VRAM ---
        //NF_Copy8bitsBuffer(0, 2, 0);  // Screen 0: buffer 0 → VRAM
        //NF_Copy8bitsBuffer(1, 2, 1);  // Screen 1: buffer 1 → VRAM

        swiWaitForVBlank();// it must be here otherwise it would flicker. loading is slow
        // --- Input ---
        scanKeys();
        if (keysDown() & KEY_START) {
            break;  // Exit cleanly
        }
        NF_Copy8bitsBuffer(0, 0, 0); 
        NF_Copy8bitsBuffer(1, 0, 1);  
        //NF_Flip8bitsBackBuffer(0, 1); 
        //NF_Flip8bitsBackBuffer(1, 1);
        
    }

    // --- Cleanup  ---
    soundKill(sound_id);
    NF_Disble8bitsBackBuffer(0);
    NF_Disble8bitsBackBuffer(1);
    //NF_Unload8bitsBg(0);
    //NF_Unload8bitsBg(1);
    NF_ResetRawSoundBuffers();
    NF_Reset8bitsBgBuffers();
    soundDisable(); 
    //dmaFillWords(0, VRAM_A, 128 * 1024);
    //dmaFillWords(0, VRAM_B, 128 * 1024);
    //dmaFillHalfWords(0, (void*)0x06000000, 128 * 1024 * 4); // clear VRAM A–D region
    dmaFillWords(0, BG_PALETTE, 512); // it seems like the pallette is corrupted affecting next so this should clean up
    dmaFillWords(0, BG_PALETTE_SUB, 512);
    
    return 0; 
}


