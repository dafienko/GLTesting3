#pragma once
#include <gl/gl.h>

#include "ghettoWin.h"
#include "glMath.h"
#include "fileUtil.h"

#define DM_FACE 0
#define DM_FACEANDLINE 1
#define DM_LINE 2

#define MAX_INSTANCES 100

int drawMode;

typedef struct instance {
    MESH* mesh;
    vec3 position;
    vec3 rotation;
    vec3 scale;
    float transparency;
    GLuint vao;
    GLuint vbo[2];
    struct instance* parent;
    char* name;
    int expanded;
    int selected;
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
INSTANCE** instances;

int addInstance(INSTANCE* inst);
void removeInstance(int instanceHandle);
void updateSize(int width, int height);
int frameTick(HWND);
void display(CAMERA*, HDC, HWND);
void initRenderer(const char*);
INSTANCE* createInstanceFromFile(const char* );
