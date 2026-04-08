//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#pragma once
#include <nds.h>
#include <string>
#include <vector>

class Stage; 

enum EnemyState {
    ENEMY_APPROACH,
    ENEMY_ATTACK,
    ENEMY_JUMP,
    ENEMY_DODGE,
    ENEMY_CHASE
};

struct EnemyInput {
    int aimX;
    int aimY;
    bool playerTouch;
    int shotX; 
    int shotY;
    bool shot; 
    int playerX; 
    int damage; 
};

class Enemy {
public:
    void Init(
        int startX,
        int startY,
        int hp,
        int attack,
        int agility,
        const char* _enemy, 
        Stage* _stage, 
        int goldValue 
    );

    void Update(const EnemyInput& input);

    bool IsAlive() const { return hp > 0; }
    bool didAttack = false; 
    void CleanUp(); 

    Stage* stage;
    int gold;  
    bool freed = false; 

private:
    // Stats
    int maxHp; 
    int hp;
    int attack;
    int agility;
    //std::string enemy; 
    const char* enemy; 


    // Fake 3D
    int x;
    int y;
    int z;
    int scale;

    // Sprite IDs
    int ramId = 59;
    int vramId;
    int rotId;

    // AI
    EnemyState state;
    int stateTimer;
    int dodgeDirX = 0;
    int dodgeSpeed = 0;

    //int deathTimer = 2; // 🔥 1프레임 딜레이

    // Internal logic
    void DecideState(const EnemyInput& input);
    void ActApproach(const EnemyInput& input);
    void ActAttack(const EnemyInput& input);
    void ActJump();
    void ActDodge();
    void ActChase();
    void UpdateVisual(const EnemyInput& input);
};
