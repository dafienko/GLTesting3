#include <stdio.h>
#include <windows.h>
#include <gl/gl.h>

#include "glExtensions.h"
#include "fileUtil.h"
#include "shaderUtil.h"
#include "consoleUtil.h"

#define ERROR_BUFFER_SIZE 1000

char errorBuffer[ERROR_BUFFER_SIZE];

int createBasicProgram(HANDLE hOut) {
    //vertex shader bullcrap
    FILE* basicVSFile = getFile("shaders/basic.vs");
    FILEDATA* vsfd = getFileData(hOut, basicVSFile);
    fclose(basicVSFile);

    int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, vsfd->numLines, vsfd->lines, vsfd->lengths);
    checkGLError(hOut, "vertex shader source");
    glCompileShader(vs);
    checkGLError(hOut, "vertex shader compilation");

    int actualLength = 0;
    glGetShaderInfoLog(vs, ERROR_BUFFER_SIZE, &actualLength, errorBuffer);
    if (actualLength > 0) {
        print(hOut, "deleting vertex shader\n");
        glDeleteShader(vs);
        print(hOut, errorBuffer);
        return -1;
    }
    freeFileData(vsfd);


    //fragment shader bullcrap
    FILE* basicFSFile = getFile("shaders/basic.fs");
    FILEDATA* fsfd = getFileData(hOut, basicFSFile);
    fclose(basicFSFile);

    int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, fsfd->numLines, fsfd->lines, fsfd->lengths);
    checkGLError(hOut, "fragment shader source");
    glCompileShader(fs);
    checkGLError(hOut, "fragment shader compilation");

    glGetShaderInfoLog(fs, ERROR_BUFFER_SIZE, &actualLength, errorBuffer);
    if (actualLength > 0) {
        print(hOut, "deleting fragment shader\n");
        glDeleteShader(fs);
        print(hOut, errorBuffer);
        return -1;
    }
    freeFileData(fsfd);

    int progId = glCreateProgram();
    checkGLError(hOut, "program creation");
    glAttachShader(progId, vs);
    checkGLError(hOut, "vertex shader attachment");
    glAttachShader(progId, fs);
    checkGLError(hOut, "fragment shader attachment");
    glLinkProgram(progId);
    checkGLError(hOut, "program linking");
    //glGetProgramInfoLog(progId, ERROR_BUFFER_SIZE, &actualLength, errorBuffer);
    if (actualLength > 0) {
        print(hOut, errorBuffer);
    }

    glDetachShader(progId, vs);
    checkGLError(hOut, "vertex shader detachment");
    glDetachShader(progId, fs);
    checkGLError(hOut, "fragment shader detachment");
    glDeleteShader(vs);
    checkGLError(hOut, "vertex shader deletion");
    glDeleteShader(fs);
    checkGLError(hOut, "fragment shader deletion");

    return progId;
}










