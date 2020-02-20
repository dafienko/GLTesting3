#include "glMath.h"
#include <math.h>
#include <stdio.h>

void printMat(HANDLE hOut, mat4 mat) {
    char* buffer = calloc(300, sizeof(char));
    sprintf(buffer, "| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n| \t %f \t %f \t %f \t %f |\n", mat.a, mat.b, mat.c, mat.d, mat.e, mat.f, mat.g, mat.h, mat.i, mat.j, mat.k, mat.l, mat.m, mat.n, mat.o, mat.p);
    //sprintf(buffer, "%f", mat.a);
    print(hOut, buffer);
    free(buffer);
}

vec4 mul(mat4 m, vec4 v) {
    return (vec4){m.a*v.x + m.b*v.y + m.c*v.z + m.d*v.w,
        m.e*v.x + m.f*v.y + m.g*v.z + m.h*v.w,
        m.i*v.x + m.j*v.y + m.k*v.z + m.l*v.w,
        m.m*v.x + m.n*v.y + m.o*v.z + m.p*v.w};
};



mat4 getTranslateMatrix(int x, int y, int z) {
    return (mat4) {1, 0, 0, x,
                    0, 1, 0, y,
                    0, 0, 1, z,
                    0, 0, 0, 1};
}

/*
a b c d
e f g h
i j k l
m n o p
*/
float* getVals(mat4 mat) { // values is a 1-D array of mat in column-major order
    float vals[16];
    vals[0] = mat.a;    vals[1] = mat.e;    vals[2] = mat.i;    vals[3] = mat.m; // 1st column
    vals[4] = mat.b;    vals[5] = mat.f;    vals[6] = mat.j;    vals[7] = mat.n; // 2nd column
    vals[8] = mat.c;    vals[9] = mat.g;    vals[10] = mat.k;   vals[11] = mat.o; // 3rd column
    vals[12] = mat.d;   vals[13] = mat.h;   vals[14] = mat.l;   vals[15] = mat.p; // 4th column
    return &vals;
}

mat4 getProjMatrix(float fov, float aspect, float nearDist, float farDist)
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
    float uh = ((double)1)/(tan(fov/2));
    float uw = ((double)1/aspect) * uh;
    float depth = farDist - nearDist;
    float reciprocalDepth = (double)1/(double)depth;

    mat4 result = identityMatrix;
    result.a = uw;
    result.f = uh;
    result.k = farDist * reciprocalDepth;
    result.o = -farDist * nearDist * reciprocalDepth;
    result.p = 0;
    return result;
}
