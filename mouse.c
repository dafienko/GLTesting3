#include "mouse.h"
#include "windows.h"

mouseLocked = 0;

vec2 mouseDelta = {0, 0};

void addMouseDelta(float x, float y) {
    mouseDelta.x += x;
    mouseDelta.y += y;
}

void lockMouse() {
    mouseLocked = 1;
}


void unlockMouse() {
    mouseLocked = 0;
}


void hideMouse() {
    ShowCursor(FALSE);
}


void showMouse() {
    ShowCursor(TRUE);
}


vec2 getMouseDelta() {
    return mouseDelta;
}


void clearMouseDelta() {
    mouseDelta.x = 0;
    mouseDelta.y = 0;
}

