#pragma once


int* keysDown;

void initKeyboard();

void keyDown(int);
void keyUp(int);
int isKeyDown(int);
void swapKbdBuffer();
