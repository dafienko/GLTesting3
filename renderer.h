#include <windows.h>
#include "glMath.h"

typedef struct model {
    float* verts;
    vec3* rotation;
    vec3* position;
} model;

mat4 perspectiveMat;

typedef struct color {
        float x;
        float y;
        float z;
} color3f;


void updateSize(int width, int height);
void frameTick(HWND);
void display(HDC, HWND);
void init();
