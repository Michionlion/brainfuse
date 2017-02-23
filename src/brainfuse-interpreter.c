#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.c"
#define INITIAL_DATA_SIZE 1000
#define INITIAL_LOOP_SIZE 10
#define RESIZE_CHANGE_ADD 1000

char *program;
int program_length;

char *data_start;
long data_size = INITIAL_DATA_SIZE;
char *data_ptr;
char *inst_ptr;

int main(int argc, char **argv) {
    if(argc < 2) {
        fprintf(stderr, "Usage: bf filename\n");
        return 1;
    }
    int DEBUG = 0;
    if(argc > 2) DEBUG = 1;
    //load program
    FILE *bf = fopen(argv[1], "r");
    if(bf == NULL) {
        fprintf(stderr, "I/O ERROR");
        return 1;
    }

    if(DEBUG) printf("FINISHED FILE");

    char ch;
    program_length = 0;
    while((ch=fgetc(bf)) != EOF) {
        //read a char, add to count
        if(ch=='<' || ch=='>' || ch=='-' || ch=='+' || ch=='.' || ch==',' || ch=='[' || ch==']') {
            program_length++;
        }
    }
    program = (char *) malloc(program_length);

    int i = 0;
    rewind(bf);
    while((ch=fgetc(bf)) != EOF) {
        //read a char, put in array
        if(ch=='<' || ch=='>' || ch=='-' || ch=='+' || ch=='.' || ch==',' || ch=='[' || ch==']') {
            program[i++] = ch;
        }
    }
    fclose(bf);
    //interpret
    data_start = (char *) calloc(data_size, 1);
    data_ptr = data_start; //initialize ptr to start of data
    inst_ptr = program;
    char* prev_ins = program;
    while(inst_ptr < (char *)(program + program_length)) {
        //debug data tracker
		if(DEBUG) printf("\x1B[33m<DEBUG: data_ptr: %ld (%d), before instruction %ld (%c)>\x1B[0m", data_ptr-data_start, *data_ptr, inst_ptr-program, *(inst_ptr));
        switch(*inst_ptr) {
            case '>':
                ++data_ptr;
                if(data_ptr - data_start >= data_size) {
                    data_start = (char *) realloc(data_start, data_size+RESIZE_CHANGE_ADD);
                    memset(data_start+data_size, 0, RESIZE_CHANGE_ADD); //zeros
                    data_size+=RESIZE_CHANGE_ADD;
                    fprintf(stderr, "RESIZED ARRAY TO %ld BYTES\n", data_size);
                }
                break;
            case '<':
                --data_ptr;
                if(data_ptr < data_start) fprintf(stderr, "DATA POINTER FELL OFF\n");
                break;
            case '+':
                ++*data_ptr;
                break;
            case '-':
                --*data_ptr;
                break;
            case '.':
				putchar(ch = *data_ptr);
                break;
            case ',':
                *data_ptr = getchar();
                break;
            case '[':
                if(*data_ptr == 0) {
                    //jump to after ]
                    int b = 1;
                    while(b>0){
                        switch(*(++inst_ptr)) {
                            case '[':
                                b++;
                                break;
                            case ']':
                                b--;
                                break;
                        }
                    }
                    //sanity check to ensure not double-looping
                    while(peek() == inst_ptr) pop();

                } else {
                    push(inst_ptr);
                }
                break;
            case ']':
                if(*data_ptr != 0) {
                    char * p = peek();
                    if(p != NULL) inst_ptr = p;
                    else fprintf(stderr, "GOT NULL RESULT\n");
                } else pop();
                break;
        }
        inst_ptr++;

    }
    free(data_start);
    stack_free();
    return 0;
}
