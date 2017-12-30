#ifndef _MANA_H_
#define _MANA_H_


/*
 * Define this macro to trace the compiler tasks, but the execution will take
 * much time longer, due to I/O on both screen and files, specially on Windows.
 */

#define NO_MANA_DEBUG

#ifdef MANA_DEBUG
	#ifdef _MSC_VER
		#define debugf  printf("\nFile \"%s\" Function \"%s\" Line %d: ", __FILE__, __FUNCTION__, __LINE__); debug
	#else
		#define debugf  printf("\nFile \"%s\" Function \"%s\" Line %d: ", __FILE__, __func__, __LINE__); debug
	#endif
#else
	#define debugf debug
#endif

typedef struct CompilerOptions {
	
};


void debug(const char * msg, ...);
#endif
