
#include <ctype.h>
#include <stdlib.h>

#include "mana.h"
#include "lexer.h"
#include "utf8.h"
#include "vec.h"
#include "dmt.h"
#include "error.h"
#include "utils.h"


/* Begin Lexical analysis process. */
startLexicalAnalysis(const char * src, LexInfo * li){
	li->src = src;

	/* Might do some check over file existance here */
	li->content = readfile(src);
	if(!li->content){
		fprintf(stderr, "Error! cannot open source file\n");
		exit(-1);
	}
	li->bufferPos = 0;
	li->pos = 0;
	li->line = 0;
	
	vec_init(&li->tokens);
	vec_init(&li->bracketStack);

	uint64_t fileSize = strlen(li->content);
	uint64_t i = 0;


	while(li->bufferPos < fileSize)
	{
		Token * t = getNextToken(li);
		if (t != NULL){
			t->bracketLevel = li->bracketStack.length;
			if(t->lt == '(' || t->lt == '[' || t->lt == '{' || t->lt == ')' || t->lt == ']' || t->lt == '}')
				t->bracketLevel--;
			debugf("%d\t%d\t%d\t|\"%s\"", i, t->line, t->pos, t->buf);
			vec_push(&li->tokens, t);
			i++;
		}
	}

	finishLexingTask(li);
}

Token * getNextToken(LexInfo * li){
	//debugf("Reading next token");
	char* content = li->content;
	uint64_t* bp = &li->bufferPos; // Position in the file

	if(content[*bp] == ' '){
		//debugf("space");
		*bp = *bp + 1;
		li->pos ++;
		return NULL;
	}

	if(isUnsupportedChar(content[*bp])){
		*bp = *bp + 1;
		return NULL;
	}
	if (isNewLine(content[*bp]))
		return lexNewLine(li);
	if(isDigit(content[*bp]))
		return lexNumber(li);
	else if(isIdChar(content[*bp], 0))
		return lexId(li);
	else if (content[*bp] == '#')
		return lexComment(li);
	else if (content[*bp] == '+')
		return lexAdd(li);
	else if (content[*bp] == '-')
		return lexSub(li);
	else if (content[*bp] == '*')
		return lexMult(li);
	else if (content[*bp] == '/')
		return lexDiv(li);
	else if (content[*bp] == '=')
		return lexEqu(li);
	else if (content[*bp] == '!')
		return lexNot(li);
	else if (content[*bp] == '>')
		return lexGreater(li);
	else if (content[*bp] == '<')
		return lexSmaller(li);
	else if (content[*bp] == '.')
		return lexDot(li);
	else if (content[*bp] == ':')
		return lexDoubleColon(li);
	else if (content[*bp] == '"')
		return lexQString(li, '"');
	else if (content[*bp] == '\'')
		return lexQString(li, '\'');
	else if (isBracket(content[*bp]))
		return lexBracket(li);
	else
		return lexRandom(li);
}


/* Number lex */
Token * lexNumber(LexInfo * li){
	//debugf("Lexing number");
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	uint64_t strSize = 1;


	Token * t = initToken(li);

	t->lt = INTEGER;

	while(isDigit(content[*bp])){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = content[*bp];
		strSize++;
		*bp = *bp + 1;
		li->pos++;
	}

	if (content[*bp] == '.'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = content[*bp];
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		while(isDigit(content[*bp])){
			t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
			t->buf[strSize-1] = content[*bp];
			strSize++;
			*bp = *bp + 1;
			li->pos++;
		}

		/* change it to float */
		t->lt = TK_FLOAT;

		if (content[*bp] == '.'){
			/* Free Resources */
			finishLexingTask(li);
			syntaxError(MANA_INVALID_NUMBER, li->src, li->pos, li->line);
		}
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';
	return t;
}

/* Identifier Lex */
Token * lexId(LexInfo * li){
	//debugf("Lexing identifier");

	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	uint64_t strSize = 1;

	Token * t = initToken(li);

	while(isIdChar(content[*bp], 0)){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = content[*bp];
		strSize++;
		*bp = *bp + 1;
		li->pos++;
	}

	if (isDigit(content[*bp])){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = content[*bp];
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		while(isIdChar(content[*bp], 1)){
			t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
			t->buf[strSize-1] = content[*bp];
			strSize++;
			*bp = *bp + 1;
			li->pos++;
		}
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';
	t->lt = IDENTIFIER;

	checkTokenKeyword(t);
	return t;
}


Token * lexAdd(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '+';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '+';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '+'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '+';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = INC;
	}
	else if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = PASS;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexNewLine(LexInfo * li){
	//debugf("new line");

	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	/* http://stackoverflow.com/questions/1761051/difference-between-n-and-r */
	/* \r: 0x0d, \n: 0x0a */
	//debugf("current: %02x", content[*bp]);


	/* if we are on windows we need to skin two chars */

	if(content[*bp]=='\r')
		if (content[*bp+1]=='\n')
		{
			*bp = *bp + 1;
		}

	li->line++;
	*bp = *bp + 1;
	li->pos = 0;

	return NULL;
}

Token * lexSub(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '-';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '-';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '-'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '-';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = DEC;
	}
	else if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = MASS;
	}
	else if (content[*bp] == '>'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '>';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = RET_TYPE_ARROW;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';
	return t;
}

Token * lexMult(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '*';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '*';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = MULASS;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexDiv(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '/';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '/';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	// multiline comment
	// TODO: CHECK EOF
	if(content[*bp] == '*'){
		dmt_free(t->buf);
		dmt_free(t);
		
		do {
			*bp = *bp + 1;
			li->pos++;
			
			if (isNewLine(content[*bp]))
				lexNewLine(li);

		} while(!((content[*bp-1] == '*') && (content[*bp] == '/' )));
		li->pos++;
		*bp += 1;
		return NULL;
	}
	// single line comment
	// TODO: check EOF
	if(content[*bp] == '/'){
		dmt_free(t->buf);
		dmt_free(t);
		
		do {
			*bp = *bp + 1;
			li->pos++;
		} while(content[*bp-1] != '\n');
		return NULL;
	}
	
	
	if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = DASS;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexEqu(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '=';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '=';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = EQUAL;
	} else if (content[*bp] == '>') {
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '>';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = LAMBDA_ARROW;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexNot(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '!';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '!';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = NOTEQUAL;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexGreater(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '>';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '>';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = GEQ;
	}else if (content[*bp] == '>'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '>';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = RSHIFT;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';
	return t;
}

Token * lexSmaller(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '<';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '<';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '='){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '=';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = LEQ;
	}else if (content[*bp] == '<'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '<';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = LSHIFT;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';
	return t;
}

Token * lexDot(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = '.';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '.';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == '.'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = '.';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = DOUBLEDOT;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexDoubleColon(LexInfo * li)
{
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = ':';

	uint64_t strSize = 1;

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = ':';
	strSize++;
	*bp = *bp + 1;
	li->pos++;

	if (content[*bp] == ':'){
		t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
		t->buf[strSize-1] = ':';
		strSize++;
		*bp = *bp + 1;
		li->pos++;
		t->lt = NAMESPACE_COLON;
	}

	t->buf = dmt_realloc(t->buf, (strSize)*sizeof(char));
	t->buf[strSize-1] = '\0';

	return t;
}

Token * lexComment(LexInfo * li){
	//debugf("Lexing comment");

	char * content = li->content;
	uint64_t * bp = &li->bufferPos;
	uint64_t fileSize = utf8len(li->content);

	while (!isNewLine(content[*bp]) && *bp < fileSize){
		*bp = *bp + 1;

		/* Need to increment position because there will be
		   a newline after comment */
		li->pos++;
	}

	return NULL;
}

Token * lexBracket(LexInfo * li){
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	char c = content[*bp];

	t->lt = c;

	t->buf = dmt_realloc(t->buf, (2)*sizeof(char));
	t->buf[0] = c;
	t->buf[1] = '\0';

	if(isOpenBracket(c)){
		vec_push(&li->bracketStack, t);
	}

	else{
		Token * x = vec_pop(&li->bracketStack);
		if (getClosedBracket(x->lt) != c){
			/* Clear Resources */
			finishLexingTask(li);
			syntaxError(MANA_UNCLOSED_BRACKET, li->src, x->line, x->pos);
		}
	}

	*bp = *bp + 1;
	li->pos++;

	return t;
}

Token * lexRandom(LexInfo * li){
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);

	t->lt = content[*bp];

	t->buf = dmt_realloc(t->buf, (2)*sizeof(char));
	t->buf[0] = content[*bp];
	t->buf[1] = '\0';
	*bp = *bp + 1;
	li->pos++;

	return t;
}

Token * lexQString(LexInfo * li, char key){
	char * content = li->content;
	uint64_t * bp = &li->bufferPos;

	Token * t = initToken(li);
	uint64_t fileSize = utf8len(content);
	uint64_t size = 1;

	uint64_t keepLooping = 1;

	if (key == '"')
		t->lt = FSTRING;
	else
		t->lt = STRING;

	t->buf = dmt_realloc(t->buf, size*sizeof(char));
	t->buf[size-1] = key;
	size++;
	*bp = *bp + 1;
	li->pos++;


	while(keepLooping){
		while(content[*bp] != key && (*bp < fileSize)){
			t->buf = dmt_realloc(t->buf, size*sizeof(char));
			t->buf[size-1] = content[*bp];

			if (isNewLine(content[*bp])){
				li->line++;
				li->pos = 0;
			}

			*bp = *bp + 1;
			li->pos++;
			size++;
		}


		if (*bp > fileSize){
			// Free Resources
			finishLexingTask(li);
			syntaxError(MANA_INVALID_END_OF_STRING, li->src, li->line, li->pos);
		}


		t->buf = dmt_realloc(t->buf, size*sizeof(char));
		t->buf[size-1] = key;

		if (content[*bp-1] != '\\')
			keepLooping = 0;
		else
			if(content[*bp-2] != NULL)
				if(content[*bp-2] != '\\')
					keepLooping = 0;
				keepLooping = 0;


		*bp = *bp + 1;
		li->pos++;
		size++;

	}

	t->buf = dmt_realloc(t->buf, (size)*sizeof(char));
	t->buf[size-1] = '\0';

	return t;
}

uint8_t isIdChar(char c, uint8_t acceptDigit){
	if (acceptDigit)
		return isalnum(c) || c == '_' || c == '$' ;
	return isalpha(c) || c == '_' || c == '$' ;
}

uint8_t isDigit(char c){
	if ((c >='0') && (c<='9'))
		return 1;
	return 0;
}

uint8_t isOctDigit(char c){
	if ((c >='0') && (c<='7'))
		return 1;
	return 0;
}

uint8_t isHexDigit(char c){
	if (((c >='0') && (c<='9')) || ((c >='a') && (c<='f')) || ((c >='A') && (c<='F')))
		return 1;
	return 0;
}

uint8_t isBracket(char c){
	return c == '(' || c == ')' || c == '[' || c == ']' || c == '{' || c == '}';
}

uint8_t isOpenBracket(char c){
	return c == '(' || c == '[' || c == '{';
}

uint8_t isCloseBracket(char c){
	return c == ')' || c == ']' || c == '}';
}

char getClosedBracket(char c){
	switch(c){
		case '(':
			return ')';
		case '[':
			return ']';
		case '{':
			return '}';
		default:
			debugf("Warning, non bracket value passed to getClosedBracket");
			return 0;
	}
}

uint8_t isNewLine(char c){
	return c == '\n' ;
}

uint8_t isUnsupportedChar(char c){
	return c == '\r' || c == '\t';
}

/* Initialises and returns a token or lexem */
Token * initToken(LexInfo * li){
	Token * t = dmt_calloc(1, sizeof(Token));
	t->pos = li->pos;
	t->line = li->line;
	t->src = li->src;
	t->buf = NULL;
	t->buf_len = 0;
	t->bracketLevel = 0;
	t->lt = 0;

	return t;
}

void checkTokenKeyword(Token * t){
	uint64_t i = 0;
	for(; i < KEYWORDS_COUNT; i++)
		if (utf8cmp(t->buf, keywords[i].str) == 0){
			t->lt = keywords[i].token;
			return;
		}
}

void freeChar(char *c)
{
	free(c);
}

void finishLexingTask(LexInfo *li){
	unsigned int i = 0;
	for(i = 0; i < li->tokens.length; i++){
		Token *t = li->tokens.data[i];
		debugf("%d\t%d\t%d\t|\"%s\"", i, t->line, t->pos, t->buf);
	}


	if (li->bracketStack.length!= 0){
		Token * t = vec_pop(&li->bracketStack);
		syntaxError(MANA_UNCLOSED_BRACKET, li->src, t->line, t->pos);
	}
	li->pos = 0;
	vec_deinit(&li->bracketStack);
	dmt_free(li->content);
}

// TODO: validate string escape characters
void checkString(char * str){

}

// TODO: validate string escape characters and format
void checkFString(char* str) {
	
}

uint64_t isKeyword(LexemType lt){
	uint64_t i = 0;
	for(; i < KEYWORDS_COUNT; i++)
		if (lt == keywords[i].token){
			return 1;
		}
	return 0;
}
