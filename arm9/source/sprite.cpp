//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include <nds.h>
#include <nf_lib.h>
#include <stdio.h>
#include <filesystem.h>
#include "sprite.hpp"


// --------------------------------------------------
// CONFIG
// --------------------------------------------------
static const int GRID_SIZE   = 9;
static const int FRAME_BODY  = 6;   // Leia_0 ~ Leia_5
static const int FRAME_MUZZLE = 6;  // Leia_6

static const int ANIM_SPEED = 4;


// --------------------------------------------------
// STATE
// --------------------------------------------------
enum AnimState {
    STATE_FORWARD,
    STATE_MUZZLE,
    STATE_REVERSE
};

static AnimState animState = STATE_FORWARD;

static int currentFrame = 0;
static int animTimer = 0;
static int muzzleTile = 0;

static std::string characterName;

// --------------------------------------------------
// AIM SPRITE
// --------------------------------------------------
static const int AIM_RAM_ID  = 63;  // 0-127 
static const int AIM_VRAM_ID = 0;  // 63 is the biggest ID in RAM (nf_sprite256.h) 
int AIM_SIZE = 1; 
// --------------------------------------------------
// PAUSE MENU SPRITE
// --------------------------------------------------
static const int PAUSE_RAM_ID  = 68;
static const int PAUSE_PAL_ID  = 58;

static const int PAUSE_VRAM_ID = 14;   // 14~15
// --------------------------------------------------
// Player HP bar 
// --------------------------------------------------
static const int PLAYERHP_GFX_RAM_ID = 67; 
static const int PLAYERHP_PAL_RAM_ID = 57; 
static const int PLAYERHP_VRAM_ID = 10; //10~13 
// --------------------------------------------------
// BULLET SPRITE
// --------------------------------------------------
static const int BULLET_GFX_RAM_ID  = 66;  // 0-127 
static const int BULLET_PAL_RAM_ID  = 56; 
static const int BULLET_VRAM_ID = 1;  // 63 is the biggest ID in RAM (nf_sprite256.h) 
// --------------------------------------------------
// HPBAR SPRITE
// --------------------------------------------------
static const int HPBAR_GFX_RAM_ID  = 65;  // 0-127
static const int HPBAR_PAL_RAM_ID  = 55; 
int maxHP; 
// --------------------------------------------------
// MUZZLE SPRITE
// --------------------------------------------------
static const int MUZZLE_VRAM_ID = 9; 
// accuracy decrease the full size of the aim. 
int accuracy; 
// --------------------------------------------------
// Helper: load full frame (9 tiles)
// --------------------------------------------------
void LoadFrame(int frame)
{
    for (int i = 0; i < GRID_SIZE; i++) {

        int ramId  = frame * GRID_SIZE + i;
        int vramId = i;

        int x = (i % 3) * 64;
        int y = (i / 3) * 64;
        NF_DeleteSprite(1, vramId);
        NF_FreeSpriteGfx(1, vramId);
        NF_VramSpriteGfx(1, ramId, vramId, false);
        NF_VramSpritePal(1, ramId, vramId);

        NF_CreateSprite(1, vramId, vramId, vramId, x, y);
        NF_ShowSprite(1, vramId, true);
        NF_SpriteLayer(1, vramId, 1); 
    }
}

// --------------------------------------------------
// Helper: play muzzle (Leia_6 tile 0~8)
// --------------------------------------------------
void PlayMuzzle()
{
    int tile = muzzleTile;

    int ramId  = FRAME_MUZZLE * GRID_SIZE + tile;
    int x = 2 * 64; 
    int y = 0 * 64; 
    NF_DeleteSprite(1, MUZZLE_VRAM_ID);
    NF_FreeSpriteGfx(1, MUZZLE_VRAM_ID);
    NF_VramSpriteGfx(1, ramId, MUZZLE_VRAM_ID, false);
    NF_VramSpritePal(1, 54, MUZZLE_VRAM_ID); //ramId
    NF_CreateSprite(1, MUZZLE_VRAM_ID, MUZZLE_VRAM_ID, MUZZLE_VRAM_ID, x, y);//screen, id, gfx, pal, x, y
    NF_ShowSprite(1, MUZZLE_VRAM_ID, true);
    NF_SpriteLayer(1, MUZZLE_VRAM_ID, 3); 

    muzzleTile = (muzzleTile + 1) % GRID_SIZE;
}

// 플레이어 X 좌표 (상단 왼쪽 기준)
static int playerX = 10; // 초기 화면 중앙
static const int playerY = 0; // Y 고정, 필요시 변경
int PLAYER_SPEED = 1; // 이동 속도

int GetPlayerX() { return playerX; }

void MovePlayerSprites()
{   
    scanKeys();
    u16 keys = keysHeld();

    if(keys & KEY_LEFT)  playerX -= PLAYER_SPEED;
    if(keys & KEY_RIGHT) playerX += PLAYER_SPEED;

    // 화면 범위 제한
    if(playerX < 0) playerX = 0;
    if(playerX > 256 - 192) playerX = 256 - 192; // 3x3 타일 * 64px = 192

    for(int i = 0; i < GRID_SIZE; i++){
        int x = playerX + (i % 3) * 64;
        int y = playerY + (i / 3) * 64;
        NF_MoveSprite(1, i, x, y);
    }

    // muzzle도 같이 이동
    int muzzleX = playerX + 2 * 64;
    int muzzleY = playerY + 0 * 64;
    NF_MoveSprite(1, MUZZLE_VRAM_ID, muzzleX, muzzleY);
}
// --------------------------------------------------
// PLAYER HP BAR
// --------------------------------------------------

static const int HPBAR_SEGMENTS = 4;

void InitPlayerHPBar(int x, int y)
{   
    int spriteId = PLAYERHP_VRAM_ID; 
    for (int i = 0; i < HPBAR_SEGMENTS; i++) {
        NF_VramSpriteGfx(1, PLAYERHP_GFX_RAM_ID, spriteId, true);
        NF_VramSpritePal(1, PLAYERHP_PAL_RAM_ID, spriteId);

        NF_CreateSprite(
            1,
            spriteId, //spriteid 
            spriteId, //gfxid 
            spriteId, //palid 
            x + i * 32,
            y
        ); 
        NF_SpriteLayer(1, spriteId, 3);
        NF_SpriteFrame(1, spriteId, 30); // full
        spriteId++; 
    }
}

void UpdatePlayerHPBar(int hp)
{
    int perSegment = 30; // 각 세그먼트 최대 HP

    for (int i = 0; i < HPBAR_SEGMENTS; i++) {

        // 오른쪽부터 계산
        int spriteId = PLAYERHP_VRAM_ID + HPBAR_SEGMENTS - 1 - i;

        int segmentHP = hp - ((HPBAR_SEGMENTS-1-i) * perSegment);

        int frame;

        if (segmentHP >= perSegment) {
            frame = 0; // 가득참
        }
        else if (segmentHP <= 0) {
            frame = 30; // 빈칸
        }
        else {
            frame = perSegment - segmentHP; // 부분 감소
        }

        NF_SpriteFrame(1, spriteId, frame);
    }
}
void ShowPauseMenu(){
    NF_ShowSprite(1, PAUSE_VRAM_ID, true);
    NF_ShowSprite(1, PAUSE_VRAM_ID+1, true);
}
void CreatePauseMenu()
{   
    int x = 14;
    int y = 40; 
    swiWaitForVBlank();
    // VRAM transfer
    NF_VramSpriteGfx(1, PAUSE_RAM_ID, PAUSE_VRAM_ID, true);
    NF_VramSpritePal(1, PAUSE_PAL_ID, PAUSE_VRAM_ID);
    NF_VramSpriteGfx(1, PAUSE_RAM_ID, PAUSE_VRAM_ID+1, true);

    // sprite 생성
    NF_CreateSprite(1, PAUSE_VRAM_ID, PAUSE_VRAM_ID, PAUSE_VRAM_ID, x, y);
    NF_CreateSprite(1, PAUSE_VRAM_ID+1, PAUSE_VRAM_ID+1, PAUSE_VRAM_ID, x + 101, y);

    // frame 설정
    NF_SpriteFrame(1, PAUSE_VRAM_ID, 0);
    NF_SpriteFrame(1, PAUSE_VRAM_ID+1, 1);

    NF_SpriteLayer(1, PAUSE_VRAM_ID, 0);
    NF_SpriteLayer(1, PAUSE_VRAM_ID+1, 0);
    
    NF_ShowSprite(1, PAUSE_VRAM_ID, false);
    NF_ShowSprite(1, PAUSE_VRAM_ID+1, false);

    NF_EnableSpriteRotScale(1, PAUSE_VRAM_ID, PAUSE_VRAM_ID, true);
    NF_EnableSpriteRotScale(1, PAUSE_VRAM_ID+1, PAUSE_VRAM_ID+1, true);
    NF_SpriteRotScale(1, PAUSE_VRAM_ID, 0, 350, 350); 
    NF_SpriteRotScale(1, PAUSE_VRAM_ID+1, 0, 350, 350);
}

void HidePauseMenu()
{   
    NF_ShowSprite(1, PAUSE_VRAM_ID, false);
    NF_ShowSprite(1, PAUSE_VRAM_ID+1, false);
}

// --------------------------------------------------
// INIT
// -------------------------------------------------- 
void InitSpriteGrid(const std::string& name, int _maxHP, int agility, int _accuracy)
{
    characterName = name;
    maxHP = _maxHP; 
    PLAYER_SPEED += agility;
    currentFrame = 0;
    animState = STATE_FORWARD;
    muzzleTile = 0;
    accuracy = _accuracy; 

    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    swiWaitForVBlank();

    nitroFSInit(NULL);
    NF_SetRootFolder("NITROFS");

    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0); 
    NF_InitTiledBgSys(1);
    
    char path[128];
    snprintf(path, sizeof(path), "bg/%sBattleTop", characterName.c_str());  
    NF_LoadTiledBg(path, "Top", 256, 256);
    snprintf(path, sizeof(path), "bg/%sBattleBottom", characterName.c_str());  
    NF_LoadTiledBg(path, "Bottom", 256, 256);
    NF_CreateTiledBg(0, 3, "Top"); //screen 0, bg 3
    NF_CreateTiledBg(1, 3, "Bottom"); //screen 1, bg 3
    if(characterName != "Lina") NF_ScrollBg(1, 3, 0, 63); 
    NF_InitSpriteBuffers();
    NF_InitSpriteSys(0); // ★ TOP SCREEN sprite system
    NF_InitSpriteSys(1);

    // Load all Sprites into RAM
    //Load body frames (character_0 ~ character_5)
    for (int frame = 0; frame < FRAME_MUZZLE; frame++) {
        for (int i = 0; i < GRID_SIZE; i++) {

            char path[128];
            sprintf(
                path,
                "sprite/%s_%d/tile_%d",
                characterName.c_str(),
                frame,
                i
            );

            int ramId = frame * GRID_SIZE + i;
            NF_LoadSpriteGfx(path, ramId, 64, 64);
            NF_LoadSpritePal(path, ramId);
        }
    }
    //Load flame "Muzzle" frame (character_flame)
    for (int i = 0; i < GRID_SIZE; i++) {

            char path[128];
            sprintf(
                path,
                "sprite/%s_%s/tile_%d",
                characterName.c_str(),
                "flame",
                i
            );

            int ramId = FRAME_MUZZLE * GRID_SIZE + i;
            NF_LoadSpriteGfx(path, ramId, 64, 64);
            if (i == 0) NF_LoadSpritePal(path, ramId);
    }
    // Initial frame
    int frame = 0;
    for (int i = 0; i < GRID_SIZE; i++) {

        int ramId  = frame * GRID_SIZE + i;
        int vramId = i;

        int x = (i % 3) * 64;
        int y = (i / 3) * 64;
        NF_VramSpriteGfx(1, ramId, vramId, false);
        NF_VramSpritePal(1, ramId, vramId);
        NF_CreateSprite(1, vramId, vramId, vramId, x, y);
        NF_ShowSprite(1, vramId, true);
        NF_SpriteLayer(1, vramId, 1); 
    }
    //Load Player HP bar
    NF_LoadSpriteGfx("sprite/playerHP", PLAYERHP_GFX_RAM_ID, 32, 8); 
    NF_LoadSpritePal("sprite/playerHP", PLAYERHP_PAL_RAM_ID);
    InitPlayerHPBar(128, 0); 
    //Load Pause sprite
    NF_LoadSpriteGfx("sprite/pause", PAUSE_RAM_ID, 64, 64);
    NF_LoadSpritePal("sprite/pause", PAUSE_PAL_ID);
    CreatePauseMenu();

    //Load HP bar for enemy sprites, this will be used in Enemy.cpp 
    NF_LoadSpriteGfx("sprite/HPbar", HPBAR_GFX_RAM_ID, 32, 8); //32*8 
    NF_LoadSpritePal("sprite/HPbar", HPBAR_PAL_RAM_ID);

    // Load Top screen sprites  
    // Load aim sprite (TOP screen)
    NF_LoadSpriteGfx("sprite/aim", AIM_RAM_ID, 64, 64);
    NF_LoadSpritePal("sprite/aim", AIM_RAM_ID);
    // Load bullet sprite (Top screen)
    NF_LoadSpriteGfx("sprite/bullet", BULLET_GFX_RAM_ID, 32, 32); 
    NF_LoadSpritePal("sprite/bullet", BULLET_PAL_RAM_ID); 
    // Send to VRAM (initially hidden)
    NF_VramSpriteGfx(0, AIM_RAM_ID, AIM_VRAM_ID, false);
    NF_VramSpritePal(0, AIM_RAM_ID, AIM_VRAM_ID);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, BULLET_VRAM_ID, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, BULLET_VRAM_ID);

    // Create AIM and Bullet Sprite but hide
    NF_CreateSprite(0, AIM_VRAM_ID, AIM_VRAM_ID, AIM_VRAM_ID, 0, 0);
    NF_ShowSprite(0, AIM_VRAM_ID, false);
    NF_SpriteLayer(0, AIM_VRAM_ID, 0); 
    NF_EnableSpriteRotScale(0, AIM_VRAM_ID, 0, true);

    NF_CreateSprite(0, BULLET_VRAM_ID, BULLET_VRAM_ID, BULLET_VRAM_ID, 0, 0);
    NF_ShowSprite(0, BULLET_VRAM_ID, false);
    NF_SpriteLayer(0, BULLET_VRAM_ID, 0); 

    //send first sprite to muzzle vram slot in order to initiate
    NF_VramSpriteGfx(1, MUZZLE_VRAM_ID, MUZZLE_VRAM_ID, false);
    NF_VramSpritePal(1, MUZZLE_VRAM_ID, MUZZLE_VRAM_ID);
    NF_CreateSprite(1, MUZZLE_VRAM_ID, MUZZLE_VRAM_ID, MUZZLE_VRAM_ID, 0, 0);
    NF_ShowSprite(1, MUZZLE_VRAM_ID, false);

    //Load first enemy
    NF_LoadSpriteGfx("sprite/enemy/enemy", 69, 64, 64);
    NF_LoadSpritePal("sprite/HPbar", 59);
    NF_VramSpriteGfx(0, 69, 2, false);
    NF_VramSpritePal(0, 59, 2);
    NF_VramSpriteGfx(0, 69, 4, false);
    NF_VramSpritePal(0, 59, 4);
    NF_VramSpriteGfx(0, 69, 6, false);
    NF_VramSpritePal(0, 59, 6);
    NF_VramSpriteGfx(0, 69, 8, false);
    NF_VramSpritePal(0, 59, 8);
    NF_VramSpriteGfx(0, 69, 10, false);
    NF_VramSpritePal(0, 59, 10);
    NF_VramSpriteGfx(0, 69, 12, false);
    NF_VramSpritePal(0, 59, 12);
    NF_VramSpriteGfx(0, 69, 14, false);
    NF_VramSpritePal(0, 59, 14); 
    NF_UnloadSpriteGfx(69);
    NF_UnloadSpritePal(59);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 3, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 3);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 5, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 5);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 7, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 7);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 9, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 9);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 11, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 11);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 13, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 13);
    NF_VramSpriteGfx(0, BULLET_GFX_RAM_ID, 15, false);
    NF_VramSpritePal(0, BULLET_PAL_RAM_ID, 15);

}

// --------------------------------------------------
// UPDATE
// --------------------------------------------------
int count = 30; 
bool UpdateSpriteGrid(bool touching, touchPosition touch, int shotX, int shotY, int& HP)
{
    //scanKeys();
    //bool touching = keysHeld() & KEY_TOUCH;
    //touchPosition touch;
    //touchRead(&touch);
    /**
    animTimer++;
    if ((animTimer < ANIM_SPEED) != (animState == STATE_MUZZLE)) return false; 
    animTimer = 0;
    */
    // ------------------------
    // 🔥 HP retore
    // ------------------------
    static int hpRegenTimer = 0;
    const int HP_REGEN_DELAY = 5; // 프레임 단위 

    if (animState == STATE_FORWARD && currentFrame == 0 && !touching) {
        hpRegenTimer++;

        if (hpRegenTimer >= HP_REGEN_DELAY) {
            hpRegenTimer = 0;
            if (HP < maxHP) {
                HP++;
            }
        }
    } else {
        hpRegenTimer = 0;
    }

    UpdatePlayerHPBar(HP);//playerHP

    switch (animState) {

    // ------------------------
    case STATE_FORWARD:
        animTimer++; 
        if ((animTimer < ANIM_SPEED)) { MovePlayerSprites(); return false;} 
        animTimer = 0; 
        if (touching) {
            if (currentFrame < FRAME_BODY - 1) {
                currentFrame++;
                LoadFrame(currentFrame);
            } else {
                animState = STATE_MUZZLE;
                muzzleTile = 0;
            }
        } else {
            if (currentFrame > 0) {
                animState = STATE_REVERSE;
            }
        }
        MovePlayerSprites();
        return false;

    // ------------------------
    case STATE_MUZZLE:
        if (touching) {
            PlayMuzzle();
            //aim sprite move 
            int aimX = touch.px - 64; // 중앙 정렬 (32x32 기준)
            int aimY = touch.py - 64;
            NF_MoveSprite(0, AIM_VRAM_ID, aimX, aimY);
            NF_ShowSprite(0, AIM_VRAM_ID, true);
            NF_SpriteRotScale(0, 0, 0, AIM_SIZE, AIM_SIZE); //screen, rotset number, angle, scalex, scaley
            if(AIM_SIZE != 1){
            NF_MoveSprite(0, BULLET_VRAM_ID, shotX-16, shotY-16);
            NF_ShowSprite(0, BULLET_VRAM_ID, true);
            }
            if (AIM_SIZE < 384 - accuracy) AIM_SIZE++; 
        } else {
            animState = STATE_REVERSE;
            AIM_SIZE = 1; 
            NF_ShowSprite(0, AIM_VRAM_ID, false);
            NF_ShowSprite(1, MUZZLE_VRAM_ID, false);
            NF_ShowSprite(0, BULLET_VRAM_ID, false); 
        }
        MovePlayerSprites();
        return true;

    // ------------------------
    case STATE_REVERSE:
        animTimer++; 
        if ((animTimer < ANIM_SPEED)) { MovePlayerSprites(); return false;} 
        animTimer = 0; 
        if (!touching) {
            if (currentFrame > 0) {
                currentFrame--;
                LoadFrame(currentFrame);
            } else {
                animState = STATE_FORWARD;
            }
        }
        MovePlayerSprites();
        return false;
    }

}

// --------------------------------------------------
// DRAW
// --------------------------------------------------
void DrawSpriteGrid()
{   
    NF_SpriteOamSet(0);
    NF_SpriteOamSet(1);
    swiWaitForVBlank();
    oamUpdate(&oamMain);
    oamUpdate(&oamSub);
}

void CleanSpriteGrid()
{
    swiWaitForVBlank();
    NF_DeleteTiledBg(0, 3);
    NF_DeleteTiledBg(1, 3);
    NF_UnloadTiledBg("Top");
    NF_UnloadTiledBg("Bottom");
    //NF_DeleteSprite(screen, id); 
    //NF_FreeSpriteGfx(screen, id)
    //NF_UnloadSpriteGfx(id);
    //NF_UnloadSpritePal(id); 
    NF_ResetSpriteBuffers(); 
    NF_ResetTiledBgBuffers();
    NF_InitSpriteSys(0); 
    NF_InitSpriteSys(1); 
}
