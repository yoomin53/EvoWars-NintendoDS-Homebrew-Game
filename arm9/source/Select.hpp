//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#ifndef SELECT_HPP
#define SELECT_HPP

// Character enum은 별도 헤더나 common.h에 두는 게 좋지만, 여기 둔다면:
#include "Game.hpp"

class StageSelect {
public:
    StageSelect();
    void init(const char* _characterName, Character _charStage);
    void update();
    void render();
    void cleanUp();
    bool isFinished();
    int getSelectedStage() { return selectedStage; }

    bool exit;          // true면 B눌러서 메뉴로 완전 복귀
    Character charStage;
    Character unLockCharStage; 
    int unLockStage; 

private:
    bool finished;
    bool confirmed;
    int selectedStage;
    int maxStage;
    const char* characterName;
};

#endif
