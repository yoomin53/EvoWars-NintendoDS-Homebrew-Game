//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "Game.hpp"
#include <nf_lib.h>
#include <vector>
#include <stdio.h>
#include <filesystem.h>
#include "credits.h"
GameState gameState = STATE_PLAYING;

void Game::cleanUp()
{
    gameState = STATE_PLAYING;
    CleanSpriteGrid();
    Audio_Unload_SFX(); 
    Audio_StopBGM(); 
}

void Game::TogglePause()
{
    if (gameState == STATE_PLAYING)
    {
        ShowPauseMenu();
        gameState = STATE_PAUSED;
    }
    else if (gameState == STATE_PAUSED)
    {
        HidePauseMenu();
        gameState = STATE_PLAYING;
    }
}

void Game::StartStage(int _stageID, int _stageNum)
{
    stageID = _stageID;
    stageNum = _stageNum; 
    goldEarned = 0; 
    victory = false; 
    int maxStage = 10; 

    RunStage(&stage);
    END(); 
    if(victory && (stageID*100+stageNum>=level)){
        if(stageNum+1<=maxStage){
            level++; 
        } else if(stageID < 2 && stageNum == maxStage) {
            level += 100 - maxStage + 1; 
        } 
    }
    if(saveData)
            saveData->level = level;
    if(victory && stageID == 2 && stageNum == maxStage) {
        playCredits(); 
    }
}

void Game::END()
{   
    Audio_Init(); 
    Audio_Load_SFX();
    //if (gameState != STATE_WIN && gameState != STATE_LOSE) return; 
    // Initialize 2D systems
    NF_Set2D(0, 0); //Top screen
    NF_Set2D(1, 0); //Bottom screen
    swiWaitForVBlank();
    // Initialize NitroFS
    nitroFSInit(NULL);
    //Set root folder for NitroFS
    NF_SetRootFolder("NITROFS");
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);

    if (victory == true){
        NF_LoadTiledBg("bg/GameWinTop", "endGameTop", 256, 256);
        NF_LoadTiledBg("bg/GameWinBottom", "endGameBottom", 256, 256);
        //Audio_PlaySFXEx(SFX_CLEAR, sfxVolume, 128);
        Audio_PlayBGM("nitro:/audio/clear.wav", true);
        
    }
    else {
        NF_LoadTiledBg("bg/GameOverTop", "endGameTop", 256, 256);
        NF_LoadTiledBg("bg/GameOverBottom", "endGameBottom", 256, 256);
        Audio_PlaySFXEx(SFX_FAIL, sfxVolume, 128);
    }
    NF_CreateTiledBg(0, 3, "endGameTop"); //screen 1, bg 3
    NF_CreateTiledBg(1, 3, "endGameBottom"); //screen 1, bg 3
    NF_ScrollBg(1, 3, 0, 63); //scroll bg to up
    //--- Text16 Initialization ---
    NF_InitTextSys(1);
    NF_LoadTextFont16("fnt/font16", "Goldfont", 256, 256, 0);    
    NF_CreateTextLayer16(1, 0, 0, "Goldfont");
    NF_DefineTextColor(1, 0, 1, 31, 31, 0); // Red
    NF_SetTextColor(1, 0, 1);

    char goldTotal[256];
    sprintf(goldTotal, "Gold Earned:  %d", goldEarned);
    NF_WriteText16(1, 0, 7, 4, goldTotal);
    NF_UpdateTextLayers();

    while(1) {
        swiWaitForVBlank();
        scanKeys();
        int pressed = keysDown();
        touchPosition touch;
        touchRead(&touch);
        if(victory) Audio_Update(); 
        if (pressed & KEY_TOUCH | pressed & KEY_A | pressed & KEY_START){
            break; 
        }

    } 
    if(victory) Audio_StopBGM();
    NF_ClearTextLayer16(1, 0);
    NF_ResetTiledBgBuffers(); 
    return; 
}

void Game::DealDamageToPlayer(int attack)
{
    HP -= attack; 
    if(HP<=0) gameState = STATE_LOSE; 
}

void Game::RunStage(Stage* stage)
{   
    stage->SetGame(this);
    int maxMuzzleFrame; 
    int muzzle_frame; 
    Audio_Init(); 
    Audio_Load_SFX();
    switch(stageID)
    {
        case LEIA: 
            HP = LeiaHp;
            damage += Leia.damage; 
            accuracy += Leia.accuracy; 
            criticalChance += Leia.criticalChance; 
            criticalDamage += Leia.criticalDamage;
            InitSpriteGrid("Leia", HP, agilityLevel, accuracy);
            muzzle_frame = 30;
            maxMuzzleFrame = LeiaMuzzleFrame; 
            Audio_PlayBGM("nitro:/audio/viacheslavstarostin-advertising-music.wav", true);
        break; 
        case AIDEN:
            HP = AidenHp; 
            damage += Aiden.damage; 
            accuracy += Aiden.accuracy; 
            criticalChance += Aiden.criticalChance; 
            criticalDamage += Aiden.criticalDamage;
            InitSpriteGrid("Aiden", HP, agilityLevel, accuracy); 
            muzzle_frame = 20;
            maxMuzzleFrame = AidenMuzzleFrame; 
            Audio_PlayBGM("nitro:/audio/viacheslavstarostin-dynamic-stomp-percussion-music.wav", true);
        break; 
        case LINA:
            damage += Lina.damage; 
            accuracy += Lina.accuracy; 
            criticalChance += Lina.criticalChance; 
            criticalDamage += Lina.criticalDamage;
            HP = LinaHp;  
            InitSpriteGrid("Lina", HP, agilityLevel, accuracy);
            muzzle_frame = 10;
            maxMuzzleFrame = LinaMuzzleFrame;  
            Audio_PlayBGM("nitro:/audio/viacheslavstarostin-gaming-game-video-game-music.wav", true);
        break; 
    } 
    stage->Init();
    stage->LoadStage(stageID, stageNum);
    DrawSpriteGrid();

    int shotX = 0;
    int shotY = 0;

    while(1)
    {   
        Audio_Update();
        scanKeys();
        u16 keysDownNow = keysDown();

        bool touching = keysHeld() & KEY_TOUCH;

        touchPosition touch;
        touchRead(&touch);

        if(keysDownNow & KEY_START)
        {
            TogglePause();
        }

        //--------------------------------
        // PLAYING
        //--------------------------------
        if(gameState == STATE_PLAYING)
        {
            bool shot = false;

            bool muzzle = UpdateSpriteGrid(
                touching,
                touch,
                shotX,
                shotY,
                HP
            );

            if(muzzle)
            {
                muzzle_frame++;

                if(muzzle_frame > maxMuzzleFrame)
                {
                    muzzle_frame = 0;

                    int AIM_RADIUS =
                        (int)(32.0f * 256.0f /
                        (512.0f - AIM_SIZE));

                    shotX = touch.px +
                        rand() % (2 * AIM_RADIUS + 1) -
                        AIM_RADIUS;

                    shotY = touch.py +
                        rand() % (2 * AIM_RADIUS + 1) -
                        AIM_RADIUS;

                    if (shotX < 0 || shotX > 255)
                        shotX = touch.px;

                    if (shotY < 0 || shotY > 191)
                        shotY = touch.py;

                    shot = true;
                    Audio_PlaySFXEx(SFX_GUNSHOT, sfxVolume, 128); //volume, pan
                }
            }
            else
            {
                muzzle_frame = maxMuzzleFrame;
            }

            PlayerInput input;

            input.touchX = touch.px;
            input.touchY = touch.py;
            input.touching = touching;
            input.shotX = shotX;
            input.shotY = shotY;
            input.shot = shot;
            input.playerX = GetPlayerX(); 
            input.damage = damage; 
            if (rand()%100 < criticalChance){
                input.damage = damage*(100+criticalDamage)/100; 
            }

            stage->Update(input);

            if(stage->IsClear())
            {   
                AddGold(stage->clearGold);
                goldEarned += stage->clearGold; 
                gameState = STATE_WIN;
            }
        }

        //--------------------------------
        // PAUSE
        //--------------------------------
        else if(gameState == STATE_PAUSED)
        {
            if(keysDownNow & KEY_B)
            {
                break;
            }

            if(keysDownNow & KEY_A)
            {
                HidePauseMenu();
                gameState = STATE_PLAYING;
            }
        }

        //--------------------------------
        // WIN
        //--------------------------------
        else if(gameState == STATE_WIN)
        {    
            victory = true; 
            UpdateSpriteGrid(
                touching,
                touch,
                shotX,
                shotY,
                HP
            );
            DrawSpriteGrid();
            Audio_StopBGM(); 
            for(int i = 0; i < 30; i++){
                swiWaitForVBlank();
            }
            break;
        }

        //--------------------------------
        // LOSE
        //--------------------------------
        else if(gameState == STATE_LOSE)
        {   
            UpdateSpriteGrid(
                touching,
                touch,
                shotX,
                shotY,
                HP
            );
            DrawSpriteGrid();
            Audio_StopBGM(); 
            for(int i = 0; i < 30; i++){
                swiWaitForVBlank();
            }
            break;
        }

        DrawSpriteGrid();
    }

    cleanUp();
}