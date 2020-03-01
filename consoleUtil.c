#include "ghettoWin.h"
#include <string.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "consoleUtil.h"

HANDLE hOut;

void setHandle(HANDLE h) {
    hOut = h;
}

void print(const char* str, ... ) {
    va_list args;
    va_start (args, str);
    char* buffer = calloc((strlen(str) + 40) * 2, sizeof(char));
    for (int i = 0; i < strlen(str); i++) {
        *(buffer + i) = *(str + i);
    }

    vsprintf(buffer, str, args);

    WriteConsole(hOut, buffer, strlen(buffer), 0, NULL);

    free(buffer);
}
