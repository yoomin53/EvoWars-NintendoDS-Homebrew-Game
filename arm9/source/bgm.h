//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#ifndef BGM_H
#define BGM_H
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

void Audio_Init(void);
void Audio_Load_SFX(void); 
void Audio_Unload_SFX(void); 
void Audio_PlayBGM(const char *path, bool loop);
void Audio_Update(void); 
void Audio_StopBGM(void);
void Audio_PlaySFX(int sfx_id);           // 간단 버전
void Audio_PlaySFXEx(int sfx_id, int volume, int pan);

#ifdef __cplusplus
}
#endif

#endif
