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
    uint8_t length1 = 5;
    char* gtPath1[] = {"org", "praisethemoon", "mana", "system", "io"};
    char* gtName1 = "stdio";

    uint8_t length2 = 3;
    char* gtPath2[] = {"org", "praisethemoon", "java"};
    char* gtName2 = "jre";

    uint8_t length3 = 2;
    char* gtPath3[] = {"std", "system"};
    char* gtName3 = "yes";

    // preparing test
	LexInfo * li = dmt_calloc(1, sizeof(LexInfo));
	startLexicalAnalysis("../tests/test_02_import.mana", li);
	
	AST* ast = dmt_calloc(1, sizeof(AST));
	ast->type = AST_PACKAGE;
    ast->package = dmt_calloc(1, sizeof(Package));
    vec_init(&ast->package->imports);

    parseImportBlock(li, ast);

    mu_check(ast->package->imports.length == 3);

    uint16_t iterator = 0;
    for(; iterator < length1; iterator++){
        mu_check(strcmp(ast->package->imports.data[0]->path.data[iterator], gtPath1[iterator]) == 0);
    }

    mu_check(strcmp(ast->package->imports.data[0]->name, gtName1) == 0);


    for(iterator = 0; iterator < length2; iterator++){
        mu_check(strcmp(ast->package->imports.data[1]->path.data[iterator], gtPath2[iterator]) == 0);
    }

    mu_check(strcmp(ast->package->imports.data[1]->name, gtName2) == 0);


    for(iterator = 0; iterator < length3; iterator++){
        mu_check(strcmp(ast->package->imports.data[2]->path.data[iterator], gtPath3[iterator]) == 0);
    }

    mu_check(strcmp(ast->package->imports.data[2]->name, gtName3) == 0);
}

MU_TEST_SUITE(test_suite) {
	MU_RUN_TEST(test_namespace);
	MU_RUN_TEST(test_import);
}

void runTests(){
    MU_RUN_SUITE(test_suite);
    MU_REPORT();
}