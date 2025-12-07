#include "music.h"
#include <graphics.h>
#include <windows.h>
#include <mmsystem.h>
#include <cmath>
#include <iostream>
#pragma comment(lib, "winmm.lib")
#include <stdio.h>

// Use a safe, English filename. 
// User must place "bgm.mp3" in the same directory as the executable.
const char* musicFile = "bgm.mp3";

void playMusic() {
    mciSendString("close mymusic", NULL, 0, NULL);
    char cmd[256];
    sprintf(cmd, "open \"%s\" alias mymusic", musicFile);
    
    int err = mciSendString(cmd, NULL, 0, NULL);
    if (err == 0) {
        mciSendString("play mymusic repeat", NULL, 0, NULL); // Added 'repeat' for looping
    } else {
        // Try absolute path or report error (console only)
        std::cerr << "Error opening music file: " << musicFile << " (MCI Error: " << err << ")" << std::endl;
        
        // Fallback: Try looking in ../bgm.mp3 in case we are in bin/
        sprintf(cmd, "open \"../%s\" alias mymusic", musicFile);
        if (mciSendString(cmd, NULL, 0, NULL) == 0) {
             mciSendString("play mymusic repeat", NULL, 0, NULL);
        }
    }
}

void stopMusic() {
    mciSendString("stop mymusic", NULL, 0, NULL);
    mciSendString("close mymusic", NULL, 0, NULL);
}

static void drawSpeakerIcon(int cx, int cy, int size, bool isPlaying) {
    setfillcolor(isPlaying ? RGB(70, 130, 180) : RGB(150, 150, 150));
    setlinecolor(RGB(0, 0, 0));
    setlinestyle(PS_SOLID, 2);

    POINT triangle[4] = {
        {cx - size / 2, cy - size / 3},
        {cx + size / 2, cy},
        {cx - size / 2, cy + size / 3},
        {cx - size / 2, cy - size / 3}
    };
    fillpolygon(triangle, 4);

    setfillcolor(isPlaying ? RGB(100, 160, 210) : RGB(180, 180, 180));
    solidrectangle(cx + size / 2 - 5, cy - size / 4, cx + size / 2 + size / 3, cy + size / 4);

    if (isPlaying) {
        setlinecolor(RGB(70, 130, 180));
        setlinestyle(PS_SOLID, 3);
        for (int i = 0; i < 3; i++) {
            arc(cx + size / 2 + size / 3 + i * 8, cy - size / 2 + i * 5,
                cx + size / 2 + size / 3 + 30 + i * 15, cy + size / 2 - i * 5,
                2.356, -0.785);
        }
    }
}

static bool isInCircleButton(int mx, int my, int cx, int cy, int radius) {
    int dx = mx - cx;
    int dy = my - cy;
    return (dx * dx + dy * dy <= radius * radius);
}

// MusicPlayer implementation
MusicPlayer::MusicPlayer(int x, int y, int r)
    : posX(x), posY(y), radius(r), isPlaying(false), isHovering(false) {
}

void MusicPlayer::Draw() {
    if (isPlaying) {
        setfillcolor(RGB(200, 230, 255));
    }
    else {
        setfillcolor(RGB(240, 240, 240));
    }

    if (isHovering) {
        setlinecolor(RGB(100, 100, 255));
        setlinestyle(PS_SOLID, 4);
    }
    else {
        setlinecolor(RGB(0, 0, 0));
        setlinestyle(PS_SOLID, 3);
    }

    fillcircle(posX, posY, radius);
    drawSpeakerIcon(posX, posY, radius, isPlaying);
}

bool MusicPlayer::HandleMouseInput(int mouseX, int mouseY, int messageType) {
    bool isMouseInButton = isInCircleButton(mouseX, mouseY, posX, posY, radius);
    switch (messageType) {
    case WM_MOUSEMOVE:
        isHovering = isMouseInButton;
        return isMouseInButton;
    case WM_LBUTTONDOWN:
        if (isMouseInButton) {
            if (isPlaying) { stopMusic(); isPlaying = false; } 
            else { playMusic(); isPlaying = true; }
            return true;
        }
        return false;
    default:
        return false;
    }
}

bool MusicPlayer::IsPlaying() { return isPlaying; }
void MusicPlayer::SetPosition(int x, int y) { posX = x; posY = y; }
void MusicPlayer::Cleanup() { if (isPlaying) stopMusic(); }