#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ghettoWin.h"
#include "fileUtil.h"
#include "consoleUtil.h"

char** strsplit(const char* str, const char* split) {
    int numSplits = 0;
    int numCorrect = 0;

    int splitIndices[strlen(str)/strlen(split)]; // the indices just before the start of a split occurence

    for (int i = 0; i < strlen(str); i++) {
        char c = *(str + i);
        char compare = *(split + numCorrect);
        if (c == compare) {
            numCorrect++;
            if (numCorrect == strlen(split)) {
                numCorrect = 0;
                splitIndices[numSplits] = i - strlen(split);
                numSplits++;

            }
        } else {
            numCorrect = 0;
        }
    }
    numSplits++; // numSplits is 1 more than the number of occurences of split in "str"

    char** strings = calloc(numSplits + 1, sizeof(char*)); // the last string is 0;

    for (int i = 0; i < numSplits + 1; i++) {
        *(strings + i) = calloc(strlen(str), sizeof(char));
    }

    int indicesFound = 0;
    int currentStringLen = 0;
    for (int i = 0; i < strlen(str); ) {
        if (indicesFound > 0 && splitIndices[indicesFound] == i - 1 && splitIndices[indicesFound - 1] == i - 2 && 0) {
            *(*(strings + indicesFound) + currentStringLen) = 'b';
        } else {
            char c = *(str + i);
            *(*(strings + indicesFound) + currentStringLen) = c;
            currentStringLen++;
        }

        if (splitIndices[indicesFound] == i) {
            *(*(strings + indicesFound) + currentStringLen) = 0;
            i += strlen(split);
            currentStringLen = 0;
            indicesFound++;
            while(splitIndices[indicesFound] == i) {
                //*(*(strings + indicesFound) + currentStringLen) = 'b';
                *(*(strings + indicesFound) + currentStringLen) = 0;
                i += strlen(split);
                currentStringLen = 0;
                indicesFound++;
            }
            i++;
        } else {
            i++;
        }
    }

    *(strings + numSplits + 1) = 0;

    return strings;
}

FILE* getFile(const char* pcstrFileName) {
    FILE* fptrResult = fopen(pcstrFileName, "rb");

    return fptrResult;
}

void printFileData(FILEDATA* fd) {
    for (int i = 0; i < fd->numLines; i++) {
        print(*(fd->lines + i));
    }
}

int getFileLength(FILE* pfFile) { //returns the number of lines in the file
    int iNumLines = 1;
    char c = 0;
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

        char* line = (char*)calloc(iLineLength + 5, sizeof(char));
        int x = 0;
        do {
            c = getc(pfFile);
            if (c != EOF) {
                *(line + x) = c;
            }
            x++;
        } while (c != '\n' && c != EOF);

        lineStart = ftell(pfFile);

        //print(line);
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

char* lstrip(const char* src) {
    char* result = (char*)calloc(strlen(src) + 1, sizeof(char));
    int rLen = 0;

    char c = 0;
    int strip = 1;
    for (int i = 0; i < strlen(src); i++) {
        c = *(src + i);
        if (strip) {
            if (c != ' ' && c != '\n' && c != '\t') {
                strip = 0;
            }
        }

        if (!strip) {
            *(result + rLen) = c;
            rLen++;
        }
    }
    return result;
}

void freeSplits(char** splits) {
    for (char** str = splits; ; str++) {
        if (*str == 0) {
            free(*str);
            break;
        }
        free(*str);
    }
    free(splits);
}



MESH* getMeshData(const char* meshName) {
    FILE* file = getFile(meshName);
    if (file == NULL) {
        print("could not find file %s %i\n", meshName, errno);
    }
    FILEDATA* fd = getFileData(file);
    MESH* mesh = calloc(1, sizeof(MESH));
    for (int i = 0; i < fd->numLines; i++) {
        char* newline = lstrip(*(fd->lines + i));
        free(*(fd->lines + i));
        *(fd->lines + i) = newline;
    }

    int numVerts = 0;
    int numNormals = 0;
    int numTextures = 0;
    int numFaces = 0;

    for (int i = 0; i < fd->numLines ; i++) {
        char* line = *(fd->lines + i);
        if (strlen(line) > 2) {
            char** splits = strsplit(line, " ");
            if (strcmp(*(splits), "vt") == 0) {
                numTextures++;
            } else if(strcmp(*(splits), "vn") == 0) {
                numNormals++;
            } else if(strcmp(*(splits), "vp") == 0) {

            } else if (strcmp(*(splits), "v") == 0) {
                numVerts++;
            } else if (strcmp(*(splits), "f") == 0) {
                numFaces++;
            }
            freeSplits(splits);
        }
    }



    //print("%i, %i, %i, %i\n", numTextures, numNormals, numVerts, numFaces);

    mesh->faces = calloc(numFaces * 3, sizeof(int)); // 3 is number of verts per face (3 vec3s)
    mesh->verts = calloc(numVerts, sizeof(vec3));
    mesh->texCoords = calloc(numTextures, sizeof(vec2));
    mesh->normals = calloc(numNormals, sizeof(vec3));
    mesh->numVerts = numVerts;
    mesh->numFaces = numFaces;
    mesh->normalsOrdered = calloc(numFaces * 3 * 2, sizeof(vec3));
    mesh->vertsOrdered = calloc(numFaces * 3 * 2, sizeof(vec3));

    int vertIndex = 0, normalIndex = 0, textureIndex = 0, faceIndex = 0;
    int orderIndex = 0;
    for (int i = 0; i < fd->numLines; i++) {
        char* line = *(fd->lines + i);
        if (strlen(line) > 2) {
            char** splits = strsplit(line, " ");
            if (strcmp(*(splits), "vt") == 0) {

            } else if(strcmp(*(splits), "vn") == 0) {
                float x = strtof(*(splits + 1), NULL);
                float y = strtof(*(splits + 2), NULL);
                float z = strtof(*(splits + 3), NULL);
                *(mesh->normals + normalIndex) = (vec3){x, y, z};
                normalIndex++;
            } else if(strcmp(*(splits), "vp") == 0) {

            } else if (strcmp(*(splits), "v") == 0) {
                float x = strtof(*(splits + 1), NULL);
                float y = strtof(*(splits + 2), NULL);
                float z = strtof(*(splits + 3), NULL);
                *(mesh->verts + vertIndex) = (vec3){x, y, z};
                vertIndex++;
            } else if (strcmp(*(splits), "f") == 0) {
                char** first = strsplit(*(splits + 1), "/");
                int a = atoi(*first);
                int na = atoi(*(first + 2));
                freeSplits(first);

                char** second = strsplit(*(splits + 2), "/");
                int b = atoi(*second);
                int nb = atoi(*(second + 2));
                freeSplits(second);

                char** third = strsplit(*(splits + 3), "/");
                int c = atoi(*third);
                int nc = atoi(*(third + 2));
                freeSplits(third);

                char* fourthS = *(splits + 4);
                if (fourthS != 0) {
                    char** fourth = strsplit(fourthS, "/");
                    int d = atoi(*fourth);
                    int nd = atoi(*(fourth + 2));
                    freeSplits(fourth);

                    *(mesh->vertsOrdered + orderIndex) = *(mesh->verts + a - 1);
                    if (nb != 0) {
                        *(mesh->normalsOrdered + orderIndex) = *(mesh->normals + na - 1);
                    }
                    orderIndex++;

                    *(mesh->vertsOrdered + orderIndex) = *(mesh->verts + c - 1);
                    if (nc != 0) {
                        *(mesh->normalsOrdered + orderIndex) = *(mesh->normals + nc - 1);
                    }
                    orderIndex++;

                    *(mesh->vertsOrdered + orderIndex) = *(mesh->verts + d - 1);
                    if (nd != 0) {
                        *(mesh->normalsOrdered + orderIndex) = *(mesh->normals + nd - 1);
                    }
                    mesh->numFaces++;
                    orderIndex++;
                }


                *(mesh->vertsOrdered + orderIndex) = *(mesh->verts + a - 1);
                if (na != 0) {
                    *(mesh->normalsOrdered + orderIndex) = *(mesh->normals + na - 1);
                }
                orderIndex++;

                *(mesh->vertsOrdered + orderIndex) = *(mesh->verts + b - 1);
                if (nb != 0) {
                    *(mesh->normalsOrdered + orderIndex) = *(mesh->normals + nb - 1);
                }
                orderIndex++;

                *(mesh->vertsOrdered + orderIndex) = *(mesh->verts + c - 1);
                if (nc != 0) {
                    *(mesh->normalsOrdered + orderIndex) = *(mesh->normals + nc - 1);
                }
                orderIndex++;
            }

            freeSplits(splits);
        }
    }


    //print("o: %i\n", orderIndex);

    return mesh;
}










