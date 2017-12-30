#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#include "mana.h"

void debug(const char * msg, ...)
{
    char temp[1024];
    va_list vl;
    va_start(vl, msg);
    vsprintf(temp, msg, vl);
    va_end(vl);
    fprintf(stderr, temp);
    fprintf(stderr, "\n");
    FILE* debugFile = fopen("debug.txt", "a");
    fprintf(debugFile, temp);
    fprintf(debugFile, "\n");
    fclose(debugFile);
}
