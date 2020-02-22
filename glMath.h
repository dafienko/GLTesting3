#include "consoleUtil.h"
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

#define identityVec3 ((vec3){0, 0, 0})

#define identityMatrix ((mat4){1, 0, 0, 0,     0, 1, 0, 0,     0, 0, 1, 0,     0, 0, 0, 1})

typedef struct v4 {
    float x, y, z, w;
} vec4;

float rad(float deg);

vec3 addVec3(vec3, vec3);
vec4 mulMatVec4(mat4, vec4);
mat4 mulMat(mat4, mat4);
vec3 mulVec3(vec3, float);
mat4 fromTranslation(float, float, float);

mat4 rotateXYZ(float, float, float);
mat4 rotateX(float);
mat4 rotateY(float);
mat4 rotateZ(float);

float vec3Magnitude(vec);

mat4 fromPositionAndRotation(vec3, vec3);

void getPerspectiveMatrix(mat4*, float, float, float, float);

float* getVals(mat4);

vec3 normalizeVec3(vec3);

mat4 inverseMat(mat4);
vec3 inverseVec3(vec3);

vec3 getPosition(mat4);

void printMat(mat4);

void printVec3(vec3);
