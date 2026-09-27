#ifndef GAME_AUDIO_H
#define GAME_AUDIO_H

#include <windows.h>
#include <mmsystem.h>
#include <cstring>
#pragma comment(lib, "winmm.lib")

const char* const GAME_AUDIO_ROOT = "iGraphics files (with Updated Keyboard Functions)\\Audios\\";

void playGameAudioChannel(const char* filename, const char* alias, bool loop = false)
{
    char command[1024];
    char audioPath[MAX_PATH];
    char fullAudioPath[MAX_PATH];
    sprintf_s(audioPath, sizeof(audioPath), "%s%s", GAME_AUDIO_ROOT, filename);
    GetFullPathNameA(audioPath, MAX_PATH, fullAudioPath, 0);

    char closeCommand[128];
    sprintf_s(closeCommand, sizeof(closeCommand), "close %s", alias);
    mciSendStringA(closeCommand, 0, 0, 0);
    sprintf_s(command, sizeof(command), "open \"%s\" type mpegvideo alias %s", fullAudioPath, alias);
    if (mciSendStringA(command, 0, 0, 0) != 0)
        return;
    sprintf_s(command, sizeof(command), loop ? "play %s repeat" : "play %s", alias);
    mciSendStringA(command, 0, 0, 0);
}

bool isGameAudioPlaying(const char* alias)
{
    char command[128];
    char status[64] = { 0 };
    sprintf_s(command, sizeof(command), "status %s mode", alias);
    if (mciSendStringA(command, status, sizeof(status), 0) != 0)
        return false;
    return strcmp(status, "playing") == 0;
}

void playGameAudio(const char* filename, bool loop = false)
{
    playGameAudioChannel(filename, "game_audio", loop);
}

void stopGameAudioChannel(const char* alias)
{
    char command[128];
    sprintf_s(command, sizeof(command), "stop %s", alias);
    mciSendStringA(command, 0, 0, 0);
    sprintf_s(command, sizeof(command), "close %s", alias);
    mciSendStringA(command, 0, 0, 0);
}

void stopGameAudio()
{
    stopGameAudioChannel("game_audio");
}

#endif
