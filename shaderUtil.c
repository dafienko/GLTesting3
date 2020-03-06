#include <stdio.h>
#include "ghettoWin.h"
#include <gl/gl.h>
#include <stdlib.h>

#include "glExtensions.h"
#include "fileUtil.h"
#include "shaderUtil.h"
#include "consoleUtil.h"

#define ERROR_BUFFER_SIZE 1000

char errorBuffer[ERROR_BUFFER_SIZE];

int createBasicProgram() {
    //vertex shader bullcrap
    char* fileName = calloc(200, sizeof(char));
    sprintf(fileName, "%sshaders\\basic.vs", installDirectory);
    FILE* basicVSFile = getFile(fileName);
    free(fileName);
    FILEDATA* vsfd = getFileData(basicVSFile);
    fclose(basicVSFile);

    int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, vsfd->numLines, (const char**)vsfd->lines, (const int*)vsfd->lengths);
    checkGLError("vertex shader source");
    glCompileShader(vs);
    checkGLError("vertex shader compilation");

    int actualLength = 0;
    glGetShaderInfoLog(vs, ERROR_BUFFER_SIZE, &actualLength, errorBuffer);
    if (actualLength > 0) {
        print("deleting vertex shader\n");
        glDeleteShader(vs);
        print(errorBuffer);
        return -1;
    }
    freeFileData(vsfd);


    //fragment shader bullcrap
    fileName = calloc(200, sizeof(char));
    sprintf(fileName, "%sshaders\\basic.fs", installDirectory);
    FILE* basicFSFile = getFile(fileName);
    free(fileName);
    FILEDATA* fsfd = getFileData(basicFSFile);
    fclose(basicFSFile);

    int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, fsfd->numLines, (const char**)fsfd->lines, (const int*)fsfd->lengths);
    checkGLError("fragment shader source");
    glCompileShader(fs);
    checkGLError("fragment shader compilation");

    glGetShaderInfoLog(fs, ERROR_BUFFER_SIZE, &actualLength, errorBuffer);
    if (actualLength > 0) {
        print("deleting fragment shader\n");
        glDeleteShader(fs);
        print(errorBuffer);
        return -1;
    }
    freeFileData(fsfd);

    int progId = glCreateProgram();
    checkGLError("program creation");
    glAttachShader(progId, vs);
    checkGLError("vertex shader attachment");
    glAttachShader(progId, fs);
    checkGLError("fragment shader attachment");
    glLinkProgram(progId);
    checkGLError("program linking");
    //glGetProgramInfoLog(progId, ERROR_BUFFER_SIZE, &actualLength, errorBuffer);
    if (actualLength > 0) {
        print(errorBuffer);
    }

    glDetachShader(progId, vs);
    checkGLError("vertex shader detachment");
    glDetachShader(progId, fs);
    checkGLError("fragment shader detachment");
    glDeleteShader(vs);
    checkGLError("vertex shader deletion");
    glDeleteShader(fs);
    checkGLError("fragment shader deletion");

    return progId;
}










