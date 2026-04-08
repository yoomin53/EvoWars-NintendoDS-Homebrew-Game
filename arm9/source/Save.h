//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#ifndef SAVE_H
#define SAVE_H

#include <stdbool.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SAVE_FILENAME  "Evowars.sav"
#define SAVE_VERSION   1

typedef struct
{
    uint32_t version;
    uint32_t level;
    uint32_t hp;
    //Shop
    uint32_t gold;
    uint32_t attack_level;
    uint32_t accuracy_level;  
    uint32_t agility_level;
    uint32_t critical_chance_level; 
    //settings
    uint32_t mute;
    uint32_t debug;
    uint32_t SFX;
    uint32_t difficulty;

} SaveData;

#pragma pack(push, 1)

#pragma pack(pop)
void Save_InitNew(SaveData* data);
bool Save_Exists(void);
bool Save_Write(const SaveData* data);
bool Save_Read(SaveData* data);
void ShowFatalError(const char* msg); 
void System_Init(void); 

#ifdef __cplusplus
}
#endif

#endif