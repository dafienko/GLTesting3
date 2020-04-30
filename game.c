#include "ghettoWin.h"
#include <math.h>

#include "glMath.h"
#include "game.h"
#include "keyboard.h"
#include "renderer.h"
#include "consoleUtil.h"
#include "mouse.h"
#include "fileUtil.h"


void initGame() {
    //lockMouse();
   // hideMouse();

    //getMeshData("assets/models/monkey.obj");
}

float sensitivity = .2;

void updateFrame(float dt) {
    vec2 delta = getMouseDelta();
    float dx = delta.x;
    float dy = delta.y;

    clearMouseDelta();

    camera->rotation.x += rad(-dy * sensitivity);
    camera->rotation.y += rad(-dx * sensitivity);

    camera->rotation.x = min(max(camera->rotation.x, rad(-80)), rad(80));
}

float speed = 130;
void updateGame(float dt) {
    ;
    float xDir = 0;
    float zDir = 0;
    float yDir = 0;

    if (isKeyDown(0x57)) { // w
        zDir += -1;
    }
    if (isKeyDown(0x53)) { // s
        zDir += 1;
    }

    if (isKeyDown(0x41)) { // a
        xDir += -1;
    }
    if (isKeyDown(0x44)) { // d
        xDir += 1;
    }

    if (isKeyDown(0x45)) { // e
        yDir += 1;
    }
    if (isKeyDown(0x51)) { // q
        yDir += -1;
    }

    vec3 rightVector = identityVec3;
    rightVector.y = 0;
    rightVector.x = cos(camera->rotation.y);
    rightVector.z = -sin(camera->rotation.y);

    vec3 lookVector = identityVec3;
    lookVector.y = sin(camera->rotation.x);
    lookVector.x = cos((camera->rotation.y) + rad(90));
    lookVector.z = -sin((camera->rotation.y) + rad(90));

    vec3 upVector = identityVec3;
    upVector.y = yDir;

    vec3 movementVector = addVec3(mulVec3(rightVector, xDir), mulVec3(lookVector, -zDir));
    movementVector = addVec3(upVector, movementVector);
    float m = vec3Magnitude(movementVector);

    if (m != 0) {
        movementVector = normalizeVec3(movementVector);
    }

    movementVector = mulVec3(movementVector, speed * dt);

    float speedModifier = 1;
    if (isKeyDown(VK_SHIFT)) {
        speedModifier = .05;
    }

    vec3 pos = camera->position;
    camera->position = addVec3(pos, mulVec3(movementVector, speedModifier));

    if (isKeyDown(VK_UP)) {
        camera->rotation.x += dt * sensitivity * 10;
    }
    if (isKeyDown(VK_DOWN)) {
        camera->rotation.x += dt * -sensitivity * 10;
    }
    camera->rotation.x = min(max(camera->rotation.x, rad(-80)), rad(80));

    if (isKeyDown(VK_RIGHT)) {
        camera->rotation.y += dt * -sensitivity * 10;
    }
    if (isKeyDown(VK_LEFT)) {
        camera->rotation.y += dt * sensitivity * 10;
    }

    drawMode = DM_FACE;
    if (isKeyDown(0x46)) {// f
        drawMode = DM_FACEANDLINE;
    }
    if (isKeyDown(0x47)) {// g
        drawMode = DM_LINE;
    }

    if (isKeyDown(VK_OEM_MINUS)) { // -
        lineThickness -= .5;
        lineThickness = max(lineThickness, .5);
    }

    if (isKeyDown(VK_OEM_PLUS)) { // =
        lineThickness += .5;
        lineThickness = min(lineThickness, 160);
    }
}

