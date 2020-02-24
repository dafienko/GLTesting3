#pragma once

#include <windows.h>
#include "glMath.h"

typedef struct mesh {
    float* verts;
    vec3* rotation;
    vec3* position;
} MESH;

typedef struct camera {
    vec3* position;
    vec3* rotation;
} CAMERA;

mat4 perspectiveMat;

typedef struct color {
        float x;
        float y;
        float z;
} color3f;

CAMERA* camera;

void updateSize(int width, int height);
int frameTick(HWND);
void display(CAMERA*, HDC, HWND);
void initRenderer();
