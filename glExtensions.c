#include "glExtensions.h"

#include "ghettoWin.h"
#include <gl/gl.h>
#include "consoleUtil.h"

void* getProc(const char* name) {
    print("loading %s\n", name);
    void* p = wglGetProcAddress(name);
    if (p == NULL) {
        int e = GetLastError();
        print("error loading %s: %i\n", name, e);
        return NULL;
    }
    if(p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1) )
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        if (module == NULL) {
            int e = GetLastError();
            print("error loading library \"opengl32.dll\" for function pointer %s: %i\n", name, e);
            return NULL;
        }
        p = GetProcAddress(module, name);
        if (p == NULL) {
            int e = GetLastError();
            print("error loading %s with getProcAddress: %i\n", name, e);
            return NULL;
        }
    }

    print("success loading\n");
    return p;
}

void checkGLError(char* desc) {
    GLenum e = glGetError();
    if (e > 0) {
        print("Error %i: %s\n", e, desc);
        return;
    }
    print("%s completed with no errors\n", desc);
}

void GLEInit() {
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProc("glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)getProc("glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)getProc("glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)getProc("glCompileShader");
    glAttachShader = (PFNGLATTACHSHADERPROC)getProc("glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)getProc("glLinkProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)getProc("glUseProgram");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProc("glGetShaderInfoLog");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProc("glGetProgramInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)getProc("glDeleteShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)getProc("glDetachShader");
    glIsProgram = (PFNGLISPROGRAMPROC)getProc("glIsProgram");
    glGenBuffers = (PFNGLGENBUFFERSPROC)getProc("glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)getProc("glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)getProc("glBufferData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getProc("glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getProc("glVertexAttribPointer");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getProc("glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getProc("glBindVertexArray");
    glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)getProc("glGetIntegeri_v");
    glGetStringi = (PFNGLGETSTRINGIPROC)getProc("glGetStringi");
    wglSwapIntervalEXT = (PFNWGLSWAPINTERVALEXTPROC)getProc("wglSwapIntervalEXT");
    wglGetSwapIntervalEXT = (PFNWGLGETSWAPINTERVALEXTPROC)getProc("wglGetSwapIntervalEXT");
    glUniformMatrix4fv = (PFNGLUNIFORMMATRIX4FVPROC)getProc("glUniformMatrix4fv");
    glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC)getProc("glGetUniformLocation");
    glUniform3f = (PFNGLUNIFORM3FPROC) getProc("glUniform3f");
    initialized = 1;
}
