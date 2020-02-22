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
#include "game.h"

GLuint bp = 0;
GLuint vboId = 0;
GLuint vaoId = 0;
int framesSinceLastUpdate = 0;
int lastMs = 0;
int lastFrameTime = 0;
int fps = 0;

//uniforms
GLint projLoc, mvLoc;
MESH* m;

#define tPos 1
#define yOff 1

 GLfloat verts[] = { //ccw = outward normal
            //back face
            tPos, tPos, -tPos,
            tPos, -tPos, -tPos,
            -tPos, -tPos,-tPos,
            tPos, tPos, -tPos,
            -tPos, -tPos, -tPos,
            -tPos, tPos, -tPos,

            //front face
            tPos, tPos, tPos,
            -tPos, -tPos,tPos,
            tPos, -tPos, tPos,
            tPos, tPos, tPos,
            -tPos, tPos, tPos,
            -tPos, -tPos, tPos,

            //right face
            tPos, tPos, tPos,
            tPos, -tPos, tPos,
            tPos, -tPos, -tPos,
            tPos, tPos, tPos,
            tPos, -tPos, -tPos,
            tPos, tPos, -tPos,

            //left face
            -tPos, tPos, tPos,
            -tPos, -tPos, -tPos,
            -tPos, -tPos, tPos,
            -tPos, tPos, tPos,
            -tPos, tPos, -tPos,
            -tPos, -tPos, -tPos,

            //top face
            -tPos, tPos, -tPos,
            -tPos, tPos, tPos,
            tPos, tPos, tPos,
            -tPos, tPos, -tPos,
            tPos, tPos, tPos,
            tPos, tPos, -tPos,

            -tPos, -tPos, -tPos,
            tPos, -tPos, tPos,
            -tPos, -tPos, tPos,
            -tPos, -tPos, -tPos,
            tPos, -tPos, -tPos,
            tPos, -tPos, tPos,
};

int timeSincePhysicsUpdate = 0;
const int timeBetweenPhysicsUpdates = 50; // milliseconds

int displayEnabled = 1; // for use with debugging shaders
void display(CAMERA* c, HDC hdc, HWND hWnd) {  //display function
    if (displayEnabled) {
        int ms = frameTick(hWnd);
        float dt = ms / 1000.0;
        updateFrame(dt);
        timeSincePhysicsUpdate += ms;
        if (timeSincePhysicsUpdate >= timeBetweenPhysicsUpdates) {
            timeSincePhysicsUpdate -= timeBetweenPhysicsUpdates;
            updateGame(timeBetweenPhysicsUpdates);
        }

        glEnable(GL_CULL_FACE);
        float col = .15f;
        glClearColor(col, col, col, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        projLoc = glGetUniformLocation(bp, "proj_matrix");
        mvLoc = glGetUniformLocation(bp, "mv_matrix");

        glUseProgram(bp);

        float* vals = getVals(perspectiveMat);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, vals);
        free(vals);

        //m->rotation->y += .001;

        mat4 vMat = fromPositionAndRotation(inverseVec3(*(c->position)), inverseVec3(*(c->rotation)));
        mat4 mMat = fromPositionAndRotation(*(m->position), *(m->rotation));
        mat4 mvMat = mulMat(mulMat(identityMatrix, vMat), mMat);

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

void initRenderer() {
    camera = calloc(1, sizeof(CAMERA));
    (camera->position) = calloc(1, sizeof(vec3));
    *(camera->position) = (vec3){0, 0, 0};

    (camera->rotation) = calloc(1, sizeof(vec3));
    *(camera->rotation) = (vec3){0, 0, 0};

    m = calloc(1, sizeof(MESH));
    m->verts = verts;

    (m->position) = calloc(1, sizeof(vec3));
    *(m->position) = (vec3){0, 0, -2};

    (m->rotation) = calloc(1, sizeof(vec3));
    *(m->rotation) = (vec3){0, 0, 0};

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
    getPerspectiveMatrix(&perspectiveMat, 70.0, (double)w/(double)h, .1, 100);
}

int frameTick(HWND hWnd) {
    int ms = GetTickCount();
    int dt = ms - lastFrameTime;
    if (dt < 0) {
        lastFrameTime = GetTickCount();
        return 0;
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

    return dt;
}
