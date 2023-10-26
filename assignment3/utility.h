#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "linkedList.h"

typedef struct {
	int setmask;
	char output;
	int bitcount;
	int index;
} bitWriterStatus;

void bitWrite(char *code, bitWriterStatus *status,
		char *buffer, int lastchunk, int out);

void bitWriteHeader(char *code, int bits, FILE *out);

linkedList *buildTree(linkedList *list);

void printTree(huffNode *parent);

void genCodes(huffNode *parent, int len, char *code, char **codes, int size);

void freeTree(huffNode *parent);

void freeCodes(char **codes);
