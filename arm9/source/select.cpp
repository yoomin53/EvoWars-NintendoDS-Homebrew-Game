//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include <stdio.h>
#include <nds.h>
#include <filesystem.h>
#include <nf_lib.h>
#include "Select.hpp"
#include "common.h" // Audio 함수, SFX 상수

bool StageSelect::isFinished() {
    if (finished) {
        finished = false;
        return true;
    }
    return false;
}

StageSelect::StageSelect()
    : finished(false), confirmed(false), selectedStage(1),
      maxStage(10), exit(false), charStage(LEIA), characterName(nullptr) {}

void StageSelect::init(const char* _characterName, Character _charStage) {
    NF_Set2D(0, 0);
    NF_Set2D(1, 0);
    swiWaitForVBlank();

    characterName = _characterName;
    charStage = _charStage;       // 버그 수정: _charStage (원래 코드는 잘못된 변수명 사용)
    selectedStage = 1;            // init마다 1로 리셋
    confirmed = false;
    finished = false;

    char path[64];

    // --- 버퍼 초기화는 한 번만 ---
    NF_InitTiledBgBuffers();
    NF_InitTiledBgSys(0);
    NF_InitTiledBgSys(1);

    // --- Top Screen 배경 ---
    snprintf(path, sizeof(path), "bg/%sSelectStageTop", characterName);
    NF_LoadTiledBg(path, "selTop", 256, 256);
    NF_CreateTiledBg(0, 3, "selTop");

    // --- Bottom Screen 배경 ---
    snprintf(path, sizeof(path), "bg/%sSelectStageBottom", characterName);
    NF_LoadTiledBg(path, "selBot", 256, 256);
    NF_CreateTiledBg(1, 3, "selBot");
    NF_ScrollBg(1, 3, 0, 63);

    // --- Text16 시스템 ---
    NF_InitTextSys(1);
    NF_LoadTextFont16("fnt/font16", "selfont", 256, 256, 0);
    NF_CreateTextLayer16(1, 0, 0, "selfont");
    NF_UpdateTextLayers();
}

#define BTN_BACK_X1  180
#define BTN_BACK_Y1  0 //adjust for scroll (-56)
#define BTN_BACK_X2  255
#define BTN_BACK_Y2  30

void StageSelect::update() {
    swiWaitForVBlank();
    scanKeys();
    int pressed = keysDown();
    touchPosition touch;
    touchRead(&touch);

    // --- 오른쪽: 스테이지 +1, 최대치 넘으면 다음 캐릭터로 ---
    if (pressed & (KEY_RIGHT | KEY_UP)) {
        if ((selectedStage < maxStage && charStage < unLockCharStage) | (selectedStage < unLockStage)) {
            selectedStage++;
            Audio_PlaySFX(SFX_SELECT);
        } else if (charStage < LINA && (Character)(charStage+1)<=unLockCharStage) {
            // 마지막 스테이지에서 오른쪽 -> 다음 캐릭터 챕터로 이동
            Audio_PlaySFX(SFX_SELECT);
            charStage = (Character)(charStage + 1);
            selectedStage = 0; 
            finished = true;
            return;
        } else {
            // LINA의 마지막 스테이지: 더 갈 곳 없음
            Audio_PlaySFX(SFX_FAIL);
        }
    }

    // --- 왼쪽: 스테이지 -1, 1 미만이면 이전 캐릭터로 ---
    if (pressed & (KEY_LEFT | KEY_DOWN)) {
        if (selectedStage > 1) {
            selectedStage--;
            Audio_PlaySFX(SFX_SELECT);
        } else if (charStage == LEIA) {
            // 첫 캐릭터의 스테이지 1: 더 뒤로 못감
            Audio_PlaySFX(SFX_FAIL);
        } else {
            // 이전 캐릭터 챕터로 이동
            Audio_PlaySFX(SFX_SELECT);
            charStage = (Character)(charStage - 1);
            selectedStage = 0;
            finished = true;
            return;
        }
    }

    // --- A 버튼: 스테이지 확정 ---
    if (pressed & KEY_A) {
        Audio_PlaySFX(SFX_SUCCESSFUL);
        confirmed = true;
    }

    // --- B 버튼: 메뉴로 돌아가기 ---
    if (pressed & KEY_B | ((pressed & KEY_TOUCH)&&touch.px >= BTN_BACK_X1 && touch.px <= BTN_BACK_X2 &&
            touch.py >= BTN_BACK_Y1 && touch.py <= BTN_BACK_Y2)) {
        Audio_PlaySFX(SFX_FAIL);
        exit = true;
        finished = true;
        return;
    }

    if (confirmed) {
        finished = true;
    }
}

void StageSelect::render() {
    if(selectedStage == 0) return; 
    NF_ClearTextLayer16(1, 0);
    // 캐릭터 이름 + 스테이지
    char buf[48];
    snprintf(buf, sizeof(buf), "< %s  Stage %d >", characterName, selectedStage);

    int len = 0;
    for (int i = 0; buf[i] != '\0'; i++) len++;
    int textX = (32 - len) / 2;
    int textY = 5;

    NF_WriteText16(1, 0, textX, textY, buf);

    // 안내 텍스트
    NF_WriteText16(1, 0, 7, 6, "A: Select  B: Back");

    NF_UpdateTextLayers();
    swiWaitForVBlank();
}

void StageSelect::cleanUp() {
    swiWaitForVBlank();
    confirmed = false;

    NF_DeleteTiledBg(0, 3);
    NF_DeleteTiledBg(1, 3);
    NF_ClearTextLayer16(1, 0);
    NF_UnloadTextFont("selfont");
    NF_UnloadTiledBg("selTop");
    NF_UnloadTiledBg("selBot");

    NF_InitTextSys(1);
    NF_ResetTiledBgBuffers();

    swiWaitForVBlank();
}
