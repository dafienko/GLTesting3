#include <stdio.h>
#include <windows.h>
#include <sys/timeb.h>
#include <time.h>
#include <gl/gl.h>

#include "renderer.h"
#include "glExtensions.h"
#include "shaderUtil.h"
#include "fileUtil.h"
#include "consoleUtil.h"

GLuint bp = 0;
GLuint vboId = 0;
GLuint vaoId = 0;
int framesSinceLastUpdate = 0;
int lastMs = 0;
int lastFrameTime = 0;
int fps = 0;

#define z 0
#define t .8

///*
 GLfloat verts[] = {t, t, z,
            t, -t, z,
            -t, -t, z,
            t, t, z,
            -t, t, z,
            -t, -t, z};

void display(HANDLE hOut, HDC hdc, HWND hWnd) {  //display function
    frameTick(hOut, hWnd);

    glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glClear(GL_DEPTH_BUFFER_BIT);

    glUseProgram(bp);

    glBindVertexArray(vaoId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glPointSize(30);
    glDrawArrays(GL_TRIANGLES, 0, (sizeof(verts) / sizeof(verts[0])) / 3);


    SwapBuffers(hdc);
}

/**
@param hOut
handle to output window
**/
void init(HANDLE hOut) {
    wglSwapIntervalEXT(0);

    bp = createBasicProgram(hOut);

    glGenVertexArrays(1, &vaoId);
    checkGLError(hOut, "glGenVertexArrays");

    glBindVertexArray(vaoId);
    checkGLError(hOut, "glBindVertexArray");

    glGenBuffers(1, &vboId);
    checkGLError(hOut, "glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    checkGLError(hOut, "glBindBuffer");

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    checkGLError(hOut, "glBufferData");

    char* v = glGetString(GL_VERSION);
    print(hOut, "version: %s\n", v);
    //*/
}

void frameTick(HANDLE hOut, HWND hWnd) {
    int ms = GetTickCount();
    int dt = ms - lastFrameTime;
    if (dt < 0) {
        lastFrameTime = GetTickCount();
        return;
    }
    framesSinceLastUpdate++;

    if (dt > 1000) {
        lastFrameTime = ms;
        fps = (1000 * framesSinceLastUpdate)/(dt);

        char windowText[10];
        sprintf(windowText, "%i FPS", fps);
        SetWindowText(hWnd, TEXT(windowText));

        framesSinceLastUpdate = 0;

    }
}
