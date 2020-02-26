#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ghettoWin.h"
#include "fileUtil.h"
#include "consoleUtil.h"


FILE* getFile(const char* pcstrFileName) {
    FILE* fptrResult = fopen(pcstrFileName, "r");

    return fptrResult;
}

void printFileData(FILEDATA* fd) {
    for (int i = 0; i < fd->numLines; i++) {
        print(*(fd->lines + i));
    }
}

int getFileLength(FILE* pfFile) { //returns the number of lines in the file
    int iNumLines = 1;
    char c;
    do {
        c = fgetc(pfFile);
        if (c == '\n') {
            iNumLines++;
        }
    } while (c != EOF);

    rewind(pfFile);

    return iNumLines;
}

FILEDATA* getFileData(FILE* pfFile) {
    int iNumLines = getFileLength(pfFile);
    char** ppcstrLines = calloc(iNumLines, sizeof(char*));
    int* piLengths = calloc(iNumLines, sizeof(int));
    int lineStart = 0;

    for (int l = 0; l < iNumLines; l++) {
        int iLineLength = 0;
        char c;
        do {
            c = getc(pfFile);
            iLineLength++;
        } while (c != '\n' && c != EOF);

        fseek(pfFile, lineStart, SEEK_SET);

        char* line = (char*)calloc(iLineLength + 1, sizeof(char));
        int x = 0;
        do {
            c = getc(pfFile);
            if (c != EOF) {
                *(line + x) = c;
            }
            x++;
        } while (c != '\n' && c != EOF);

        lineStart = ftell(pfFile);


        *(ppcstrLines + l) = line;
        *(piLengths + l) = iLineLength;

    }

    FILEDATA* fd = calloc(1, sizeof(FILEDATA));
    fd->lengths = piLengths;
    fd->lines = ppcstrLines;
    fd->numLines = iNumLines;

    rewind(pfFile);
    return fd;
}

void freeFileData(FILEDATA* fd) {
    for (int i = 0; i < fd->numLines; i++) {
        free(*(fd->lines + i));
    }
    free(fd->lines);
    free(fd->lengths);
    free(fd);
}
