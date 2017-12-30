#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stddef.h>

#include "mana.h"
#include "lexer.h"
#include "parser.h"
#include "error.h"
#include "vec.h"
#include "dmt.h"

Token* currentToken(LexInfo *li)
{
	Token* t= li->tokens.data[li->pos];
	return t;
}

Token* nextToken(LexInfo *li)
{
	if(li->pos >= li->tokens.length)
		return NULL;
	Token* t = li->tokens.data[li->pos+1];
	return t;
}

void step(LexInfo *li)
{
	li->pos++;
}

/*
 * TODO: token to string for better errors
 */
void match(LexInfo * li, LexemType type)
{
	if(li->pos >= li->tokens.length)
		customError(MANA_UNEXPECTED_TOKEN, "Syntax error: file: '%s', line %d:%d, expected '%s' before EOF.", li->src, li->line, currentToken(li)->pos, "");//getTokenStr(type));

	if (currentToken(li)->lt != type)
		customError(MANA_UNEXPECTED_TOKEN, "Syntax error: file: '%s', line %d:%d, '%s' expected but %s was found.", li->src, li->line, currentToken(li)->pos, "", "");//getTokenStr(type), getTokenStr(currentToken(li)->lt));
}


/*
 * Recursive parse functions
 * Starting from simplest
 */


/* 
 * @Tested
 * <namespace> ::= ID '.' <namespace>
 *               | ID
 */
Namespace* parseNamespace(LexInfo* li, AST* ast){
	Namespace* ns = NULL;
	INIT_NAMESPACE(ns);

	int keep = 1;
	char* name = NULL;

	while(keep){
		name = currentToken(li)->buf;
		match(li, IDENTIFIER);
		step(li);

		if(currentToken(li)->lt == '.')
		{
			vec_push(&ns->path, name);
			step(li);
		}
		else{
			ns->name = name;
			keep = 0;
		}
	}

	return ns;
}


/*
 * <import_statement> ::= ID '.' <import_statement>
 *                      | ID '{' <import_sub_elements> '}'
 *                      | ID
 * 
 */

void parseImportStatement(LexInfo* li, AST* ast){
	printf("parsing import statement\n");
	match(li, IDENTIFIER);

	Namespace* ins = NULL;
	ins = parseNamespace(li, ast);
	if(currentToken(li)->lt == '{'){
		step(li);
		char keep = 1;

		while(keep){
			Namespace* subins = parseNamespace(li, ast);
			
			// Concatenate ins and all its sub-imports to obtain full import paths
			Namespace* dest = NULL;
			INIT_NAMESPACE(dest);

			uint16_t iterator = 0;
			// iterate over the root
			for(;iterator < ins->path.length; iterator++){
				vec_push(&dest->path, ins->path.data[iterator]);
			}
			// push the name as well
			vec_push(&dest->path, ins->name);

			// iterate over the sub imports
			for(iterator = 0;iterator < subins->path.length; iterator++){
				vec_push(&dest->path, subins->path.data[iterator]);
			}

			// subimport namespace's name is the final name used.
			dest->name = subins->name;

			for(iterator = 0; iterator<dest->path.length; iterator++){
				printf("%s, ", dest->path.data[iterator]);
			}
			printf("%s\n", dest->name);

			if(currentToken(li)->lt == ','){
				step(li);
			}
			else{
				keep = 0;
			}
		}
	}
}


/*
 * <import_block> ::= import '(' <import_statement>* ')'
 */ 
void parseImportBlock(LexInfo* li, AST* ast){
	match(li, IMPORT);
	step(li);
	match(li, '(');
	step(li);

	if(currentToken(li)->lt == ')'){
		// TODO
		step(li);
		return;
	}
	else {
		parseImportStatement(li, ast);
	}
}

/*
 * <basic_type> ::= <namespace>
 *                | <namespace> '<' <type_list> '>'
 */

void parseBasicType(LexInfo* li, AST* ast){
	match(li, IDENTIFIER);
	Namespace* ns = parseNamespace(li, ast);
	step(li);
	if(currentToken(li)->lt == '<'){
		// TODO
	}
	else{
		// TODO
	}
}

/*
 * <type> ::= <basic_type> ('[' ']')*
 *          |
 */
void parseType(LexInfo* li, AST* ast){
	match(li, IDENTIFIER);
	
}

/*
 * annotation ::= '@' IDENTIFIER
 *              | '@' IDENTIFIER '(' <properties> ')'
 */ 
void parseAnnotations(LexInfo* li, AST* ast){
	match(li, '@');
	step(li);
	match(li, IDENTIFIER);
	printf("Found annotation @%s\n", currentToken(li)->buf);
}

void parsePackage(LexInfo* li, AST* ast){
	// Check if we have annotations for this package
	if(currentToken(li)->lt == '@'){
		parseAnnotations(li, ast);
	}
	else{
		match(li, IMPORT);
	}
}

void parse(LexInfo* li, AST* ast) {
	
	//parsePackage(li, ast);
	parseImportBlock(li, ast);
}

void prepareParsing(LexInfo* li){
	AST* ast = dmt_calloc(1, sizeof(AST));
	ast->type = AST_PACKAGE;
}