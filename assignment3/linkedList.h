#include<stdio.h>
#include<stdlib.h>
#include<string.h>

typedef struct huffNode{
	int frequency;
	int letter;
	struct huffNode *right;
	struct huffNode *left;
	struct huffNode *next;
}huffNode;

typedef struct linkedList{
	struct huffNode *head;
}linkedList;

linkedList *createList();

huffNode *createNode(int letter, int freq);

void insert(int character, int freq, linkedList *list);

void insertNode(huffNode *newNode, linkedList *list);

void removeNode(linkedList *list);

void freeNode(linkedList *list);

void freeList(linkedList *list);



