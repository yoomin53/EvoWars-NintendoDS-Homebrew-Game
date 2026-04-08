//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "Save.h"
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <nds.h>
#include <fat.h>

void Save_InitNew(SaveData* data)
{
    if (!data)
        return;

    memset(data, 0, sizeof(SaveData));

    data->version = SAVE_VERSION;
    data->level = 1;
    data->hp = 100;
    data->gold = 0;
    (*data).attack_level = 1;
    (*data).accuracy_level = 1;  
    data->agility_level = 1; 
    data->critical_chance_level = 1; 
    data->mute = false;
    data->debug = false;
    data->SFX = 255;
    data->difficulty = 0;
}

bool Save_Exists(void)
{
    FILE* file = fopen(SAVE_FILENAME, "rb");
    if (!file)
        return false;

    fclose(file);
    return true;
}

bool Save_Write(const SaveData* data)
{
    if (!data)
    {
        printf("Save_Write: data NULL\n");
        return false;
    }
    
    char *cwd = fatGetDefaultCwd(); 
    char path[512]; 
    snprintf(path, sizeof(path), "%s/%s", cwd, SAVE_FILENAME);   
    FILE* file = fopen(path, "wb");
    free(cwd);
    //FILE* file = fopen(SAVE_FILENAME, "wb");

    if (!file)
    {
        perror("fopen (write)");
        return false;
    }

    size_t written = fwrite(data, sizeof(SaveData), 1, file);
    //fflush(file); 
    fsync(fileno(file)); 

    if (written != 1)
    {
        perror("fwrite");
        fclose(file);
        return false;
    }

    if (fclose(file) != 0)
    {
        perror("fclose (write)");
        return false;
    }

    printf("Save written successfully!\n");
    return true;
}

bool Save_Read(SaveData* data)
{   
    if (!data)
    {
        printf("Save_Read: data NULL\n");
        return false;
    }

    FILE* file = fopen(SAVE_FILENAME, "rb");
    if (!file)
    {
        perror("fopen (read)");
        return false;
    }
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);
    printf("Actual file size = %ld\n", size);
    size_t read = fread(data, sizeof(SaveData), 1, file);
    printf("sizeof(SaveData) = %zu\n", sizeof(SaveData));
    printf("Read blocks: %u\n", (unsigned)read);
    printf("Read version: %lu\n", (unsigned long)data->version);
    printf("Expected version: %u\n", SAVE_VERSION);
    printf("Save_Read called\n");
    if (read != 1)
    {
        perror("fread");
        fclose(file);
        return false;
    }

    if (fclose(file) != 0)
    {
        perror("fclose (read)");
        return false;
    }

    if (data->version != SAVE_VERSION)
    {
        printf("Save version mismatch!\n");
        return false;
    }

    printf("Save loaded successfully!\n");
    return true;
}


static PrintConsole topScreen;

void ShowFatalError(const char* msg)
{
    consoleClear();
    printf("FATAL ERROR\n\n");
    printf("%s\n\n", msg);
    printf("Press START to exit.\n");

    while (1)
    {
        swiWaitForVBlank();
        scanKeys();
        if (keysDown() & KEY_START)
            break;
    }

    while (1);
}

void System_Init(void)
{
    videoSetMode(MODE_0_2D);
    vramSetBankA(VRAM_A_MAIN_BG);

    consoleInit(&topScreen,
                0,
                BgType_Text4bpp,
                BgSize_T_256x256,
                31,
                0,
                true,
                true);

    consoleSelect(&topScreen);

    printf("Initializing FAT...\n\n");

    if (!fatInitDefault())
    {
        perror("fatInitDefault");
        ShowFatalError("FAT initialization failed.");
    }

    printf("FAT initialized successfully!\n\n");
}
