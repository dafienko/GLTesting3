#include <stdio.h>
#include <stdlib.h>
#include "ghettoWin.h"
#include <sys/timeb.h>
#include <time.h>
#include <gl/gl.h>
#include <math.h>

#include "keyboard.h"
#include "glMath.h"
#include "renderer.h"
#include "glExtensions.h"
#include "shaderUtil.h"
#include "fileUtil.h"
#include "consoleUtil.h"
#include "game.h"

GLuint bp = 0;
GLuint vboId[6];
GLuint vaoId = 0;

//uniforms
GLint projLoc, mvLoc, camposLoc, modelposLoc, scaleLoc, mLoc, wfColLoc, wfEnabledLoc, transparencyLoc;

#define tPos 1
#define yOff 1

int timeSincePhysicsUpdate = 0;
const int timeBetweenPhysicsUpdates = 10; // milliseconds
float dtMs;
int ms;

MESH* monkeyMesh;
INSTANCE* monkey;

void drawInstance(INSTANCE*, mat4, int);


int displayEnabled = 1; // for use with debugging shaders
void display(CAMERA* c, HDC hdc, HWND hWnd) {  //display function
    if (displayEnabled) {
        ms = frameTick(hWnd);
        dtMs = ms / 1000.0; // 1000 ms in one second
        updateFrame(dtMs);
        timeSincePhysicsUpdate += ms;
        while (timeSincePhysicsUpdate >= timeBetweenPhysicsUpdates) {
            timeSincePhysicsUpdate -= timeBetweenPhysicsUpdates;
            updateGame(timeBetweenPhysicsUpdates/1000.0); // 1000 ms in one second
        }

        monkey->rotation.y += dtMs * (PI / 12.0);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
        wfColLoc = glGetUniformLocation(bp, "wireframeColor");
        wfEnabledLoc = glGetUniformLocation(bp, "wireframeEnabled");
        transparencyLoc = glGetUniformLocation(bp, "transparency");

        glUseProgram(bp);

        ms = GetTickCount();
        vec3 wfColor = (vec3){(sin(ms/500.0) + 1.0) * .5, (sin(ms/500.0) + 1.0) * .5, 1};
        glUniform3f(camposLoc, c->position.x, c->position.y, c->position.z);

        float* vals = getVals(perspectiveMat);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, vals);
        free(vals);

        glUniform3f(wfColLoc, wfColor.x, wfColor.y, wfColor.z);

        mat4 vMat = fromPositionAndRotation(inverseVec3(c->position), inverseVec3(c->rotation));

        glLineWidth(2);

        int instancesDrawn = 0;
        for (int i = 0; i < MAX_INSTANCES; i++) {
            if (*(instances + i) != 0) {
                INSTANCE* inst = *(instances + i);
                switch(drawMode) {
                case DM_FACE:
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                    glUniform1i(wfEnabledLoc, 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    drawInstance(inst, vMat, 1);
                    break;
                case DM_FACEANDLINE:
                    glEnable(GL_CULL_FACE);
                    glCullFace(GL_BACK);
                    glUniform1i(wfEnabledLoc, 0);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
                    drawInstance(inst, vMat, 1);
                case DM_LINE:
                    glDisable(GL_CULL_FACE);
                    glUniform1i(wfEnabledLoc, 1);
                    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                    glDepthFunc(GL_LEQUAL);
                    drawInstance(inst, vMat, 0);
                    break;
                }
            }
        }


        SwapBuffers(hdc);
    }
}

void initRenderer(const char* cmd) {
    drawMode = DM_FACE;

    instances = (INSTANCE**)calloc(sizeof(INSTANCE*), MAX_INSTANCES);

    bp = createBasicProgram();

    if (strlen(cmd) > 1) {
        char* betterCmd = calloc(strlen(cmd) + 1, sizeof(char));
        int charsUsed = 0;
        for (int i = 0; i < strlen(cmd); i++) {
            if (*(cmd + i) != '"') {
                *(betterCmd + charsUsed) = *(cmd + i);
                charsUsed++;
            }
        }

        monkey = createInstanceFromFile(betterCmd);
        free(betterCmd);
    } else {
        char fb[100];
        sprintf(fb, "%sassets\\models\\sphere.obj", installDirectory);
        monkey = createInstanceFromFile(fb);
    }

    char* name = calloc(sizeof(char), 200);

    sprintf(name, "%sassets\\models\\oem.obj", installDirectory);
    INSTANCE* oem = createInstanceFromFile(name);
    oem->position = (vec3){250, 0, 0};
    oem->name = "OEM";
    addInstance(oem);

    sprintf(name, "%sassets\\models\\mindbender.obj", installDirectory);
    INSTANCE* mb = createInstanceFromFile(name);
    mb->name = "mindbender";
    mb->position = (vec3){500, 0, 0};
    addInstance(mb);

    sprintf(name, "%sassets\\models\\monkey.obj", installDirectory);
    INSTANCE* mon = createInstanceFromFile(name);
    mon->name = "monkey";
    mon->position = (vec3){500, 0, 250};
    addInstance(mon);

    monkey->name = "sphere";
    mb->parent = oem;
    mon->parent = oem;
    monkey->parent = mon;

    free(name);
    addInstance(monkey);

    camera = calloc(1, sizeof(CAMERA));
    camera->position = (vec3){0, 0, 0};
    camera->rotation = (vec3){-PI/9, 0, 0};

    wglSwapIntervalEXT(0);

    const char* v = (const char*)glGetString(GL_VERSION);
    print("version: %s\n", v);
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

INSTANCE* createInstanceFromFile(const char* filename) {
    INSTANCE* inst = calloc(1, sizeof(INSTANCE));

    inst->mesh = getMeshData(filename);

    float maxSize = 0;
    for (int i = 0; i < inst->mesh->numFaces; i++) {
        vec3 a = *(inst->mesh->vertsOrdered + i * 3 + 0);
        vec3 b = *(inst->mesh->vertsOrdered + i * 3 + 1);
        vec3 c = *(inst->mesh->vertsOrdered + i * 3 + 2);

        maxSize = max(maxSize, vec3Magnitude(a));
        maxSize = max(maxSize, vec3Magnitude(b));
        maxSize = max(maxSize, vec3Magnitude(c));
    }

    float s = 100 / maxSize;

    inst->position = (vec3){0, -64, -200};
    inst->scale = (vec3){s, s, s};
    inst->rotation = (vec3){0, -PI/4, 0};
    inst->transparency = 0;

    glGenVertexArrays(1, &(inst->vao));

    glBindVertexArray(inst->vao);

    glGenBuffers(2, inst->vbo);

    glBindBuffer(GL_ARRAY_BUFFER, inst->vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, inst->mesh->numFaces * sizeof(vec3) * 3, inst->mesh->vertsOrdered, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, inst->vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, inst->mesh->numFaces * sizeof(vec3) * 3, inst->mesh->normalsOrdered, GL_STATIC_DRAW);

    return inst;
}

void drawInstance(INSTANCE* inst, mat4 vMat, int cull) {

    mat4 mMat = rotateXYZ(inst->rotation.x, inst->rotation.y, inst->rotation.z);
    mMat.d = inst->position.x;
    mMat.h = inst->position.y;
    mMat.l = inst->position.z;

    mat4 mvMat = mulMat(mulMat(identityMatrix, vMat), mMat);

    float* vals = getVals(mMat);
    glUniformMatrix4fv(mLoc, 1, GL_FALSE, vals);
    free(vals);

    vals = getVals(mvMat);
    glUniformMatrix4fv(mvLoc, 1, GL_FALSE, vals);
    free(vals);

    glUniform3f(modelposLoc, inst->position.x, inst->position.y, inst->position.z);
    glUniform3f(scaleLoc, inst->scale.x, inst->scale.y, inst->scale.z);

    glUniform1f(transparencyLoc, inst->transparency);


    glBindVertexArray(inst->vao);

    glBindBuffer(GL_ARRAY_BUFFER, inst->vbo[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, inst->vbo[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    if (inst->transparency == 0 && cull) {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        glDisable(GL_CULL_FACE);
    }

    glDrawArrays(GL_TRIANGLES, 0, inst->mesh->numFaces * 3 * 3);
}

int numInstances = 0;
int addInstance(INSTANCE* inst) {
    if (numInstances >= MAX_INSTANCES) {
        return -1;
    }

    for (int i = 0; i < MAX_INSTANCES; i++) {
        if (*(instances + i) == NULL) {
            numInstances++;
            *(instances + i) = inst;
            return i;
        }
    }
    return -1;
}

void removeInstance(int instanceHandle) {
    numInstances--;
    *(instances + instanceHandle) = NULL;
}




