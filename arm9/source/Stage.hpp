//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#pragma once

#include <vector>
#include "Enemy.hpp"
#include <memory>


struct PlayerInput
{
    int touchX;
    int touchY;
    bool touching;
    int shotX;
    int shotY;
    bool shot;
    int playerX; 
    int damage; 
};

struct SpawnEvent
{
    int frame;
    int type;
    int x;
    int y;
};

class Game;

 
void OnEnemyKilled(int goldAmount);

class Stage
{
private:
    Game* game = nullptr;
    const SpawnEvent* events;   // 스폰 테이블
    int eventCount;

    int frame;

    std::vector<int> freeSlots;
    //std::vector<Enemy> enemies;
    std::vector<std::unique_ptr<Enemy>> enemies;
    std::vector<SpawnEvent> pendingSpawns;
    void SpawnEnemy(const SpawnEvent& e);

public:
    int clearGold; 
    Stage();

    void LoadStage(int character, int stageNum);

    void Init();

    void Update(PlayerInput input);

    bool IsClear();
    void FreeSlot(int slot);
    int AllocateSlot();
    void SetGame(Game* g) { game = g; }
    

    void OnEnemyKilled(int goldAmount);
};