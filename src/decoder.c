#include "utility.h"
#include<fcntl.h>
#include<unistd.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<arpa/inet.h>
    
#define BLOCK 256
#define CHUNK 4096
#define BYTESIZE 8
#define INTERVAL 5

int main(int argc, char *argv[]){
    int in = 0;
    int out = 1;
    char rbuffer[CHUNK];
    char wbuffer[CHUNK];

    unsigned int *freqs = NULL;
    linkedList *list = NULL;
    huffNode *dummy = NULL;

    size_t bytes_read;
    off_t file_size;
    struct stat st;

    unsigned char current;
    unsigned int headersize, value, freqtotal, buflen;
    int i, j, test, written, symbols;
   
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

    /* Processing the header*/
    freqtotal = 0;
    bytes_read = read(in, rbuffer, sizeof(rbuffer));
    value = 0;

    headersize = (int)(unsigned char)rbuffer[0] + 1;

    for(i = 1, j = 0; j < headersize; i += INTERVAL, j++){
        value = ntohl(*(unsigned int*)(&rbuffer[i + 1]));
        freqs[(unsigned char)rbuffer[i]] = value;
        freqtotal += value;
    }
    memset(rbuffer, 0, sizeof(rbuffer));

    /* frequencies of the header*/
    list = createList();
    symbols = 0;
    for (i = 0; i < BLOCK; i++){
        if (freqs[i] > 0){
            insert((unsigned char)i, freqs[i], list);
            symbols += 1;
        }
    }

    /*only continue with non empty files*/
    if(!list -> head){
        free(freqs);
        free(list);
        return;
    }
    /* build a tree using the header*/
    list = buildTree(list);

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

    /* decoding the file contents
     * while skipping the header bytes
     */
    bytes_read = read(in, rbuffer, INTERVAL*(headersize) + 1);
    memset(rbuffer, 0, sizeof(rbuffer));

    dummy = list -> head;
    written = 0;
    buflen = 0;

    /*edgecase: only one char*/
    if (symbols == 1){
        for (i = 0; i < dummy -> frequency; i++){
            wbuffer[i] = dummy -> letter;
            buflen += 1;
            if(buflen >= CHUNK){
                write(out, wbuffer, sizeof(wbuffer));
                memset(wbuffer, 0, sizeof(wbuffer));
                buflen = 0;
            }
        }
    }
    
    /*regular decoding*/
    while ((bytes_read = read(in, rbuffer, sizeof(rbuffer))) > 0){
        for (i = 0; i < bytes_read; i++){
            current = rbuffer[i];
            for(j = BYTESIZE - 1; j >= 0; j--){
                test = current & (1 << j);
                if (test != 0){
                    dummy = dummy -> right;
                }
                else{
                    dummy = dummy -> left;
                }
                if (dummy -> left == NULL && dummy -> right == NULL){
                    wbuffer[buflen] = dummy -> letter;
                    dummy = list -> head;
                    buflen += 1;
                    written += 1;
                }
                if (buflen >= CHUNK){
                    write(out, wbuffer, sizeof(wbuffer));
                    memset(wbuffer, 0, sizeof(wbuffer));
                    buflen = 0;
                }
                if (written >= freqtotal){
                    break;
                }
            } 
        }
        memset(rbuffer, 0, sizeof(rbuffer));
    }
    /*writes leftover chars in buffer*/
    if (buflen >= 0){
        write(out, wbuffer, buflen);
    }

    /*free everything*/
    free(freqs);
    freeTree(list -> head);
    free(list);

    close(in);
    close(out);
    return 0;
}

