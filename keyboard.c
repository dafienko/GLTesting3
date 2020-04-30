#include "ghettoWin.h"
#include "keyboard.h"
#include "consoleUtil.h"
#include <stdlib.h>

#define kbdBufferSize 50

int currentLength;

int* b1;
int* b2;
int* addBuffer;

void swapKbdBuffer() {
    int newLength = 0;

    for (int i = 0; i < kbdBufferSize; i++) {
        *(keysDown + i) = -1;

        if (*(addBuffer + i) != -1) {
            *(keysDown + newLength) = *(addBuffer + i);
            *(addBuffer + newLength) = *(addBuffer + i);
            newLength++;
        }
    }
    currentLength = newLength;
}


void initKeyboard() {
    currentLength = 0;
    b1 = calloc(kbdBufferSize, sizeof(int));
    b2 = calloc(kbdBufferSize, sizeof(int));
    for (int i = 0; i < kbdBufferSize; i++) {
        *(b1 + i) = -1;
        *(b2 + i) = -1;
    }

    keysDown = b1;
    addBuffer = b2;
}

void keyDown(int key) {

}

void keyUp(int key) {

}

int isKeyDown(int key) {
    return GetKeyState(key) < 0;
}
