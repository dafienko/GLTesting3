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
#define MAX_BUTTONS 100

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK hierarchyWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK viewportWindowProc(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK enumChildProc(HWND, LPARAM);

void EnableOpenGL(HWND hWnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

const int baseWidth = 1200;
const int baseHeight = 800;
int hierarchyCursor;

HGLRC hRC = NULL;

RAWINPUTDEVICE Rid[1];
HINSTANCE hInstance;

HWND hHierarchyWnd;
HWND hMainWnd;
HWND hViewportWnd;

HBRUSH selectedBrush;
HBRUSH selectedHighlightBrush;
HBRUSH highlightBrush;

HCURSOR hCursorArrow;
HCURSOR insertCursor;


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
    HICON icon = LoadImage(hInstance, fileName, IMAGE_ICON, 128, 128, LR_SHARED | LR_LOADFROMFILE);

    sprintf(fileName, "%sassets\\cursors\\insertCursor.cur", installDirectory);
    insertCursor = LoadCursorFromFile(fileName);
    free(fileName);

    hCursorArrow = LoadCursor(NULL, IDC_ARROW);

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
    hierarchyWndClass.hIcon = NULL;
    hierarchyWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    hierarchyWndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
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
    viewportWndClass.hIcon = NULL;
    viewportWndClass.hCursor = LoadCursor(NULL, IDC_ARROW);
    viewportWndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);
    viewportWndClass.lpszMenuName = NULL;
    viewportWndClass.lpszClassName = TEXT("Viewport");
    viewportWndClass.hIconSm = NULL;

    if (!RegisterClassEx(&wcex))
        return 0;

    RegisterClassEx(&hierarchyWndClass);
    RegisterClassEx(&viewportWndClass);

    COLORREF windowColor = GetSysColor(COLOR_HIGHLIGHT);
    selectedBrush = CreateSolidBrush(windowColor);
    selectedHighlightBrush = CreateSolidBrush(RGB(min(GetRValue(windowColor) * 1.5, 255), min(GetGValue(windowColor) * 1.5, 255), min(GetBValue(windowColor) * 1.5, 255)));
    highlightBrush = CreateSolidBrush(RGB(180, 180, 180));

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

    AllocConsole();
    setHandle(GetStdHandle(STD_OUTPUT_HANDLE));

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
    Rid[0].hwndTarget = hViewportWnd;
    RegisterRawInputDevices(Rid, 1, sizeof(Rid[0]));

    EnableOpenGL(hViewportWnd, &hdc, &hRC);

    GLEInit();
    initMouse(hViewportWnd);
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
    switch (uMsg)
    {
        case WM_KEYDOWN:
            keyDown(wParam);

            break;
        case WM_KEYUP:
            keyUp(wParam);

            break;
        case WM_SIZE:
            PostMessage(hHierarchyWnd, uMsg, wParam, lParam);
            PostMessage(hViewportWnd, uMsg, wParam, lParam);
            break;
        default:
            return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

int tHeight = 24;
int padding = 5;

HFONT hFont;
HDC hdcExpandedMask, hdcExpandedSource, hdcCollapsedMask, hdcCollapsedSource;
HBITMAP hExpandedImage, hExpandedMask, hCollapsedImage, hCollapsedMask;

int numHierarchyButtons = 0;
int* hierarchyInstanceOrder;

typedef struct button {
    RECT rect;
    int id;
    void (*pressedCallback)(int);
    void (*releasedCallback)(int);
    int (*mouseEnterCallback)(int);
    int (*mouseExitCallback)(int);
    int mouseInFrame;
} BUTTON;

void addButton(BUTTON** buttons, BUTTON* button) {
    for (int i = 0; i < MAX_BUTTONS; i++) {
        BUTTON* b = *(buttons + i);
        if (b == NULL) {
            *(buttons + i) = button;
            return;
        }
    }
}

void clearButtons(BUTTON** buttons) {
    for (int i = 0; i < MAX_INSTANCES; i++) {
        BUTTON* b = *(buttons + i);
        if (b != NULL) {
            free(b);
        }
        *(buttons + i) = NULL;
    }
}

int getNumChildren(int id) {
    int numChildren = 0;
    INSTANCE* parent = *(instances + id);

    for (int i = 0; i < MAX_INSTANCES; i++) {
        INSTANCE* inst = *(instances + i);
        if (inst != NULL && inst->parent == parent) {
            numChildren++;
        }
    }

    return numChildren;
}

void deselectChildren(INSTANCE* parent) {
    for (int i = 0; i < MAX_BUTTONS; i++) {
        INSTANCE* inst = *(instances + i);
        if (inst != NULL) {
            if (inst->parent == parent || parent == NULL) {
                inst->selected = FALSE;
                deselectChildren(inst);
            }
        }
    }
}

void hButtonReleasedCallback(int id) {
    INSTANCE* inst = *(instances + id);
    inst->expanded = !(inst->expanded);

    if (!inst->expanded) { // if the instance has been collapsed, deselect all its children
        deselectChildren(inst);
    }

    InvalidateRect(hHierarchyWnd, NULL, TRUE);
}

int sign(int x) {
    if (x == 0) {
        return 0;
    } else {
        if (x < 0) {
            return -1;
        } else {
            return 1;
        }
    }
}

int getHierarchyOrder(int id) {
    for (int i = 0; i < MAX_INSTANCES; i++) {
        if (hierarchyInstanceOrder[i] == id) {
            return i;
        }
    }
    return -1;
}

void hMainButtonPressCallback(int id) {
    static int lastSelected = -1; // sentinel value of -1

    INSTANCE* inst = *(instances + id);


    if (!isKeyDown(VK_LSHIFT) && !isKeyDown(VK_LCONTROL)) {
        if (!inst->selected) {
            deselectChildren(NULL);
        }
    } else {
        if (isKeyDown(VK_LSHIFT)) {
            if (lastSelected >= 0) { // lastSelected has actually been initialized
                int diff = hierarchyInstanceOrder[id] - lastSelected;
                for (int i = 1; i <= abs(diff); i++) { // select everything in between this button and
                    int offset = hierarchyInstanceOrder[lastSelected + i * sign(diff)];
                    INSTANCE* thisInst = *(instances + offset);
                    thisInst->selected = TRUE;
                }
            }
        }
    }

    inst->selected = TRUE;

    InvalidateRect(hHierarchyWnd, NULL, TRUE);

    lastSelected = getHierarchyOrder(id);
}

int hMainButtonMouseEnterCallback(int id) {
    INSTANCE* inst = *(instances + id);
    if (!inst->mouseOverHierarchy) {
        inst->mouseOverHierarchy = TRUE;
        return TRUE;
    }
    return FALSE;
}


int hMainButtonMouseExitCallback(int id) {
    INSTANCE* inst = *(instances + id);
    if (inst->mouseOverHierarchy) {
        inst->mouseOverHierarchy = FALSE;
        return TRUE;
    }
    return FALSE;
}

int isInstanceSelected(int id) {
    INSTANCE* inst = *(instances + id);
    return inst != NULL && inst->selected;
}

int getHighlightedInstance() {
    for (int i = 0; i < MAX_INSTANCES; i++) {
            INSTANCE* inst = *(instances + i);
            if (inst != NULL && inst->mouseOverHierarchy) {
                return i;
            }
    }

    return -1;
}

void drawInstanceInHierarchy(HDC hdc, INSTANCE* inst, int x, int y, int instId, BUTTON** buttons) {
    hierarchyInstanceOrder[numHierarchyButtons] = instId;
    numHierarchyButtons++;

    int imgSize = (int)(tHeight * .8);
    int imgDiff = tHeight - imgSize;

    //add the buttons to the window listener
    BUTTON* b = calloc(sizeof(BUTTON), 1);
    *b = (BUTTON) {(RECT){x, y+imgDiff/2, x+imgSize, y+imgDiff/2+imgSize}, instId, NULL, &hButtonReleasedCallback, NULL, NULL, -1};
    addButton(buttons, b);

    BUTTON* mainButton = calloc(sizeof(BUTTON), 1);
    RECT mainButtonRect = (RECT){x + tHeight, y, x + 200, y+tHeight};
    *mainButton = (BUTTON){mainButtonRect, instId, &hMainButtonPressCallback, NULL, hMainButtonMouseEnterCallback, hMainButtonMouseExitCallback, -1};
    addButton(buttons, mainButton);

    // set main button background color
    RECT mainButtonFrameRect = (RECT){x, y - padding/2, x + 250, y + tHeight + padding/2};
    HBRUSH mainButtonBrush = selectedBrush;
    if (inst->selected) {
        if (inst->mouseOverHierarchy) {
            mainButtonBrush = selectedHighlightBrush;
        } else {
            mainButtonBrush = selectedBrush;
        }
    } else {
        if (inst->mouseOverHierarchy) {
            mainButtonBrush = highlightBrush;
        } else {
            mainButtonBrush = (HBRUSH)GetStockObject(GRAY_BRUSH);
        }
    }
    FillRect(hdc, &mainButtonFrameRect, mainButtonBrush);

    //draw the expanded/collapsed icon button
    if (getNumChildren(instId) > 0) {
        if (!inst->expanded) {
            StretchBlt(hdc, x, y + imgDiff/2, imgSize, imgSize, hdcCollapsedMask, 0, 0, 100, 100, SRCAND);
            StretchBlt(hdc, x, y + imgDiff/2, imgSize, imgSize, hdcCollapsedSource, 0, 0, 100, 100, SRCPAINT);
        } else {
            StretchBlt(hdc, x, y + imgDiff/2, imgSize, imgSize, hdcExpandedMask, 0, 0, 100, 100, SRCAND);
            StretchBlt(hdc, x, y + imgDiff/2, imgSize, imgSize, hdcExpandedSource, 0, 0, 100, 100, SRCPAINT);
        }
    }

    //write the name of the instance in the main button
    SetTextColor(hdc, RGB(255, 255, 255));
    TextOut(hdc, x + tHeight, y, inst->name, strlen(inst->name));
}


int drawInstanceChildren(INSTANCE* parent, HDC hdc, BUTTON** buttons, int x, int y) {
    int height = y;
    int addedHeight = 0;
    for (int i = 0; i < MAX_INSTANCES; i++) {
        INSTANCE* inst = *(instances + i);
        if (inst != NULL && inst->parent == parent) {
            drawInstanceInHierarchy(hdc, inst, x, height, i, buttons);
            height += tHeight + padding;
            addedHeight += tHeight + padding;
            if (inst->expanded) {
                height += drawInstanceChildren(inst, hdc, buttons, x + 20, height);
            }
        }
    }
    return addedHeight;
}

int getNumSelectedInstances() {
    int numSelectedInstances = 0;
    for (int i = 0; i < MAX_INSTANCES; i++) {
        INSTANCE* inst = *(instances + i);
        if (inst != NULL && inst->selected) {
            numSelectedInstances++;
        }
    }
    return numSelectedInstances;
}

int isDescendant(int id, int parentId) {
    INSTANCE* child = *(instances + id);
    INSTANCE* parent = *(instances + parentId);
    while (parent != NULL && parent != child) {
        parent = parent->parent;
    }

    return parent == child;
}

LRESULT CALLBACK hierarchyWndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static RECT rect;
    static PAINTSTRUCT ps;
    static HDC hdc;
    static BUTTON** buttons;
    static int LMouseDown = 0;
    static int draggedFrom = 0;

    switch(msg) {
        case WM_SETCURSOR:
            break;
        case WM_MOUSEMOVE:
            {
                SetCursor(hierarchyCursor);

                int x = LOWORD(lParam);
                int y = HIWORD(lParam);

                int repaintRequired = 0;

                for (int i = 0; i < MAX_INSTANCES; i++) {
                    BUTTON* b = *(buttons + i);
                    if (b != NULL) {
                        if (x >= b->rect.left && x <= b->rect.right && y >= b->rect.top && y <= b->rect.bottom) { // the mouse is in the button
                            if (b->mouseInFrame == 0) { // the mouse wasn't in the button before
                                if (b->mouseEnterCallback != NULL) {  // not all buttons may have this callback defined
                                    repaintRequired = repaintRequired || (b->mouseEnterCallback)(b->id);
                                }
                            }

                            b->mouseInFrame = 1;
                        } else { // the mouse isn't in the button
                            if (b->mouseInFrame == 1) { // the mouse was in the button before
                                if (b->mouseExitCallback != NULL) {  // not all buttons may have this callback defined
                                    repaintRequired = repaintRequired || (b->mouseExitCallback)(b->id);
                                }
                            }

                            b->mouseInFrame = 0;
                        }
                    }
                }

                if (repaintRequired) {
                    InvalidateRect(hWnd, NULL, FALSE);
                }

                if (LMouseDown) {
                    int highlightedInstance = getHighlightedInstance();
                    if (highlightedInstance == -1 || !isInstanceSelected(highlightedInstance)) { // the user is highlighting nothing or the highlighted instance isn't selected
                        int numSelectedInstances = getNumSelectedInstances();
                        if (numSelectedInstances > 0) { // there's at least one instance selected
                            int instancesArentDescendants = 1;
                            if (highlightedInstance >= 0) {
                                for (int i = 0; i < MAX_INSTANCES; i++) {
                                    INSTANCE* inst = *(instances + i);
                                    if (inst != NULL && inst->selected) {
                                        instancesArentDescendants = instancesArentDescendants && !isDescendant(i, highlightedInstance);
                                    }
                                }
                            }

                            if (instancesArentDescendants) {
                                hierarchyCursor = insertCursor;
                                SetCursor(insertCursor);
                                break;
                            }
                        }
                    }
                }
                hierarchyCursor = hCursorArrow;
                SetCursor(hCursorArrow);
            }
            break;
        case WM_CREATE:
            ;
            hierarchyInstanceOrder = calloc(sizeof(int), MAX_INSTANCES);
            hierarchyCursor = hCursorArrow;
            hCollapsedImage = getBmpHandle("collapsed3.bmp");
            hCollapsedMask = getBmpMask(hCollapsedImage);
            hExpandedImage = getBmpHandle("expanded3.bmp");
            hExpandedMask = getBmpMask(hExpandedImage);

            hdcCollapsedSource = CreateCompatibleDC(GetDC(hWnd));
            SelectObject(hdcCollapsedSource, hCollapsedImage);

            hdcCollapsedMask = CreateCompatibleDC(GetDC(hWnd));
            SelectObject(hdcCollapsedMask, hCollapsedMask);

            hdcExpandedSource = CreateCompatibleDC(GetDC(hWnd));
            SelectObject(hdcExpandedSource, hExpandedImage);

            hdcExpandedMask = CreateCompatibleDC(GetDC(hWnd));
            SelectObject(hdcExpandedMask, hExpandedMask);

            buttons = (BUTTON**)calloc(sizeof(BUTTON*), MAX_BUTTONS);

            hFont = CreateFont(tHeight, //                cHeight
                                         (int)(tHeight/2.5), //              cWidth
                                         0, //                      cEscapement
                                         0, //                      cOrientation
                                         100, //                    cWeight
                                         FALSE, //                  bItalic
                                         FALSE, //                  bUnderline
                                         FALSE, //                  bStrikeout
                                         DEFAULT_CHARSET,//         iCharSet
                                         OUT_OUTLINE_PRECIS,//      iOutPrecision
                                         CLIP_DEFAULT_PRECIS,//     iClipPrecision
                                         CLEARTYPE_QUALITY, //      iQuality
                                         FF_DONTCARE,//               iPitchAndFamily
                                         TEXT("Dubai"));            //pszFaceName

            break;

        case WM_SIZE:
            GetClientRect(hMainWnd, &rect);
            MoveWindow(hWnd, 0, 0, 300, rect.bottom, TRUE);
            ShowWindow(hWnd, SW_SHOW);
            break;
        case WM_PAINT:
            if (initialized) {
                clearButtons(buttons);
                hdc = BeginPaint(hWnd, &ps);
                SetBkMode(hdc, TRANSPARENT);

                SelectObject(hdc,hFont);

                int height = 10;
                int x = 10;

                numHierarchyButtons = 0;
                drawInstanceChildren(NULL, hdc, buttons, x, height);


                EndPaint(hWnd, &ps);
            }
            break;
        case WM_LBUTTONUP:
            ;
            {

                LMouseDown = FALSE;

                int x = LOWORD(lParam);
                int y = HIWORD(lParam);

                for (int i = 0; i < MAX_BUTTONS; i++) {
                    BUTTON* b = *(buttons + i);
                    if (b != NULL) {
                        if (x >= b->rect.left && x <= b->rect.right && y >= b->rect.top && y <= b->rect.bottom) {
                            if (b->releasedCallback != NULL) {  // not all buttons may have this callback defined
                                (b->releasedCallback)(b->id);
                                break; // only fire one button per click
                            }
                        }
                    }
                }


                int repaintRequired = 0;
                if (draggedFrom) {
                    int highlightedInstance = getHighlightedInstance();
                    if (highlightedInstance == -1 || !isInstanceSelected(highlightedInstance)) { // the user is highlighting nothing or the highlighted instance isn't selected
                        int numSelectedInstances = getNumSelectedInstances();
                        if (numSelectedInstances > 0) { // there's at least one instance selected
                            for (int i = 0; i < MAX_INSTANCES; i++) {
                                INSTANCE* inst = *(instances + i);
                                if (inst != NULL && inst->selected) {
                                    repaintRequired = 1;
                                    if (highlightedInstance == -1) {
                                        inst->parent = NULL;
                                    } else if (!isDescendant(i, highlightedInstance)) {
                                        (*(instances + highlightedInstance))->expanded = TRUE;
                                        inst->parent = *(instances + highlightedInstance);
                                    }
                                }
                            }
                        }
                    }
                }

                if (repaintRequired) {
                    InvalidateRect(hWnd, NULL, TRUE);
                } else {
                    if (getHighlightedInstance() == -1) {
                        deselectChildren(NULL);
                        InvalidateRect(hWnd, NULL, TRUE);
                    }
                }
            }
            break;
        case WM_LBUTTONDOWN:
            ;
            {

                LMouseDown = TRUE;

                if (getHighlightedInstance()  != -1) {
                    draggedFrom = 1;
                } else {
                    draggedFrom = 0;
                }

                int x = LOWORD(lParam);
                int y = HIWORD(lParam);

                for (int i = 0; i < MAX_INSTANCES; i++) {
                    BUTTON* b = *(buttons + i);
                    if (b != NULL) {
                        if (x >= b->rect.left && x <= b->rect.right && y >= b->rect.top && y <= b->rect.bottom) {
                            if (b->pressedCallback != NULL) { // not all buttons may have this callback defined
                                (b->pressedCallback)(b->id);
                                break; // only fire one button per click
                            }
                        }
                    }
                }
            }
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
    static int trackMouse = 0;

    switch(msg) {
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
        case WM_MOUSEMOVE:
            if (mouseLocked) {
                GetWindowRect(hViewportWnd, &rect);
                if (trackMouse) {
                    SetCursorPos((rect.left + rect.right)/2, (rect.top + rect.bottom)/2);
                } else {
                    trackMouse = 1;
                }
            }

            break;

        case WM_RBUTTONUP:
            showMouse();
            unlockMouse();
            trackMouse = 0;
            break;

        case WM_RBUTTONDOWN:
            hideMouse();
            lockMouse();
            trackMouse = 0;
            break;

        case WM_MOUSEACTIVATE:
            if ((int)wParam == (int)hViewportWnd) {
                SetCapture(hWnd);
                return MA_ACTIVATE;
            }
            break;
        case WM_ACTIVATE:
            if (wParam == WA_INACTIVE) {
                ReleaseCapture();
                return 0;
            }
            break;
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
