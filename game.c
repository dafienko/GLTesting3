#include "ghettoWin.h"
#include <math.h>

#include "glMath.h"
#include "game.h"
#include "keyboard.h"
#include "renderer.h"
#include "consoleUtil.h"
#include "mouse.h"
#include "fileUtil.h"

float sensitivity = .2;

void initGame() {

}

void updateFrame(float dt) {
    vec2 delta = getMouseDelta();
    float dx = delta.x;
    float dy = delta.y;

    clearMouseDelta();

    if (!leftMouseDown) {
         ry += dt * (PI / 12.0);
    } else {
        rx += dy * sensitivity/50;
        ry += dx * sensitivity/50;
    }
}

float speed = 130;
void updateGame(float dt) {

    if (isKeyDown(0x46)) {// f
        drawMode = DM_FACEANDLINE;
    } else if (isKeyDown(0x47)) {// g
        drawMode = DM_LINE;
    } else if (isKeyDown(0x48)) { // h
        drawMode = DM_FACE;
    }


}

