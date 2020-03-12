#include <stdio.h>
#include "ghettoWin.h"
#include <winuser.h>
#include <gl/gl.h>
#include <stdlib.h>

#include "shaderUtil.h"
#include "fileUtil.h"
#include "renderer.h"
#include "consoleUtil.h"
#include "glExtensions.h"
#include "keyboard.h"
#include "game.h"
#include "mouse.h"


// mouse input bullcrap
#ifndef HID_USAGE_PAGE_GENERIC
#define HID_USAGE_PAGE_GENERIC         ((USHORT) 0x01)
#endif
#ifndef HID_USAGE_GENERIC_MOUSE
#define HID_USAGE_GENERIC_MOUSE        ((USHORT) 0x02)
#endif



LRESULT MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hWnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

const int baseWidth = 1200;
const int baseHeight = 800;

HGLRC hRC = NULL;

RAWINPUTDEVICE Rid[1];

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hWnd;
    HDC hdc;
    MSG msg;
    BOOL bQuit = FALSE;

    updatePath();

    char* fileName = calloc(200, sizeof(char));
    sprintf(fileName, "%sicon.ico", installDirectory);
    HICON icon = LoadImage(hInstance, fileName, IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
    free(fileName);

    /* register window class */
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = icon;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = icon;

    if (!RegisterClassEx(&wcex))
        return 0;

    hWnd = CreateWindowEx(0,
                          "GLSample",
                          "OpenGL Sample",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          baseWidth,
                          baseHeight,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);


    // hid mouse setup
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = hWnd;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    //uncomment the next 2 lines to show console
    //AllocConsole();
    //setHandle(GetStdHandle(STD_OUTPUT_HANDLE));

    SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, NULL, 0);

    ShowWindow(hWnd, TRUE);

    EnableOpenGL(hWnd, &hdc, &hRC);

    GLEInit();
    initMouse(hWnd);
    initKeyboard();
    initRenderer(lpCmdLine);
    updateSize(baseWidth, baseHeight);
    initGame();

    while (!bQuit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        } else {
            swapKbdBuffer();
            display(camera, hdc, hWnd);
        }
    }

    DisableOpenGL(hWnd, hdc, hRC);

    DestroyWindow(hWnd);
    FreeConsole();

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc;
    static PAINTSTRUCT ps;
    static RECT rect;
    static int trackMouse = 0;

    switch (uMsg)
    {
        case WM_MOUSEWHEEL:
            ;
            int zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
            zoom += zDelta * zoom/2000.0;
            break;
        case WM_LBUTTONDOWN:
            leftMouseDown = 1;
            break;
        case WM_LBUTTONUP:
            leftMouseDown = 0;
            break;
        case WM_INPUT:
            if (mouseLocked || 1) {
                UINT dwSize = 40;
                static BYTE lpb[40];

                GetRawInputData((HRAWINPUT)lParam, RID_INPUT,
                                lpb, &dwSize, sizeof(RAWINPUTHEADER));

                RAWINPUT* raw = (RAWINPUT*)lpb;

                if (raw->header.dwType == RIM_TYPEMOUSE)
                {
                    int xPosRelative = raw->data.mouse.lLastX;
                    int yPosRelative = raw->data.mouse.lLastY;
                    addMouseDelta(xPosRelative, yPosRelative);

                }
            }
            break;

        case WM_CLOSE:
            PostQuitMessage(0);
            break;

        case WM_DESTROY:
            return 0;

        case WM_MOUSEMOVE:
            if (mouseLocked) {
                GetWindowRect(hWnd, &rect);
                if (trackMouse) {
                    SetCursorPos((rect.left + rect.right)/2, (rect.top + rect.bottom)/2);
                } else {
                    trackMouse = 1;
                }
            }

            break;
        case WM_KEYDOWN:
            keyDown(wParam);

            switch (wParam)
            {
                case VK_ESCAPE:
                    if (mouseLocked) {
                        showMouse();
                        unlockMouse();
                        trackMouse = 0;
                    } else {
                        hideMouse();
                        lockMouse();
                        trackMouse = 0;
                    }
                    break;

            }

            break;
        case WM_KEYUP:
            keyUp(wParam);

            break;
        case WM_SIZE:
            ;
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);
            updateSize(width, height);
            glViewport (0, 0, width, height);

            GetWindowRect(hWnd, &rect);

            hdc = BeginPaint(hWnd, &ps);

            if (initialized) {
                display(camera, hdc, hWnd);
            }

            EndPaint(hWnd, &ps);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hWnd, HDC* hdc, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    *hdc = GetDC(hWnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hdc, &pfd);

    SetPixelFormat(*hdc, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hdc);

    wglMakeCurrent(*hdc, *hRC);
}

void DisableOpenGL (HWND hWnd, HDC hdc, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hdc);
}

LRESULT MouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (wParam == WM_MOUSEMOVE) {

    }

    return CallNextHookEx(NULL, nCode, wParam, lParam);
}

