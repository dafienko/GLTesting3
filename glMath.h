#include "consoleUtil.h"

#define identityMatrix ((mat4){1, 0, 0, 0,     0, 1, 0, 0,     0, 0, 1, 0,     0, 0, 0, 1});
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

vec4 mul(mat4, vec4);
mat4 getTranslateMatrix(int , int , int );

mat4 getProjMatrix(float, float, float, float);

float* getVals(mat4);

void printMat(HANDLE, mat4);
