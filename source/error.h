#ifndef _MANA_ERROR_H_
#define _MANA_ERROR_H_

#include <stdint.h>

#include "mana.h"

uint8_t syntaxError(uint8_t type, const char* file, uint64_t line, uint64_t pos);
void customError(uint8_t type, const char* msg, ...);

const char* getErrorType(unsigned int type);

#define MANA_MEMORY_ALLOCATION_ERROR 0
#define MANA_INVALID_COMPILER_ARGS   1
#define MANA_INVALID_NUMBER          2
#define MANA_UNCLOSED_BRACKET        3
#define MANA_INVALID_END_OF_STRING   4
#define MANA_UNABLE_TO_READ_FILE     5
#define MANA_UNEXPECTED_TOKEN        6

#endif
