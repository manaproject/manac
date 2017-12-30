#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdarg.h>

#include "lexer.h"
#include "error.h"
#include "mana.h"


uint8_t syntaxError(uint8_t type, const char* file, uint64_t line, uint64_t pos){
    char fmsg[1024];
    sprintf(fmsg, "Syntax Error: file: '%s' line: %"PRIu64" pos: %"PRIu64" (%s).\n", file, line, pos, getErrorType(type));
    fprintf(stderr, fmsg);
    exit(type);
}

void customError(uint8_t type, const char* msg, ...)
{
    char temp[1024];
    va_list vl;
    va_start(vl, msg);
    vsprintf(temp, msg, vl);
    va_end(vl);
    fprintf(stderr, "Error: %s\n", temp);
    exit(type);
}

const char* getErrorType(unsigned int type)
{
    switch(type)
    {
    case MANA_MEMORY_ALLOCATION_ERROR:
        return "Memory allocation error";
    case MANA_INVALID_COMPILER_ARGS:
        return "Invalid compiler arguments";
    case MANA_INVALID_NUMBER:
        return "Invalid number token";
    case MANA_UNCLOSED_BRACKET:
        return "Unclosed bracket";
    case MANA_INVALID_END_OF_STRING:
        return "Invalid end of string";
    case MANA_UNABLE_TO_READ_FILE:
        return "Unable to read file";
    case MANA_UNEXPECTED_TOKEN:
        return "Unexpected token";
    }
    return "ERROR?";
}
