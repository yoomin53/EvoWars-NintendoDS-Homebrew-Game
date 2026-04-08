//Copyright (c) 2026 Jung, Yoomin. All rights reserved.
#include "bgm.h"        // or audio.h depending on your name
#include <maxmod9.h> // Maxmod header for the ARM9
#include <nds.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <filesystem.h>

#include "soundbank.h"  // Header with IDs of songs and sound effects
#include "soundbank_bin.h" 

#define STREAM_BUF_SIZE 16384
//static char streamBuf[STREAM_BUF_SIZE];
static char *streamBuf = NULL;
static int streamIn = 0;   // write index
static int streamOut = 0;  // read index

static FILE *bgmFile = NULL;
static int wavDataOffset = 0;   // file offset where data begins (after header)
static bool bgmLoop = true;

static mm_stream_formats mmFormat = MM_STREAM_16BIT_STEREO;
static bool streamOpened = false;
//MelonDS 1.0 works but 1.1 has some errors. 


// WAV header structure (simple RIFF/WAV header)
typedef struct WAVHeader
{
    uint32_t chunkID;
    uint32_t chunkSize;
    uint32_t format;

    uint32_t subchunk1ID;
    uint32_t subchunk1Size;
    uint16_t audioFormat;
    uint16_t numChannels;
    uint32_t sampleRate;
    uint32_t byteRate;
    uint16_t blockAlign;
    uint16_t bitsPerSample;

    uint32_t subchunk2ID;
    uint32_t subchunk2Size;
} WAVHeader_t;

enum { RIFF_ID = 0x46464952, WAVE_ID = 0x45564157, FMT_ID = 0x20746d66, DATA_ID = 0x61746164 };

// helper to select maxmod format
static mm_stream_formats getMMStreamType(uint16_t numChannels, uint16_t bitsPerSample)
{
    if (numChannels == 1) {
        if (bitsPerSample == 8)  return MM_STREAM_8BIT_MONO;
        else                     return MM_STREAM_16BIT_MONO;
    } else {
        if (bitsPerSample == 8)  return MM_STREAM_8BIT_STEREO;
        else                     return MM_STREAM_16BIT_STEREO;
    }
}

// --------- Maxmod streaming callback (called inside interrupt) ---------
// Copies `length` samples (not bytes) to `dest`. Must use circular buffer.
static mm_word streamCallback(mm_word length, mm_addr dest, mm_stream_formats format)
{
    // convert sample count -> byte count
    size_t mul = 1;
    switch (format) {
        case MM_STREAM_8BIT_MONO:   mul = 1; break;
        case MM_STREAM_8BIT_STEREO: mul = 2; break;
        case MM_STREAM_16BIT_MONO:  mul = 2; break;
        case MM_STREAM_16BIT_STEREO:mul = 4; break;
        default: mul = 2; break;
    }
    size_t bytes = (size_t)length * mul;

    size_t bytes_until_end = STREAM_BUF_SIZE - streamOut;

    if (bytes_until_end >= bytes) {
        // simple copy
        memcpy((void*)dest, &streamBuf[streamOut], bytes);
        streamOut = (streamOut + bytes) % STREAM_BUF_SIZE;
    } else {
        // wrap around copy
        memcpy((void*)dest, &streamBuf[streamOut], bytes_until_end);
        memcpy((char*)dest + bytes_until_end, &streamBuf[0], bytes - bytes_until_end);
        streamOut = (bytes - bytes_until_end) % STREAM_BUF_SIZE;
    }

    return length;
}

// --------- Low-level read that loops/rewinds if needed ---------
// Reads exactly `size` bytes from bgmFile into buffer, looping if EOF & bgmLoop true.
static void readFileLooping(char *buffer, size_t size)
{
    if (!bgmFile) return;

    while (size > 0) {
        size_t got = fread(buffer, 1, size, bgmFile);
        buffer += got;
        size -= got;

        if (got == 0) {
            // EOF
            if (!bgmLoop) {
                // fill remaining with silence
                memset(buffer, 0, size);
                break;
            }
            // rewind to data start (skip header)
            fseek(bgmFile, wavDataOffset, SEEK_SET);
        }
    }
}

// --------- Fill circular buffer with available free space ---------
static void streamingFillBuffer(bool force_fill)
{
    // compute free space in ring buffer (leave one byte to differentiate full/empty)
    int freeSpace;
    if (streamIn >= streamOut) {
        freeSpace = STREAM_BUF_SIZE - (streamIn - streamOut) - 1;
    } else {
        freeSpace = (streamOut - streamIn) - 1;
    }

    if (!force_fill && freeSpace == 0) return;

    while (freeSpace > 0) {
        // how many bytes can we write before wrapping
        int contiguous = (streamIn < streamOut) ? (streamOut - streamIn - 1) : (STREAM_BUF_SIZE - streamIn);
        int toRead = (contiguous < freeSpace) ? contiguous : freeSpace;
        if (toRead <= 0) break;

        readFileLooping(&streamBuf[streamIn], (size_t)toRead);
        streamIn += toRead;
        if (streamIn >= STREAM_BUF_SIZE) streamIn -= STREAM_BUF_SIZE;

        if (streamIn >= streamOut)
            freeSpace = STREAM_BUF_SIZE - (streamIn - streamOut) - 1;
        else
            freeSpace = (streamOut - streamIn) - 1;

        // if not force_fill, break after single chunk to avoid hogging CPU
        if (!force_fill) break;
    }
}

// --------- Public API ---------

// Initialize maxmod + NitroFS. Call once at startup.
void Audio_Init(void)
{
    mm_ds_system sys = {0};
    sys.fifo_channel = FIFO_MAXMOD;
    mmInit(&sys);

    // initialize NitroFS once
    nitroFSInit(NULL);

    // soundbank Load (SFX on memory)
    //mmLoad(soundbank_bin);
    mmInitDefaultMem((mm_addr)soundbank_bin);

    // ★★★ 힙에 버퍼 할당 ★★★
    if (streamBuf == NULL) {
        streamBuf = (char*)malloc(STREAM_BUF_SIZE);
        if (streamBuf == NULL) {
            // 메모리 부족 시 처리 (필요하면 에러 출력)
            printf("Failed to allocate stream buffer!\n");
        }
    }

    // reset indices
    streamIn = streamOut = 0;
    bgmFile = NULL;
    streamOpened = false;
}


void Audio_Load_SFX(void)
{   
    mmLoadEffect(SFX_GUNSHOT);
    mmLoadEffect(SFX_CRASH);
    mmLoadEffect(SFX_FAIL);
    mmLoadEffect(SFX_FIRE_EXPLOSION);
    mmLoadEffect(SFX_SELECT);
    mmLoadEffect(SFX_SUCCESSFUL);
}

void Audio_Unload_SFX(void)
{
    mmUnloadEffect(SFX_GUNSHOT);
    mmUnloadEffect(SFX_CRASH);
    mmUnloadEffect(SFX_FAIL);
    mmUnloadEffect(SFX_FIRE_EXPLOSION);
    mmUnloadEffect(SFX_SELECT);
    mmUnloadEffect(SFX_SUCCESSFUL);
}


// SFX 재생 함수 추가
void Audio_PlaySFX(int sfx_id)
{
    mmEffect(sfx_id);                    // 기본 재생 (볼륨 255, 중앙)
}

void Audio_PlaySFXEx(int sfx_id, int volume, int pan)
{   /*
    mm_sfxhand h = mmEffect(sfx_id);
    mmEffectVolume(h, volume);           // 0~255
    mmEffectPanning(h, pan);             // 0~255 (128 = 중앙)
    */
   // Play sound at half playback rate, 200/255 volume, and center panning
    mm_sound_effect sound;
    sound.id      = sfx_id; // Sample ID (make sure it is loaded)
    sound.rate    = 1024;     // Playback rate, 1024 = original sound
    sound.handle  = 0;           // 0 = allocate new handle
    sound.volume  = volume;         // 200/255 volume level
    sound.panning = pan;         // Centered panning
    
    mmEffectEx(&sound);
}

// Play WAV at path (e.g. "nitro:/title.wav"). If loop == true it will loop.
void Audio_PlayBGM(const char *path, bool loop)
{
    // stop existing stream if any
    if (streamOpened) {
        mmStreamClose();
        streamOpened = false;
    }
    if (bgmFile) {
        fclose(bgmFile);
        bgmFile = NULL;
    }

    bgmLoop = loop;

    bgmFile = fopen(path, "rb");
    if (!bgmFile) return;

    // read header
    WAVHeader_t hdr;
    if (fread(&hdr, 1, sizeof(hdr), bgmFile) != sizeof(hdr)) {
        fclose(bgmFile);
        bgmFile = NULL;
        return;
    }

    // basic header checks (optional)
    if (hdr.chunkID != RIFF_ID || hdr.format != WAVE_ID || hdr.subchunk1ID != FMT_ID || hdr.subchunk2ID != DATA_ID) {
        // header invalid - try to continue but set reasonable defaults
        // for safety set sampleRate to 22050 and 16-bit stereo
        hdr.sampleRate = hdr.sampleRate ? hdr.sampleRate : 22050;
        hdr.numChannels = hdr.numChannels ? hdr.numChannels : 2;
        hdr.bitsPerSample = hdr.bitsPerSample ? hdr.bitsPerSample : 16;
    }

    // remember where data starts so we can rewind
    wavDataOffset = sizeof(WAVHeader_t);

    // clear / reset ring buffer
    streamIn = streamOut = 0;
    if (streamBuf) { 
    memset(streamBuf, 0, STREAM_BUF_SIZE);
    } 

    // select maxmod format
    mmFormat = getMMStreamType(hdr.numChannels, hdr.bitsPerSample);

    // prefill buffer fully (force)
    streamingFillBuffer(true);

    // configure and open mm stream
    mm_stream stream = {
        .sampling_rate = hdr.sampleRate,
        .buffer_length = 2048,
        .callback      = streamCallback,
        .format        = mmFormat,
        .timer         = MM_TIMER0,
        .manual        = false
    };
    mmStreamOpen(&stream);
    streamOpened = true;
}

// Must be called from your main loop (once per frame). It refills buffer gradually.
// This keeps file I/O out of interrupts and avoids stalls.
void Audio_Update(void)
{
    if (!bgmFile || !streamOpened) return;

    // Fill a bit each frame — don't block for too long
    streamingFillBuffer(false);
}

// Stop playback & cleanup
void Audio_StopBGM(void)
{
    if (streamOpened) {
        mmStreamClose();
        streamOpened = false;
    }
    swiWaitForVBlank();
    if (bgmFile) {
        fclose(bgmFile);
        bgmFile = NULL;
    }
    streamIn = streamOut = 0;
    if (streamBuf) {
        memset(streamBuf, 0, STREAM_BUF_SIZE);
    }

    if (streamBuf) {
        free(streamBuf);
        streamBuf = NULL;
    }

    //soundDisable();                     // 사운드 전체 끄기
}
