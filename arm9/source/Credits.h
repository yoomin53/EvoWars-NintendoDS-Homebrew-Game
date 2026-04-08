//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#ifndef CREDITS_H
#define CREDITS_H

#include <nds.h>
#include <nf_lib.h>
#include <filesystem.h>

#ifdef __cplusplus
extern "C" {
#endif

    void playCredits(void); 
    static void cleanUpCredits(void); 
    static void initCredits(void); 
    static void loadCredits(void); 

#ifdef __cplusplus
}
#endif

#endif // VIDEO_H
