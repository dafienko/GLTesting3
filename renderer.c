#include <stdio.h>
#include <stdlib.h>
#include "ghettoWin.h"
#include <sys/timeb.h>
#include <time.h>
#include <gl/gl.h>

#include "glMath.h"
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
GLint projLoc, mvLoc, camposLoc, modelposLoc, scaleLoc, mLoc;

#define tPos 1
#define yOff 1

//ccw = outward normal

int timeSincePhysicsUpdate = 0;
const int timeBetweenPhysicsUpdates = 10; // milliseconds
float dtMs;
int ms;

MESH* monkeyMesh;
INSTANCE* monkey;




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

        monkey->rotation.y += dtMs * (PI / 6.0);

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
        mLoc = glGetUniformLocation(bp, "m_matrix");

        glUseProgram(bp);

        float* vals = getVals(perspectiveMat);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, vals);
        free(vals);

        mat4 vMat = fromPositionAndRotation(inverseVec3(c->position), inverseVec3(c->rotation));
        mat4 mMat = rotateXYZ(monkey->rotation.x, monkey->rotation.y, monkey->rotation.z);
        mMat.d = monkey->position.x;
        mMat.h = monkey->position.y;
        mMat.l = monkey->position.z;


        mat4 mvMat = mulMat(mulMat(identityMatrix, vMat), mMat);
        //mat4 mvMat = mulMat(mulMat(identityMatrix, mMat), vMat);


        vals = getVals(mMat);
        glUniformMatrix4fv(mLoc, 1, GL_FALSE, vals);
        free(vals);

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

        glDrawArrays(GL_TRIANGLES, 0, monkey->mesh->numFaces * 3 * 3);

        SwapBuffers(hdc);
    }
}

void initRenderer(const char* cmd) {
    bp = createBasicProgram();
    monkey = calloc(1, sizeof(INSTANCE));

    if (strlen(cmd) > 1) {
        char* betterCmd = calloc(strlen(cmd) + 1, sizeof(char));
        int charsUsed = 0;
        for (int i = 0; i < strlen(cmd); i++) {
            if (*(cmd + i) != '"') {
                *(betterCmd + charsUsed) = *(cmd + i);
                charsUsed++;
            }
        }

        monkeyMesh = getMeshData(betterCmd);
        free(betterCmd);
    } else {
        monkeyMesh = getMeshData("C:\\Users\\dafie\\Desktop\\codeBlocksWorkspace\\GLTesting3\\assets\\models\\oem.obj");
    }


    float maxSize = 0;
    for (int i = 0; i < monkeyMesh->numFaces; i++) {
        vec3 a = *(monkeyMesh->vertsOrdered + i * 3 + 0);
        vec3 b = *(monkeyMesh->vertsOrdered + i * 3 + 1);
        vec3 c = *(monkeyMesh->vertsOrdered + i * 3 + 2);

        maxSize = max(maxSize, vec3Magnitude(a));
        maxSize = max(maxSize, vec3Magnitude(b));
        maxSize = max(maxSize, vec3Magnitude(c));
    }

    float s = 100 / maxSize;

    camera = calloc(1, sizeof(CAMERA));
    camera->position = (vec3){0, 0, 0};
    camera->rotation = (vec3){-PI/9, 0, 0};

    monkey->position = (vec3){0, -64, -200};
    monkey->scale = (vec3){s, s, s};
    monkey->rotation = (vec3){0, -PI/4, 0};
    monkey->mesh = monkeyMesh;

    wglSwapIntervalEXT(1);

    glGenVertexArrays(1, &vaoId);

    glBindVertexArray(vaoId);

    glGenBuffers(2, vboId);

    glBindBuffer(GL_ARRAY_BUFFER, vboId[0]);
    glBufferData(GL_ARRAY_BUFFER, monkey->mesh->numFaces * sizeof(float) * 3 * 3, monkey->mesh->vertsOrdered, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vboId[1]);
    glBufferData(GL_ARRAY_BUFFER, monkey->mesh->numFaces * sizeof(float) * 3 * 3, monkey->mesh->normalsOrdered, GL_STATIC_DRAW);

    const char* v = (const char*)glGetString(GL_VERSION);
    print("version: %s\n", v);
    //*/
}

void updateSize(int w, int h) {
    getPerspectiveMatrix(&perspectiveMat, 70.0, (double)w/(double)h, 2, 10000);
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
