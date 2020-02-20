#include <windows.h>
#include "glMath.h"

mat4 perspectiveMat;

typedef struct color {
        float x;
        float y;
        float z;
} color3f;


void updateSize(int width, int height);
void frameTick(HANDLE, HWND);
void display(HANDLE, HDC, HWND);
void init();
