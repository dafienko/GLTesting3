#include <windows.h>
#include <math.h>

#include "game.h"
#include "keyboard.h"
#include "renderer.h"
#include "consoleUtil.h"

void initGame() {

}

void updateFrame(float dt) {

}

float speed = .00002;
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
        yDir += -1;
    }
    if (isKeyDown(0x51)) { // q
        yDir += 1;
    }

    vec3 rightVector = identityVec3;
    rightVector.y = 0;
    rightVector.x = cos(camera->rotation->y);
    rightVector.z = -sin(camera->rotation->y);

    vec3 lookVector = identityVec3;
    lookVector.y = sin(camera->rotation->x);
    lookVector.x = cos((camera->rotation->y) + rad(90));
    lookVector.z = -sin((camera->rotation->y) + rad(90));

    vec3 upVector = identityVec3;
    upVector.y = yDir * speed;

    vec3 movementVector = addVec3(mulVec3(rightVector, xDir), mulVec3(lookVector, -zDir));
    movementVector = addVec3(upVector, movementVector);
    float m = vec3Magnitude(movementVector);
    if (m != 0) {
        movementVector = normalizeVec3(movementVector);
    }

    movementVector = mulVec3(movementVector, speed * dt);

    vec3 pos = *(camera->position);
    *(camera->position) = addVec3(pos, movementVector);

    if (isKeyDown(VK_UP)) {
        camera->rotation->x += dt * -.00001;
    }
    if (isKeyDown(VK_DOWN)) {
        camera->rotation->x += dt * .00001;
    }
    camera->rotation->x = min(max(camera->rotation->x, rad(-80)), rad(80));

    if (isKeyDown(VK_RIGHT)) {
        camera->rotation->y += dt * -.00001;
    }
    if (isKeyDown(VK_LEFT)) {
        camera->rotation->y += dt * .00001;
    }


}

