#include "glMath.h"
#include <math.h>
#include <stdio.h>
#include "consoleUtil.h"

void printMat(HANDLE hOut, mat4 mat) {
    char* buffer = calloc(300, sizeof(char));
    sprintf(buffer, "| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n", mat.a, mat.b, mat.c, mat.d, mat.e, mat.f, mat.g, mat.h, mat.i, mat.j, mat.k, mat.l, mat.m, mat.n, mat.o, mat.p);
    //sprintf(buffer, "%f", mat.a);
    print(hOut, buffer);
    free(buffer);
}

vec4 mulVec(mat4 m, vec4 v) {
    return (vec4){m.a*v.x + m.b*v.y + m.c*v.z + m.d*v.w,
        m.e*v.x + m.f*v.y + m.g*v.z + m.h*v.w,
        m.i*v.x + m.j*v.y + m.k*v.z + m.l*v.w,
        m.m*v.x + m.n*v.y + m.o*v.z + m.p*v.w};
};

/*
a b c d
e f g h
i j k l
m n o p
*/
mat4 mulMat(mat4 m1, mat4 m2) {
    return (mat4){
        m1.a*m2.a + m1.b*m2.e + m1.c*m2.i + m1.d*m2.m,
        m1.a*m2.b + m1.b*m2.f + m1.c*m2.j + m1.d*m2.n,
        m1.a*m2.c + m1.b*m2.g + m1.c*m2.k + m1.d*m2.o,
        m1.a*m2.d + m1.b*m2.h + m1.c*m2.l + m1.d*m2.p,

        m1.e*m2.a + m1.f*m2.e + m1.g*m2.i + m1.h*m2.m,
        m1.e*m2.b + m1.f*m2.f + m1.g*m2.j + m1.h*m2.n,
        m1.e*m2.c + m1.f*m2.g + m1.g*m2.k + m1.h*m2.o,
        m1.e*m2.d + m1.f*m2.h + m1.g*m2.l + m1.h*m2.p,

        m1.i*m2.a + m1.j*m2.e + m1.k*m2.i + m1.l*m2.m,
        m1.i*m2.b + m1.j*m2.f + m1.k*m2.j + m1.l*m2.n,
        m1.i*m2.c + m1.j*m2.g + m1.k*m2.k + m1.l*m2.o,
        m1.i*m2.d + m1.j*m2.h + m1.k*m2.l + m1.l*m2.p,

        m1.m*m2.a + m1.n*m2.e + m1.o*m2.i + m1.p*m2.m,
        m1.m*m2.b + m1.n*m2.f + m1.o*m2.j + m1.p*m2.n,
        m1.m*m2.c + m1.n*m2.g + m1.o*m2.k + m1.p*m2.o,
        m1.m*m2.d + m1.n*m2.h + m1.o*m2.l + m1.p*m2.p
    };
}

mat4 rotateX(float a) {
    return (mat4) {
        1, 0, 0, 0,
        0, cos(a), -sin(a), 0,
        0, sin(a), cos(a), 0,
        0, 0, 0, 1
    };
}

mat4 rotateY(float a) {
    return (mat4) {
        cos(a), 0, sin(a), 0,
        0, 1, 0, 0,
        -sin(a), 0, cos(a), 0,
        0, 0, 0, 1
    };
}

mat4 rotateZ(float a) {
    return (mat4) {
        cos(a), -sin(a), 0, 0,
        sin(a), cos(a), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

mat4 fromTranslation(float x, float y, float z) {
    return (mat4) {1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1};
}

void clearMatrix(mat4* m) {
    m->a = 0; m->b = 0; m->c = 0; m->d = 0;
    m->e = 0; m->f = 0; m->g = 0; m->h = 0;
    m->i = 0; m->j = 0; m->k = 0; m->l = 0;
    m->m = 0; m->n = 0; m->o = 0; m->p = 0;
}

void getPerspectiveMatrix(HANDLE hOut, mat4* dest, float fov, float aspect, float nearDist, float farDist)
{
    // General form of the Projection Matrix
    //
    // uh = Cot( fov/2 ) == 1/Tan(fov/2)
    // uw / uh = 1/aspect
    //
    //   uw         0       0       0
    //    0        uh       0       0
    //    0         0      f/(f-n)  1
    //    0         0    -fn/(f-n)  0
    //

    float top = nearDist * tan((PI/180) * (fov/2));
    float bottom = -top;
    float r = top * aspect;
    float l = -r;

    print(hOut, "top: %f\n", top);

    clearMatrix(dest);

    /*
    a   c
      f g
        k l
        o p
    */

    /*
    dest->a = 1/(aspect * tan(fov/2));
    dest->f = 1/(tan(fov/2));
    //dest->f = (2 * nearDist) / (top - bottom);
    //dest->g = (top + bottom) / (top - bottom);
    dest->k = (-farDist + nearDist) / (farDist - nearDist);
    dest->l = (2 * farDist * nearDist) / (farDist - nearDist);
    dest->o = -1;
    dest->p = 0;
    */
    ///*
    float n = nearDist;
    float f = farDist;

    dest->a = n/r;
    dest->f = n/l;
    dest->k = -(f + n)/(f-n);
    dest->l = (-2 * f * n) / (f - n);
    dest->o = -1;
    dest->p = 0;
    //*/
}

/*
a b c d
e f g h
i j k l
m n o p
*/
float* getVals(mat4 mat) { // values is a 1-D array of mat in column-major order
    float* vals = calloc(16, sizeof(float));
    *(vals + 0) = mat.a;    *(vals + 1) = mat.e;    *(vals + 2) = mat.i;    *(vals + 3) = mat.m; // 1st column
    *(vals + 4) = mat.b;    *(vals + 5) = mat.f;    *(vals + 6) = mat.j;    *(vals + 7) = mat.n; // 2nd column
    *(vals + 8) = mat.c;    *(vals + 9) = mat.g;    *(vals + 10) = mat.k;   *(vals + 11) = mat.o; // 3rd column
    *(vals + 12) = mat.d;   *(vals + 13) = mat.h;   *(vals + 14) = mat.l;   *(vals + 15) = mat.p; // 4th column
    return vals;
}

float rad(float a) {
    return a * (PI/180);
}

mat4 rotateXYZ(float x, float y, float z) {
    return mulMat(rotateX(x), mulMat(rotateY(y), rotateZ(z)));
}

mat4 fromPositionAndRotation(vec3 position, vec3 rotation) {
    //return mulMat(rotateXYZ(rotation.x, rotation.y, rotation.z), fromTranslation(position.x, position.y, position.z));
    return mulMat(fromTranslation(position.x, position.y, position.z), rotateXYZ(rotation.x, rotation.y, rotation.z));
}

void printVec3(HANDLE hOut, vec3 v) {
    print(hOut, "(%f, %f, %f)\n", v.x, v.y, v.z);
}


