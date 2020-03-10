#pragma once

#include <stdio.h>
#include "glMath.h"

//#define installDirectory "D:\\CodeBlocksWorkspace\\GLTesting3\\"
#define installDirectory "C:\\Users\\dafie\\Desktop\\codeBlocksWorkspace\\GLTesting3\\"

typedef struct mesh {
    vec3* verts;
    vec3* normals;
    vec2* texCoordsOrdered;
    int numVerts;
    int numFaces;
    vec3* vertsOrdered;
    vec3* normalsOrdered;
} MESH;

typedef struct {
    int numLines;
    char** lines;
    int* lengths;
} FILEDATA;

char** strsplit(const char*, const char*); // returns an array of strings by splitting the first string by the second string
int getFileLength(FILE*); //returns the number of lines in the file
FILEDATA* getFileData(FILE*); // returns the lines in the file as an array of strings
FILE* getFile(const char*); //loads the file based on the given filename
void freeFileData(FILEDATA*); // frees all the memory in a filedata struct
void printFileData(FILEDATA*); // prints the filedata struct in a human-readable way
char* lstrip(const char*); // removes all whitespace left of the first non-whitespace character in a string
MESH* getMeshData(const char*);
