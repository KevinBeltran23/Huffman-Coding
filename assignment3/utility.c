#include "utility.h"
#include<fcntl.h>
#include<unistd.h>
#define BITSET 49
#define BYTESIZE 8
#define BLOCK 256

void bitWrite(char *code, bitWriterStatus *status, 
                char *wbuffer, int lastchunk, int out){
    if(!code && lastchunk && (status -> bitcount % BYTESIZE != 0)){
        wbuffer[status -> index] = status -> output;
        return;
    }
    while(*code){
        if(*code == BITSET){
            status->output |= status->setmask;
        }
        status -> setmask >>= 1;
        status -> bitcount ++;
        if(status -> bitcount % BYTESIZE == 0){
            wbuffer[status -> index] = status -> output;
            status -> output = 0;
            status -> setmask = 1 << (BYTESIZE - 1);
            status -> index += 1;

            if(status -> index + 1 == sizeof(wbuffer)){
                write(out, wbuffer, status -> index);
                status -> index = 0;
                memset(wbuffer, 0, sizeof(wbuffer));
            }
        }  
        code++;
    }
    if(status -> setmask == (1 << (BYTESIZE -1))){
        status -> output = 0;
        status -> setmask = 1 << (BYTESIZE - 1);
        status -> bitcount = 0;
    }
}

/*frees array of huffman codes*/
void freeCodes(char **codes){
    int i;
    for(i = 0; i < BLOCK; i++){
        if(codes[i]){
            free(codes[i]);
        }
    }
    free(codes);
}
/*frees each node of huffman Tree*/
void freeTree(huffNode *parent){
    if(parent){  
        freeTree(parent -> left);
        freeTree(parent -> right);
        free(parent);
    }
}

/*displays huffTree in order traversal*/
void printTree(huffNode *parent){
    if(parent){
        printTree(parent -> left);
        if(!(parent -> left) && !(parent -> right)){
            printf("%d and %c\n", parent -> frequency, parent -> letter);
        }
        printTree(parent -> right);
    }
}

/*generates and prints huffcodes recursively in order*/
void genCodes(huffNode *parent,int length,char *code,char **codes,int size){
    if(parent){
        code[length] = '0';
        genCodes(parent -> left, length + 1, code, codes, size);
        if(!(parent -> left) && !(parent -> right)){
            char *temp;
            temp = NULL;
            code[length] = '\0';
            /*only puts code in array if its a leaf node*/
            if(parent -> letter != -1){
                if(!(temp = (char*)malloc((length + 1)*sizeof(char)))){
                    perror("malloc");
                    exit(EXIT_FAILURE);
                }
                codes[parent -> letter] = temp;
                strcpy(temp, code);
            }
        }
        code[length] = '1';
        genCodes(parent -> right, length + 1, code, codes, size);
    }
}

/*builds huffman Tree using a frequency list*/
linkedList *buildTree(linkedList *list){
    huffNode *parent;
    while (list -> head -> next){
        parent = createNode(-1, 
        list -> head -> frequency + list -> head -> next -> frequency);
        parent -> left = list -> head;
        parent -> right = list -> head -> next;

        removeNode(list);
        removeNode(list);

        insertNode(parent, list);
    }
    return list;
}
