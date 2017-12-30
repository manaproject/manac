/*
 * Mana Programming Language
 * Lexer and Parser Unit tests
 * (c) Soulaymen Chouri 2017 - 2018, check LICENSE.md for license information
 */ 

#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "utils.h"
#include "lexer.h"
#include "parser.h"
#include "dmt.h"
#include "minunit.h"


MU_TEST(test_namespace) {
    // ground truth
    uint8_t length = 3;
    char* gtPath[] = {"org", "praisethemoon", "mana"};
    char* gtName = "stdio";

    // preparing test
	LexInfo * li = dmt_calloc(1, sizeof(LexInfo));
	startLexicalAnalysis("../tests/test_01_namespace.mana", li);
	Namespace* ns = parseNamespace(li, NULL);

    uint16_t iterator = 0;
    for(; iterator < length; iterator++){
        mu_check(strcmp(ns->path.data[iterator], gtPath[iterator]) == 0);
    }

    mu_check(strcmp(ns->name, gtName) == 0);
}

MU_TEST(test_import) {
    // ground truth
    uint8_t length = 3;
    char* gtPath[] = {"org", "praisethemoon", "mana"};
    char* gtName = "stdio";

    // preparing test
	LexInfo * li = dmt_calloc(1, sizeof(LexInfo));
	startLexicalAnalysis("../tests/test_01_namespace.mana", li);
	Namespace* ns = parseNamespace(li, NULL);

    uint16_t iterator = 0;
    for(; iterator < length; iterator++){
        mu_check(strcmp(ns->path.data[iterator], gtPath[iterator]) == 0);
    }

    mu_check(strcmp(ns->name, gtName) == 0);
}

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_namespace);
}

void runTests(){
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
}