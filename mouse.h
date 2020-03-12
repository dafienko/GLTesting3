#pragma once

#include "glMath.h"

int leftMouseDown;
int mouseLocked;

void initMouse(HANDLE);
void lockMouse();
void unlockMouse();
void hideMouse();
void showMouse();
vec2 getMouseDelta();
void clearMouseDelta();
void addMouseDelta(float, float);
