#include <stdio.h>
#include <mana.h>
#include <lexer.h>

int main(int argc, char *argv[])
{
	printf("mana programming language v%s\n", MANA_VERSION);
	startLex();
	return 0;
}