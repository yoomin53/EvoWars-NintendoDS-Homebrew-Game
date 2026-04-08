//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#pragma once

#include "sprite.hpp"
#include "Enemy.hpp"
#include "Stage.hpp"
#include <nds.h>
#include <string>
#include "Save.h"

#include "bgm.h"
#include "soundbank.h"

using namespace std;

enum Character
{   
    LEIA,
    AIDEN,
    LINA
};

enum GameState
{
    STATE_PLAYING,
    STATE_PAUSED,
    STATE_WIN,
    STATE_LOSE
};

extern GameState gameState;

struct Base
{
    int damage;
    int accuracy;
    int criticalChance;
    int criticalDamage;
};

class Game
{

private:

    string username;

    int damage;
    int accuracy;
    int criticalChance;
    int criticalDamage;
    int agilityLevel; 
    int gold; 
    int sfxVolume; 
    int level; 

    inline static int HP; 
    const static int AidenHp = 90;
    const static int LinaHp = 120;
    const static int LeiaHp = 60;
    const static int AidenMuzzleFrame = 20; 
    const static int LinaMuzzleFrame = 10; 
    const static int LeiaMuzzleFrame = 30; 
    int stageID;
    int stageNum; 

    Base Leia{10, 0, 40, 40};
    Base Aiden{10, 20, 50, 50};
    Base Lina{10, 30, 60, 60};
    bool victory = false; 
    Stage stage;
    SaveData* saveData;

    void cleanUp();
    void TogglePause();

public:
    int goldEarned = 0; 

    Game(){}

    Game(string name, SaveData* save)
        : username(name),
          damage(save->attack_level),
          accuracy(save->accuracy_level*10),
          criticalChance(save->critical_chance_level),
          criticalDamage(save->critical_chance_level),
          agilityLevel(save->agility_level), 
          gold(save->gold), 
          sfxVolume(save->SFX*(!save->mute)), 
          level(save->level), 
          saveData(save)
    {}
    static void DealDamageToPlayer(int attack); 
    void StartStage(int _stageID, int stageNum);
    void RunStage(Stage* stage); 

    void AddGold(int amount)
    {
        gold += amount;
        if(saveData)
            saveData->gold = gold;
    }

    int GetGold() const
    {
        return gold;
    }
    void END();
};