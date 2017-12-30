#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dmt.h"

/* Get file length from a handle. */
long flengthh(FILE* file)
{
	long cur = ftell(file), len;
	fseek(file, 0, SEEK_END);
	len = ftell(file);
	fseek(file, cur, SEEK_SET);
	return len;
}

/* Get file length from a file name. */
long flength(const char* fname)
{
	FILE* file = fopen(fname, "rb");
	long flen = 0;
	if (file)
	{
		flen = flengthh(file);
		fclose(file);
	}
	return flen;
}

/* Read the file content into a buffer and return it. Return 0 if failed. */
char* readfile(const char* fname)
{
	char* buf;
	FILE* fp;
	unsigned int flen = flength(fname);
	if ((buf=dmt_calloc(flen+1, sizeof(char))) == NULL)
		return NULL;
	fp = fopen(fname, "rb");
	if (fp == NULL)
	{
		dmt_free(buf);
		return NULL;
	}
	fread(buf, sizeof(char), flen, fp);
	fclose(fp);
	buf[flen] = 0;
	return buf;
}
