#include "glMath.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "consoleUtil.h"

/*
    a b c d
    e f g h
    i j k l
    m n o p
*/

float vec3Magnitude(vec3 v) {
    return (sqrt(v.x*v.x + v.y*v.y + v.z*v.z));
}

vec3 getPosition(mat4 m) {
    return (vec3) {m.d, m.h, m.l};
}

vec3 mulVec3(vec3 v, float f) {
    return (vec3){v.x*f, v.y*f, v.z*f};
}

vec3 addVec3(vec3 v1, vec3 v2) {
    return (vec3){v1.x + v2.x, v1.y + v2.y, v1.z + v2.z};
}

vec4 mulMatVec4(mat4 m, vec4 v) {
    return (vec4){  m.a*v.x + m.b*v.y + m.c*v.z + m.d*v.w,
                    m.e*v.x + m.f*v.y + m.g*v.z + m.h*v.w,
                    m.i*v.x + m.j*v.y + m.k*v.z + m.l*v.w,
                    m.m*v.x + m.n*v.y + m.o*v.z + m.p*v.w};
};

vec3 inverseVec3(vec3 v) {
    return (vec3){-v.x, -v.y, -v.z};
}

mat4 inverseMat(mat4 m) {
    mat4 inv = identityMatrix;

    inv.a = m.f  * m.k * m.p -
              m.f  * m.l * m.o -
              m.j  * m.g  * m.p +
              m.j  * m.h  * m.o +
              m.n * m.g  * m.l -
              m.n * m.h  * m.k;

    inv.e = -m.e  * m.k * m.p +
              m.e  * m.l * m.o +
              m.i  * m.g  * m.p -
              m.i  * m.h  * m.o -
              m.m * m.g  * m.l +
              m.m * m.h  * m.k;

    inv.i = m.e  * m.j * m.p -
              m.e  * m.l * m.n -
              m.i  * m.f * m.p +
              m.i  * m.h * m.n +
              m.m * m.f * m.l -
              m.m * m.h * m.j;

    inv.m = -m.e  * m.j * m.o +
                m.e  * m.k * m.n +
                m.i  * m.f * m.o -
                m.i  * m.g * m.n -
                m.m * m.f * m.k +
                m.m * m.g * m.j;

    inv.b = -m.b  * m.k * m.p +
              m.b  * m.l * m.o +
              m.j  * m.c * m.p -
              m.j  * m.d * m.o -
              m.n * m.c * m.l +
              m.n * m.d * m.k;

    inv.f = m.a  * m.k * m.p -
              m.a  * m.l * m.o -
              m.i  * m.c * m.p +
              m.i  * m.d * m.o +
              m.m * m.c * m.l -
              m.m * m.d * m.k;

    inv.j = -m.a  * m.j * m.p +
              m.a  * m.l * m.n +
              m.i  * m.b * m.p -
              m.i  * m.d * m.n -
              m.m * m.b * m.l +
              m.m * m.d * m.j;

    inv.n = m.a  * m.j * m.o -
              m.a  * m.k * m.n -
              m.i  * m.b * m.o +
              m.i  * m.c * m.n +
              m.m * m.b * m.k -
              m.m * m.c * m.j;

    inv.c = m.b  * m.g * m.p -
              m.b  * m.h * m.o -
              m.f  * m.c * m.p +
              m.f  * m.d * m.o +
              m.n * m.c * m.h -
              m.n * m.d * m.g;

    inv.g = -m.a  * m.g * m.p +
              m.a  * m.h * m.o +
              m.e  * m.c * m.p -
              m.e  * m.d * m.o -
              m.m * m.c * m.h +
              m.m * m.d * m.g;

    inv.k = m.a  * m.f * m.p -
              m.a  * m.h * m.n -
              m.e  * m.b * m.p +
              m.e  * m.d * m.n +
              m.m * m.b * m.h -
              m.m * m.d * m.f;

    inv.o = -m.a  * m.f * m.o +
                m.a  * m.g * m.n +
                m.e  * m.b * m.o -
                m.e  * m.c * m.n -
                m.m * m.b * m.g +
                m.m * m.c * m.f;

    inv.d = -m.b * m.g * m.l +
              m.b * m.h * m.k +
              m.f * m.c * m.l -
              m.f * m.d * m.k -
              m.j * m.c * m.h +
              m.j * m.d * m.g;

    inv.h = m.a * m.g * m.l -
              m.a * m.h * m.k -
              m.e * m.c * m.l +
              m.e * m.d * m.k +
              m.i * m.c * m.h -
              m.i * m.d * m.g;

    inv.l = -m.a * m.f * m.l +
                m.a * m.h * m.j +
                m.e * m.b * m.l -
                m.e * m.d * m.j -
                m.i * m.b * m.h +
                m.i * m.d * m.f;

    inv.p = m.a * m.f * m.k -
              m.a * m.g * m.j -
              m.e * m.b * m.k +
              m.e * m.c * m.j +
              m.i * m.b * m.g -
              m.i * m.c * m.f;

    float det = m.a * inv.a + m.b * inv.e + m.c * inv.i + m.d * inv.m;

    if (det == 0)
        return identityMatrix;

    det = 1.0 / det;

    inv.a *= det;
    inv.b *= det;
    inv.c *= det;
    inv.d *= det;

    inv.e *= det;
    inv.f *= det;
    inv.g *= det;
    inv.h *= det;

    inv.i *= det;
    inv.j *= det;
    inv.k *= det;
    inv.l *= det;

    inv.m *= det;
    inv.n *= det;
    inv.o *= det;
    inv.p *= det;
    return inv;
}

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

vec3 normalizeVec3(vec3 v) {
    float d = vec3Magnitude(v);
    return (vec3) {v.x/d, v.y/d, v.z/d};
}

void clearMatrix(mat4* m) {
    m->a = 0; m->b = 0; m->c = 0; m->d = 0;
    m->e = 0; m->f = 0; m->g = 0; m->h = 0;
    m->i = 0; m->j = 0; m->k = 0; m->l = 0;
    m->m = 0; m->n = 0; m->o = 0; m->p = 0;
}

//   http://www.songho.ca/opengl/gl_projectionmatrix.html
void getPerspectiveMatrix(mat4* dest, float fov, float aspect, float nearDist, float farDist)
{
    float t = nearDist * tan((PI/180) * (fov/2));
    float r = t* aspect;

    clearMatrix(dest);

    dest->a = nearDist/r;
    dest->f = nearDist/t;
    dest->k = -(farDist + nearDist)/(farDist-nearDist);
    dest->l = (-2 * farDist * nearDist) / (farDist - nearDist);
    dest->o = -1;
    dest->p = 0;
}

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
    return mulMat(rotateXYZ(rotation.x, rotation.y, rotation.z), fromTranslation(position.x, position.y, position.z));
    //return mulMat(fromTranslation(position.x, position.y, position.z), rotateXYZ(rotation.x, rotation.y, rotation.z));
}

void printVec3(vec3 v) {
    print("(%f, %f, %f)\n", v.x, v.y, v.z);
}

void printMat(mat4 mat) {
    char* buffer = calloc(300, sizeof(char));

    sprintf(buffer, "| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n",
            mat.a, mat.b, mat.c, mat.d, mat.e, mat.f, mat.g, mat.h, mat.i, mat.j, mat.k, mat.l, mat.m, mat.n, mat.o, mat.p);

    print(buffer);

    free(buffer);
}

