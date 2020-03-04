#include "mouse.h"
#include "windows.h"

HANDLE hWnd;
int mouseLocked = 0;
vec2 mouseDelta = {0, 0};

void initMouse(HANDLE h) {
    hWnd = h;
}

void addMouseDelta(float x, float y) {
    mouseDelta.x += x;
    mouseDelta.y += y;
}

void lockMouse() {
    mouseLocked = 1;
    RECT rc;
    GetClientRect(hWnd, &rc);

    // Convert the client area to screen coordinates.
    POINT pt = { rc.left, rc.top };
    POINT pt2 = { rc.right, rc.bottom };
    ClientToScreen(hWnd, &pt);
    ClientToScreen(hWnd, &pt2);
    SetRect(&rc, pt.x, pt.y, pt2.x, pt2.y);

    // Confine the cursor.
    ClipCursor(&rc);
}


void unlockMouse() {
    mouseLocked = 0;
    ClipCursor(NULL);
}


void hideMouse() {
    ShowCursor(FALSE);
}


void showMouse() {
    ShowCursor(TRUE);
}


vec2 getMouseDelta() {
    return mouseDelta;
}


void clearMouseDelta() {
    mouseDelta.x = 0;
    mouseDelta.y = 0;
}

