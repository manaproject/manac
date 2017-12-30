
#include <stdio.h>

#include "flag.h"
#include "utf8.h"
#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "dmt.h"
#include "unit_tests.h"

#define VERSION "v1.0.0"

int main(int argc, const char *argv[]) {
	/*
	LexInfo * li = dmt_calloc(1, sizeof(LexInfo));
	startLexicalAnalysis("sample.mana", li);
	parse(li, NULL);
	printf("done\n");
	//dmt_dump(stdout);
	return 0;
	*/

	runTests();
}
