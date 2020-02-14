#include <windows.h>

typedef struct color {
        float x;
        float y;
        float z;
} color3f;

void frameTick(HWND);
void display(HANDLE, HDC, HWND);
void init();
