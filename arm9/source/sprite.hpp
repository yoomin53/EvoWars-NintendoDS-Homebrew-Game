//Copyright (c) 2026 Jung, Yoomin. All rights reserved.

#ifndef SPRITE_HPP
#define SPRITE_HPP

#pragma once
#include <string>
#include <nds.h>

extern int AIM_SIZE; 



void MovePlayerSprites(); 
void InitSpriteGrid(const std::string& baseNSame, int _maxHP, int agility, int _accuracy);
bool UpdateSpriteGrid(bool touching, touchPosition touch, int shotX, int shotY, int& HP);
void DrawSpriteGrid();
void CleanSpriteGrid();
void ShowPauseMenu(); 
void HidePauseMenu();
int GetPlayerX();

#endif
