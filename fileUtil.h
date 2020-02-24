#pragma once

#include <stdio.h>

typedef struct {
    int numLines;
    char** lines;
    int* lengths;
} FILEDATA;

int getFileLength(FILE*); //returns the number of lines in the file
FILEDATA* getFileData(FILE*); // returns the lines in the file as an array of strings
FILE* getFile(const char*); //loads the file based on the given filename
void freeFileData(FILEDATA*);
void printFileData(FILEDATA*);
