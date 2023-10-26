#include "linkedList.h"

/*creates a node given the character and frequency*/
huffNode *createNode(int character, int freq){
    huffNode *newNode;

    if(!(newNode = (huffNode*)malloc(sizeof(huffNode)))){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    newNode -> right = NULL;
    newNode -> left = NULL;
    newNode -> next = NULL;
    newNode -> letter = character;
    newNode -> frequency = freq;
    
    return newNode;
}

/*Initializes an ordered list*/
linkedList *createList(){
    linkedList *newList;
    
    if(!(newList = (linkedList*)malloc(sizeof(linkedList)))){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    newList -> head = NULL;

    return newList;
}

/*inserts an existing node into the ordered list*/
void insert(int character, int freq, linkedList *list){
    huffNode *newNode, *currentNode;
    newNode = createNode(character, freq);

    /*list is empty or newNode goes before head*/
    if (list -> head == NULL
        || newNode -> frequency < list -> head -> frequency 
            || (newNode -> frequency == list -> head -> frequency 
                && newNode -> letter < list -> head -> letter)){
        newNode -> next = list -> head;
        list -> head = newNode;
        return;
    }

    /*dummy node to iterate through linkedList*/
    currentNode = list -> head;
    /*iterates until either small freq or same freq with smaller ascii*/
    while (currentNode -> next != NULL 
        && (newNode -> frequency > currentNode -> next -> frequency 
            || (newNode -> frequency == currentNode -> next -> frequency
                && newNode -> letter > currentNode -> next -> letter))){
        currentNode = currentNode -> next; 
    }
    newNode -> next = currentNode -> next;
    currentNode -> next = newNode;
}
/*removes references to head node and frees it*/
void freeNode(linkedList *list){
    if (list -> head == NULL){
        return;
    }
    huffNode *temp;
    temp = list -> head;
    list -> head  = list -> head -> next;
    free(temp);
}
/*inserts already malloc'd node*/
void insertNode(huffNode *newNode, linkedList *list){
    huffNode *currentNode;

    if(list -> head == NULL){
        list -> head = newNode;
        return;
    }
    if (newNode -> frequency <= list -> head -> frequency){
        newNode -> next = list -> head;
        list -> head = newNode;
        return;
    }
    /*dummy node to iterate through linkedList*/
    currentNode = list -> head;
    while (currentNode -> next != NULL 
    && newNode -> frequency > currentNode -> next -> frequency){
        currentNode = currentNode -> next;
    }
    newNode -> next = currentNode -> next;
    currentNode -> next = newNode;
}
/*removes references to head node, but does not free it*/
void removeNode(linkedList *list){
    huffNode *temp;
    if (list -> head == NULL){
        return;
    }
    if (list -> head -> next != NULL){
        temp = list -> head;
        list -> head = list -> head -> next;
        temp -> next = NULL;
        return;
    }
    list -> head = NULL;
}

/*frees all nodes in ordered list*/
void freeList(linkedList *list){
    if(list -> head == NULL){
        free(list);
        return;
    }
    huffNode *temp;
    
    /*frees nodes until next node is NULL*/
    while (list -> head -> next != NULL){
        temp = list -> head -> next;
        free(list -> head);
        list -> head = temp;
    }
    free(list);
    free(temp);
}



