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
#define t .5

 GLfloat verts[] = {t, t, z,
            t, -t, z,
            -t, -t, z,
            t, t, z,
            -t, t, z,
            -t, -t, z};

model* m;

GLint projLoc, mvLoc;


int displayEnabled = 1; // for use with debugging shaders
void display(HDC hdc, HWND hWnd) {  //display function
    if (displayEnabled) {
        frameTick(hWnd);

        glClearColor(0.0f, 1.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        projLoc = glGetUniformLocation(bp, "proj_matrix");
        mvLoc = glGetUniformLocation(bp, "mv_matrix");

        glUseProgram(bp);

        float* vals = getVals(perspectiveMat);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, vals);
        free(vals);

        m->rotation->y += .001;

        mat4 mvMat = fromPositionAndRotation(*(m->position), *(m->rotation));
        vals = getVals(mvMat);
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, vals);
        free(vals);

        glBindVertexArray(vaoId);
        glBindBuffer(GL_ARRAY_BUFFER, vboId);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(0);

        glPointSize(30);
        glDrawArrays(GL_TRIANGLES, 0, (sizeof(verts) / sizeof(verts[0])) / 3);


        SwapBuffers(hdc);
    }
}

void init() {
    m = calloc(1, sizeof(model));
    m->verts = verts;

    (m->position) = calloc(1, sizeof(vec3));
    *(m->position) = (vec3){0, 0, -2};

    (m->rotation) = calloc(1, sizeof(vec3));
    *(m->rotation) = (vec3){0, rad(-30), 0};

    wglSwapIntervalEXT(0);

    bp = createBasicProgram();

    glGenVertexArrays(1, &vaoId);
    checkGLError("glGenVertexArrays");

    glBindVertexArray(vaoId);
    checkGLError("glBindVertexArray");

    glGenBuffers(1, &vboId);
    checkGLError("glGenBuffers");

    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    checkGLError("glBindBuffer");

    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    checkGLError("glBufferData");

    const char* v = (const char*)glGetString(GL_VERSION);
    print("version: %s\n", v);
}

void updateSize(int w, int h) {
    getPerspectiveMatrix(&perspectiveMat, 70.0, (double)w/(double)h, 1, 100);
}

void frameTick(HWND hWnd) {
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
