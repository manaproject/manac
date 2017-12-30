#ifndef _MANA_PARSER_H_
#define _MANA_PARSER_H_

#include <stdlib.h>
#include <stdint.h>
#include "vec.h"

typedef enum ASTType{
	AST_PACKAGE = 0, 
}ASTType;

/*
 * Namespace data structyre
 * reprents any abstract variables, imports, etc
 */
typedef struct Namespace {
	vec_t(char*) path;
	char* name;
}Namespace;

#define INIT_NAMESPACE(ns)\
ns = dmt_calloc(1, sizeof(Namespace));\
vec_init(&ns->path);

/*
 * Pakcage, file root
 */ 
typedef struct Package {
	vec_t(Namespace*) imports;
	// TODO: add more here
} Package;


typedef struct AST{
	union {
		Package* package;
	};

	ASTType type;

	uint64_t pos;
	uint64_t line;
} AST;


typedef struct AnnotationAST {
	AST* node;
	char* name;
}AnnotationAST;

#define PARSE_FUNC(name) void name(LexInfo* li, AST* ast)

void parse();

PARSE_FUNC(parsePackage);
PARSE_FUNC(parseAnnotationDecl);
PARSE_FUNC(parseModuleDecl);
PARSE_FUNC(parseTypeDecl);
PARSE_FUNC(parseFunctionDecl);

Namespace* parseNamespace(LexInfo* li, AST* ast);

#undef PARSE_PARAMS
#endif
