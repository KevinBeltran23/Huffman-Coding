#include "utility.h"
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<arpa/inet.h>
    
#define BLOCK 256
#define CHUNK 4096
#define BYTESIZE 8

int main(int argc, char *argv[]){
    int in = 0;
    int out = 1;
    char rbuffer[CHUNK];
    char wbuffer[CHUNK];

    unsigned int *freqs = NULL;
    char *code = NULL;
    char **codes = NULL;
    linkedList *list = NULL;
    huffNode *dummy = NULL;

    size_t bytes_read, total_bytes, totalsize, currentsize;
    off_t file_size;
    struct stat st;

    unsigned int header_int, i;
    unsigned char symbols;
    
    /*manual parsing*/
    if(argc >= 2){
        if(access(argv[1], F_OK) == -1){
            perror("access");
            exit(EXIT_FAILURE);
        }
        if(!(in = open(argv[1], O_RDONLY))){
            perror("open");
            exit(EXIT_FAILURE);
        }
        if (argc >= 3){
            if(!(out = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 
                        S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH))){
                perror("open");
                exit(EXIT_FAILURE);
            }
        }
        else{
            out = STDOUT_FILENO;
        }
    }

    memset(wbuffer, 0, sizeof(wbuffer));
    memset(rbuffer, 0, sizeof(rbuffer));

    /*array of character frequencies*/
    if(!(freqs = (unsigned int*)calloc(BLOCK, sizeof(int)))){
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    /*processing file*/
    while((bytes_read = read(in, rbuffer, sizeof(rbuffer))) > 0){
        for(i = 0; i < bytes_read; i++){
            freqs[(unsigned char)rbuffer[i]] += 1;
        }
        memset(rbuffer, 0, sizeof(rbuffer));
    }

    /*ordered list of frequencies*/
    list = createList();
    for (i = 0; i < BLOCK; i++){
        if (freqs[i] > 0){
            insert(i, freqs[i], list);
        }
    }

    /*only continue with non empty files*/
    if(!list -> head){
        free(freqs);
        free(list);
        return;
    }

    /*arrays to create and store huffCodes*/
    if(!(code = (char*)calloc(BLOCK, sizeof(char)))){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    if(!(codes = (char**)calloc(BLOCK, sizeof(char*)))){
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    list = buildTree(list);
    genCodes(list -> head, 0, code, codes, BLOCK);

    /*encodes the header*/
    symbols = -1;
    for(i = 0; i < BLOCK; i++){
        if(codes[i]){
            symbols += 1;
        }
    }
    write(out, &symbols, sizeof(symbols));
    for(i = 0; i < BLOCK; i++){
       if(codes[i]){
            write(out, &i, 1);
            header_int = freqs[i];
            header_int = htonl(header_int);
            write(out, &header_int, sizeof(header_int));
       }
    }

    /*Go to start of file*/
    if(fstat(in, &st) == -1){
        perror("fstat");
        exit(EXIT_FAILURE);
    }
    file_size = st.st_size;
    if((lseek(in, 0, SEEK_SET) == -1)){
        perror("lseek");
        exit(EXIT_FAILURE);
    }
    
    /*encoding the file contents*/
    bitWriterStatus status = {1 << (BYTESIZE - 1), 0, 0, 0};

    while ((bytes_read = read(in, rbuffer, sizeof(rbuffer))) > 0){
        for (i = 0; i < bytes_read; i++){
            bitWrite(codes[(unsigned char)rbuffer[i]], 
                            &status, wbuffer, 0, out);
        }
        memset(rbuffer, 0, sizeof(rbuffer));
    }
    /*padding if needed*/
    if (status.bitcount > 0){
        bitWrite(NULL, &status, wbuffer, 1, out);
        write(out, wbuffer, status.index + 1);
        memset(wbuffer, 0, sizeof(wbuffer));
    }
    else if (status.index > 0){
        write(out, wbuffer, status.index);
        memset(wbuffer, 0, sizeof(wbuffer));
    }

    /*free everything*/
    for(i = 0; i < BLOCK; i++){
        if(codes[i]){
            free(codes[i]);
        }
    }
    free(freqs);
    freeTree(list -> head);
    free(list);
    free(code);
    free(codes);

    close(in);
    close(out);
    return 0;
}

