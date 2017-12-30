#ifndef _MANA_LEXER_H_
#define _MANA_LEXER_H_

#include <stdint.h>
#include <stddef.h>

#include "vec.h"

typedef enum
{
	/* Reserved words. */
	LET=256, MUT, FN, 
	CLASS, INTERFACE, IMPLEMENTS, LOCAL, IMMUTABLE,
	ENUM, RETURN, USE, EXTERN, SYNC,
	MODULE, IMPORT,
	IF, ELSE, MATCH,
	FOR, FOREACH, WHILE, REPEAT, BREAK, CONTINUE, CONST,
	// Types
	U8, I8, U16, I16, U32, S32, U64, S64, F32, F64, TK_BOOL, CHAR, TK_BYTE,
	// Values
	STRING, FSTRING, INTEGER, TK_FLOAT, /*HEX, OCT,*/
	IN, AS, TRUE, FALSE, ANNOTATION,
	
	TYPE,
	
	IDENTIFIER, LAMBDA_ARROW, RET_TYPE_ARROW, NAMESPACE_COLON,
	
	// Operators
	COMMENT, SPACE, DOUBLEDOT,
	TRIPLEDOT, INC, DEC, PREINC, POSTINC, PREDEC, POSTDEC, PASS, MASS, MULASS,
	DASS, MODASS, LSASS, RSASS, BAASS, BOASS, BXASS, LEQ, GEQ, EQUAL, NOTEQUAL,
	LOGAND, LOGOR, TCONSTRUCT, LSHIFT, RSHIFT, ENDOFFILE, 
} LexemType;

typedef struct Token {
	unsigned int lt;
	char* buf;
	uint64_t buf_len;
	uint64_t bracketLevel;
	uint64_t pos;
	uint64_t line;
	char* src;
}Token;

typedef struct KeywordToken{
	int token;
	char* str;
}KeywordToken;


#define KEYWORDS_COUNT sizeof(keywords)/sizeof(KeywordToken)

static const KeywordToken keywords[] =
{
	/* keywords */
	{LET, "let"},
	{MUT, "mut"},
	{FN, "fn"}, 
	{CLASS, "class"},
	{INTERFACE, "interface"},
	{IMPLEMENTS, "implements"},
	{LOCAL, "local"},
	{IMMUTABLE, "imm"},
	{ENUM, "enum"}, 
	{TYPE, "type"},
	{RETURN, "return"}, 
	{USE, "use"},
	{MODULE, "module"},
	{IMPORT, "import"},
	{IF, "if"},
	{ELSE, "else"},
	{MATCH, "match"},
	{FOR, "for"},
	{FOREACH, "foreach"},
	{WHILE, "while"},
	{REPEAT, "repeat"}, 
	{BREAK, "break"},
	{CONTINUE, "continue"},
	{U8, "u8"},
	{I8, "i8"},
	{U16, "u16"},
	{I16, "i16"},
	{U32, "u32"},
	{S32, "s32"},
	{U64, "u64"},
	{S64, "s64"},
	{F32, "f32"},
	{F64, "f64"},
	{TK_BOOL, "bool"}, 
	{CHAR, "char"},
	{TK_BYTE, "byte"},
	{TRUE, "true"},
	{FALSE, "false"},
	{IN, "in"}, 
	{AS, "as"},
	{EXTERN, "extern"},
	{SYNC, "sync"},
	{CONST, "const"},
	{ANNOTATION, "annotation"}
};

typedef struct LexInfo
{
	const char * src;
	const char * content;
	uint64_t bufferPos;
	uint64_t pos, line;
	/* Parser Counter: used by the parser */
	uint64_t pc;
	/* Bracket Stack used for brackets matching */
	vec_t(Token*) bracketStack;
	/* list of generated tokens */
	vec_t(Token*) tokens; 
} LexInfo;

/* Lexing various tokens */
startLexicalAnalysis(const char* src, LexInfo *li);
Token * getNextToken(LexInfo *);
Token * lexNumber(LexInfo *);
Token * lexId(LexInfo *);
Token * lexComment(LexInfo *);
Token * lexNewLine(LexInfo *);
Token * lexAdd(LexInfo *);
Token * lexSub(LexInfo *);
Token * lexMult(LexInfo *);
Token * lexDiv(LexInfo *);
Token * lexEqu(LexInfo *);
Token * lexNot(LexInfo *);
Token * lexGreater(LexInfo *);
Token * lexSmaller(LexInfo *);
Token * lexDot(LexInfo *);
Token * lexDoubleColon(LexInfo *);
Token * lexBracket(LexInfo *);
Token * lexRandom(LexInfo *);
Token * lexQString(LexInfo *, char);
Token * initToken(LexInfo *);

void finishLexingTask(LexInfo *);

/* utilities */
uint8_t isIdChar(char c, uint8_t acceptDigit);
uint8_t isDigit(char);
uint8_t isBracket(char);
uint8_t isOpenBracket(char);
uint8_t isCloseBracket(char);
char getClosedBracket(char);
uint8_t isNewLine(char);
uint8_t isUnsupportedChar(char);
void checkTokenKeyword(Token *);
void freeChar(char *);
void prepareLexTable(LexInfo * li);
void checkString(char *);

#endif
