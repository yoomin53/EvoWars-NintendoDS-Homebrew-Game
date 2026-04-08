//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#ifndef VIDEO_H
#define VIDEO_H

/**
 * @file video.h
 * @brief Reusable dual-screen 8-bit bitmap video player for NDS (NightFox Lib)
 *
 * Plays any number of frames from:
 *   <baseFolder>/Top/frame0.bmp, frame1.bmp, ...
 *   <baseFolder>/Bottom/frame0.bmp, frame1.bmp, ...
 *
 * All files must be 8-bit BMPs (256×192) with embedded palette.
 * Press START to exit playback.
 */

#include <nds.h>
#include <nf_lib.h>
#include <filesystem.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Play a dual-screen 8-bit video from the given folder.
 *
 * @param baseFolder   Path inside NITROFS, e.g. "video/Intro"
 *                     Looks for:
 *                       <baseFolder>/Top/frame0.bmp
 *                       <baseFolder>/Bottom/frame0.bmp
 *                       ...
 * @return 0 on success
 *        -1 if folder path is invalid
 *        -2 if no frames found or Top/Bottom mismatch
 */
int playVideoDual(const char *baseFolder);

#ifdef __cplusplus
}
#endif

#endif // VIDEO_H
