#include "glExtensions.h"

#include <windows.h>
#include <gl/gl.h>
#include "consoleUtil.h"

void* getProc(HANDLE hOut, const char* name) {
    print(hOut, "loading %s\n", name);
    void* p = wglGetProcAddress(name);
    if (p == NULL) {
        int e = GetLastError();
        print(hOut, "error loading %s: %i\n", name, e);
        return NULL;
    }
    if(p == 0 ||
        (p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
        (p == (void*)-1) )
    {
        HMODULE module = LoadLibraryA("opengl32.dll");
        if (module == NULL) {
            int e = GetLastError();
            print(hOut, "error loading library \"opengl32.dll\" for function pointer %s: %i\n", name, e);
            return NULL;
        }
        p = GetProcAddress(module, name);
        if (p == NULL) {
            int e = GetLastError();
            print(hOut, "error loading %s with getProcAddress: %i\n", name, e);
            return NULL;
        }
    }

    print(hOut, "success loading\n");
    return p;
}

void checkGLError(HANDLE hOut, char* desc) {
    GLenum e = glGetError();
    if (e > 0) {
        print(hOut, "Error %i: %s\n", e, desc);
        return;
    }
    print(hOut, "%s completed with no errors\n", desc);
}

void GLEInit(HANDLE hOut) {
    glCreateProgram = (PFNGLCREATEPROGRAMPROC)getProc(hOut, "glCreateProgram");
    glCreateShader = (PFNGLCREATESHADERPROC)getProc(hOut, "glCreateShader");
    glShaderSource = (PFNGLSHADERSOURCEPROC)getProc(hOut, "glShaderSource");
    glCompileShader = (PFNGLCOMPILESHADERPROC)getProc(hOut, "glCompileShader");
    glAttachShader = (PFNGLATTACHSHADERPROC)getProc(hOut, "glAttachShader");
    glLinkProgram = (PFNGLLINKPROGRAMPROC)getProc(hOut, "glLinkProgram");
    //glUseProgram = (PFNGLUSEPROGRAMPROC)getProc(hOut, "glUseProgram");
    glUseProgram = (PFNGLUSEPROGRAMPROC)wglGetProcAddress("glUseProgram");
    glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC)getProc(hOut, "glGetShaderInfoLog");
    glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC)getProc(hOut, "glGetProgramInfoLog");
    glDeleteShader = (PFNGLDELETESHADERPROC)getProc(hOut, "glDeleteShader");
    glDetachShader = (PFNGLDETACHSHADERPROC)getProc(hOut, "glDetachShader");
    glIsProgram = (PFNGLISPROGRAMPROC)getProc(hOut, "glIsProgram");
    glGenBuffers = (PFNGLGENBUFFERSPROC)getProc(hOut, "glGenBuffers");
    glBindBuffer = (PFNGLBINDBUFFERPROC)getProc(hOut, "glBindBuffer");
    glBufferData = (PFNGLBUFFERDATAPROC)getProc(hOut, "glBufferData");
    glEnableVertexAttribArray = (PFNGLENABLEVERTEXATTRIBARRAYPROC)getProc(hOut, "glEnableVertexAttribArray");
    glVertexAttribPointer = (PFNGLVERTEXATTRIBPOINTERPROC)getProc(hOut, "glVertexAttribPointer");
    glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)getProc(hOut, "glGenVertexArrays");
    glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)getProc(hOut, "glBindVertexArray");
    glGetIntegeri_v = (PFNGLGETINTEGERI_VPROC)getProc(hOut, "glGetIntegeri_v");
    glGetStringi = (PFNGLGETSTRINGIPROC)getProc(hOut, "glGetStringi");
}
