#pragma once
#include <gl/gl.h>

#include "ghettoWin.h"
#include "glMath.h"
#include "fileUtil.h"

#define DM_FACE 0
#define DM_FACEANDLINE 1
#define DM_LINE 2

float rx, ry;
float zoom;

int drawMode;

typedef struct instance {
    MESH* mesh;
    vec3 position;
    vec3 rotation;
    vec3 scale;
    float transparency;
    GLuint vao;
    GLuint vbo[2];
} INSTANCE;

typedef struct camera {
    vec3 position;
    vec3 rotation;
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
void initRenderer(const char*);
INSTANCE* createInstanceFromFile(const char* );
