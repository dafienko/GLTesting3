#include <stdio.h>
#include <stdlib.h>
#include "ghettoWin.h"
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
GLuint vboId[2];
GLuint vaoId = 0;

//uniforms
GLint projLoc, mvLoc, camposLoc, modelposLoc, scaleLoc;

#define tPos 1
#define yOff 1

//ccw = outward normal
 vec3 verts[] = { //back face
            {tPos, tPos, -tPos},
            {tPos, -tPos, -tPos},
            {-tPos, -tPos,-tPos},
            {tPos, tPos, -tPos},
            {-tPos, -tPos, -tPos},
            {-tPos, tPos, -tPos},

            //front face
            {tPos, tPos, tPos},
            {-tPos, -tPos,tPos},
            {tPos, -tPos, tPos},
            {tPos, tPos, tPos},
            {-tPos, tPos, tPos},
            {-tPos, -tPos, tPos},

            //right face
            {tPos, tPos, tPos},
            {tPos, -tPos, tPos},
            {tPos, -tPos, -tPos},
            {tPos, tPos, tPos},
            {tPos, -tPos, -tPos},
            {tPos, tPos, -tPos},

            //left face
            {-tPos, tPos, tPos},
            {-tPos, -tPos, -tPos},
            {-tPos, -tPos, tPos},
            {-tPos, tPos, tPos},
            {-tPos, tPos, -tPos},
            {-tPos, -tPos, -tPos},

            //top face
            {-tPos, tPos, -tPos},
            {-tPos, tPos, tPos},
            {tPos, tPos, tPos},
            {-tPos, tPos, -tPos},
            {tPos, tPos, tPos},
            {tPos, tPos, -tPos},

            {-tPos, -tPos, -tPos},
            {tPos, -tPos, tPos},
            {-tPos, -tPos, tPos},
            {-tPos, -tPos, -tPos},
            {tPos, -tPos, -tPos},
            {tPos, -tPos, tPos}
};

int timeSincePhysicsUpdate = 0;
const int timeBetweenPhysicsUpdates = 10; // milliseconds
float dtMs;
int ms;

MESH* monkey;





int displayEnabled = 1; // for use with debugging shaders
void display(CAMERA* c, HDC hdc, HWND hWnd) {  //display function
    if (displayEnabled) {
        ms = frameTick(hWnd);
        dtMs = ms / 1000.0;
        updateFrame(dtMs);

        timeSincePhysicsUpdate += ms;
        while (timeSincePhysicsUpdate >= timeBetweenPhysicsUpdates) {
            timeSincePhysicsUpdate -= timeBetweenPhysicsUpdates;
            updateGame(timeBetweenPhysicsUpdates/1000.0);
        }

        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        float col = .15f;
        glClearColor(col, col, col, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);

        projLoc = glGetUniformLocation(bp, "proj_matrix");
        mvLoc = glGetUniformLocation(bp, "mv_matrix");
        camposLoc = glGetUniformLocation(bp, "cameraPos");
        modelposLoc = glGetUniformLocation(bp, "modelPos");
        scaleLoc = glGetUniformLocation(bp, "scale");

        glUseProgram(bp);

        float* vals = getVals(perspectiveMat);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, vals);
        free(vals);

        //m->rotation->y += .001;

        mat4 vMat = fromPositionAndRotation(inverseVec3(c->position), inverseVec3(c->rotation));
        mat4 mMat = fromPositionAndRotation(monkey->position, monkey->rotation);
        mat4 mvMat = mulMat(mulMat(identityMatrix, vMat), mMat);

        vals = getVals(mvMat);
        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, vals);
        free(vals);

        glUniform3f(camposLoc, c->position.x, c->position.y, c->position.z);
        glUniform3f(modelposLoc, monkey->position.x, monkey->position.y, monkey->position.z);
        glUniform3f(scaleLoc, monkey->scale.x, monkey->scale.y, monkey->scale.z);

        glBindVertexArray(vaoId);

        glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, monkey->numFaces * 3 * 3);

        SwapBuffers(hdc);
    }
}

void initRenderer() {
    monkey = getMeshData("assets/models/mindbender.obj");

    ///*
    camera = calloc(1, sizeof(CAMERA));
    camera->position = (vec3){0, 0, 0};
    camera->rotation = (vec3){0, 0, 0};

    monkey->position = (vec3){0, 0, -5};
    monkey->scale = (vec3){.1, .1, .1};
    monkey->rotation = (vec3){0, 0, 0};

    wglSwapIntervalEXT(1);

    bp = createBasicProgram();

    glGenVertexArrays(1, &vaoId);
    checkGLError("glGenVertexArrays");

    glBindVertexArray(vaoId);
    checkGLError("glBindVertexArray");

    glGenBuffers(2, vboId);
    checkGLError("glGenBuffers");

    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboId[1]);
    //checkGLError("glBindBuffer0");
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, monkey->numFaces * 3 * sizeof(int), monkey->faces, GL_STATIC_DRAW);
    //checkGLError("glBufferIndices");

    glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
    checkGLError("glBindBuffer1");
    glBufferData(GL_ARRAY_BUFFER, monkey->numFaces * sizeof(float) * 3 * 3, monkey->vertsOrdered, GL_STATIC_DRAW);
    checkGLError("glBufferData");

    glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
    checkGLError("glBindBuffer1");
    glBufferData(GL_ARRAY_BUFFER, monkey->numFaces * sizeof(float) * 3 * 3, monkey->normalsOrdered, GL_STATIC_DRAW);
    checkGLError("glBufferData");

    const char* v = (const char*)glGetString(GL_VERSION);
    print("version: %s\n", v);
    //*/
}

void updateSize(int w, int h) {
    getPerspectiveMatrix(&perspectiveMat, 70.0, (double)w/(double)h, .1, 1000);
}

int lastTime = 0, dt = 0, timeSinceLastFrameUpdate = 0, lastFrameTime = 0, fps = 0, framesSinceLastUpdate = 0;
char windowText[12];

int frameTick(HWND hWnd) {
    if (lastTime == 0) { // first run through
        lastFrameTime = GetTickCount();
        lastTime = GetTickCount();
        return 0;
    }

    ms = GetTickCount();
    dt = ms - lastTime;
    lastTime = ms;

    framesSinceLastUpdate++;

    timeSinceLastFrameUpdate = ms - lastFrameTime;
    if (timeSinceLastFrameUpdate > 1000) {
        lastFrameTime = ms;
        fps = (1000 * framesSinceLastUpdate)/(timeSinceLastFrameUpdate);

        sprintf(windowText, "%i FPS", fps);
        SetWindowText(hWnd, TEXT(windowText));

        framesSinceLastUpdate = 0;
    }

    return dt;
}
