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

#define ID_HIERARCHY 100
#define ID_VIEWPORT 101

LRESULT MouseProc(int, WPARAM, LPARAM);
LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK hierarchyWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK viewportWindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK enumChildProc(HWND, LPARAM);

void EnableOpenGL(HWND hWnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

const int baseWidth = 1200;
const int baseHeight = 800;

HGLRC hRC = NULL;

RAWINPUTDEVICE Rid[1];
HINSTANCE hInstance;

HWND hHierarchyWnd;
HWND hMainWnd;
HWND hViewportWnd;

int WINAPI WinMain(HINSTANCE hInst,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
    hInstance = hInst;
    WNDCLASSEX wcex;
    WNDCLASSEX hierarchyWndClass;
    WNDCLASSEX viewportWndClass;
    HWND hWnd;
    HDC hdc;
    MSG msg;
    BOOL bQuit = FALSE;

    updatePath();

    char* fileName = calloc(200, sizeof(char));
    sprintf(fileName, "%sicon.ico", installDirectory);
    HICON icon = LoadImage(hInstance, fileName, IMAGE_ICON, 128, 128, LR_LOADFROMFILE);
    free(fileName);

    // setup the main window
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = icon;
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = TEXT("Main Window");
    wcex.hIconSm = icon;

    // setup the hierarchy window
    hierarchyWndClass.cbSize = sizeof(WNDCLASSEX);
    hierarchyWndClass.style = CS_OWNDC;
    hierarchyWndClass.lpfnWndProc = hierarchyWndProc;
    hierarchyWndClass.cbClsExtra = 0;
    hierarchyWndClass.cbWndExtra = 0;
    hierarchyWndClass.hInstance = hInstance;
    hierarchyWndClass.hIcon = icon;
    hierarchyWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    hierarchyWndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    hierarchyWndClass.lpszMenuName = NULL;
    hierarchyWndClass.lpszClassName = TEXT("Hierarchy");
    hierarchyWndClass.hIconSm = NULL;

    // setup the viewport window
    viewportWndClass.cbSize = sizeof(WNDCLASSEX);
    viewportWndClass.style = CS_OWNDC;
    viewportWndClass.lpfnWndProc = viewportWindowProc;
    viewportWndClass.cbClsExtra = 0;
    viewportWndClass.cbWndExtra = 0;
    viewportWndClass.hInstance = hInstance;
    viewportWndClass.hIcon = icon;
    viewportWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    viewportWndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    viewportWndClass.lpszMenuName = NULL;
    viewportWndClass.lpszClassName = TEXT("Viewport");
    viewportWndClass.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
        return 0;

    RegisterClassEx(&hierarchyWndClass);
    RegisterClassEx(&viewportWndClass);



    hMainWnd = CreateWindowEx(0,
                          TEXT("Main Window"),
                          "Peach Tea",
                          WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          baseWidth,
                          baseHeight,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);
    hWnd = hMainWnd;

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    hHierarchyWnd = CreateWindowEx(0,
                           TEXT("Hierarchy"),
                           (LPSTR) NULL,
                           WS_VISIBLE | WS_CHILD,
                           0, 0, 300, 300,
                           hMainWnd,
                           (HMENU)ID_HIERARCHY,
                           hInstance,
                           NULL);

    ShowWindow(hHierarchyWnd, SW_SHOW);
    UpdateWindow(hHierarchyWnd);

    hViewportWnd = CreateWindowEx(0,
                           TEXT("Viewport"),
                           (LPSTR) NULL,
                           WS_VISIBLE | WS_CHILD,
                           0, 0, 300, 300,
                           hMainWnd,
                           (HMENU)ID_VIEWPORT,
                           hInstance,
                           NULL);

    ShowWindow(hViewportWnd, SW_SHOW);
    UpdateWindow(hViewportWnd);

    // hid mouse setup
    Rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
    Rid[0].usUsage = HID_USAGE_GENERIC_MOUSE;
    Rid[0].dwFlags = RIDEV_INPUTSINK;
    Rid[0].hwndTarget = hMainWnd;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    AllocConsole();
    setHandle(GetStdHandle(STD_OUTPUT_HANDLE));

    SetWindowsHookEx(WH_MOUSE, (HOOKPROC)MouseProc, NULL, 0);

    EnableOpenGL(hViewportWnd, &hdc, &hRC);

    GLEInit();
    initMouse(hMainWnd);
    initKeyboard();
    print("initializing renderer\n");
    initRenderer(lpCmdLine);
    print("done initializing renderer\n");
    updateSize(baseWidth - 300, baseHeight);

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
    static RECT rect;
    static int trackMouse = 0;
    switch (uMsg)
    {
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
        case WM_INPUT:
            if (mouseLocked) {
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
        case WM_SIZE:
            PostMessage(hHierarchyWnd, uMsg, wParam, lParam);
            PostMessage(hViewportWnd, uMsg, wParam, lParam);
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

LRESULT CALLBACK hierarchyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static RECT rect;
    ShowWindow(hWnd, SW_SHOW);
    switch(msg) {
        case WM_SIZE:
            GetClientRect(hMainWnd, &rect);
            MoveWindow(hWnd, 0, 0, 300, rect.bottom, TRUE);
            ShowWindow(hWnd, SW_SHOW);
            break;
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;

}

LRESULT CALLBACK viewportWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HDC hdc;
    static PAINTSTRUCT ps;
    static RECT rect;
    switch(msg) {
        case WM_SIZE:
            ;
            GetClientRect(hMainWnd, &rect);
            MoveWindow(hWnd, 300, 0, rect.right - 300, rect.bottom, TRUE);
            ShowWindow(hWnd, SW_SHOW);

            updateSize(rect.right - 300, rect.bottom);
            glViewport (0, 0, rect.right - 300, rect.bottom);
            hdc = BeginPaint(hWnd, &ps);
            if (initialized) {
                display(camera, hdc, hMainWnd);
            }
            EndPaint(hWnd, &ps);
            break;

        case WM_PAINT:
            GetClientRect(hMainWnd, &rect);
            MoveWindow(hWnd, 300, 0, rect.right - 300, rect.bottom, TRUE);
            ShowWindow(hWnd, SW_SHOW);
            return DefWindowProc(hWnd, msg, wParam, lParam);
        default:
            return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}
