#include "consoleUtil.h"

#define identityMatrix ((mat4){1, 0, 0, 0,     0, 1, 0, 0,     0, 0, 1, 0,     0, 0, 0, 1})
#define PI 3.14159265359


typedef struct m4 {
    float a, b, c, d,
        e, f, g, h,
        i, j, k, l,
        m, n, o, p;
} mat4;

typedef struct v3 {
    float x, y, z;
} vec3;

typedef struct v4 {
    float x, y, z, w;
} vec4;

float rad(float deg);

vec4 mulVec(mat4, vec4);
mat4 mulMat(mat4, mat4);
mat4 fromTranslation(float, float, float);

mat4 rotateXYZ(float, float, float);
mat4 rotateX(float);
mat4 rotateY(float);
mat4 rotateZ(float);

mat4 fromPositionAndRotation(vec3, vec3);

void getPerspectiveMatrix(HANDLE hOut, mat4*, float, float, float, float);

float* getVals(mat4);

void printMat(HANDLE, mat4);

void printVec3(HANDLE, vec3);
