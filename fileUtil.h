#pragma once

#include <stdio.h>
#include "glMath.h"

typedef struct mesh {
    float* verts;
    vec3* rotation;
    vec3* position;
} MESH;

typedef struct {
    int numLines;
    char** lines;
    int* lengths;
} FILEDATA;

int getFileLength(FILE*); //returns the number of lines in the file
FILEDATA* getFileData(FILE*); // returns the lines in the file as an array of strings
FILE* getFile(const char*); //loads the file based on the given filename
void freeFileData(FILEDATA*); // frees all the memory in a filedata struct
void printFileData(FILEDATA*); // prints the filedata struct in a human-readable way
char* lstrip(const char*); // removes all whitespace left of the first non-whitespace character in a string
MESH* getMeshData(const char*);
