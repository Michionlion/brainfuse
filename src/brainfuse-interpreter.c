#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_DATA_SIZE 1000
#define INITIAL_LOOP_SIZE 10
#define RESIZE_CHANGE_ADD 1000
#define UNKNOWN 0x0
#define OPEN_BRACE 0x7
#define CLOSE_BRACE 0x8

char *program;
int program_length;

char **loop_stack;
char **loop_top;
long loop_size = INITIAL_LOOP_SIZE * sizeof(char*);

char *data_start;
long data_size = INITIAL_DATA_SIZE;
char *data_ptr;
char *inst_ptr;

void push (char * inst) {
    //printf("pushing %d to %d", inst, loop_top);
    *loop_top = inst;
    loop_top += sizeof(char *);
    if(loop_top > loop_stack + loop_size) {
        fprintf(stderr, "LOOP POINTER OVERFLOW\n");
        loop_stack = (char **) realloc(loop_stack, loop_size+(INITIAL_LOOP_SIZE * sizeof(char*)));
        loop_size+=INITIAL_LOOP_SIZE * sizeof(char*);
        fprintf(stderr, "RESIZED LOOP_STACK TO %ld BYTES\n", loop_size);
    }
}

//poping empty stack too much when running echo.bf, something is up here

char * pop() {
    if(loop_top <= loop_stack) {
        fprintf(stderr, "POP ON EMPTY LOOP_STACK, LIKELY UNMATCHED BRACKETS\n");
        loop_top = loop_stack;
        return (char *) *loop_top;
    }

    loop_top-=sizeof(long);
    return (char *) *loop_top;
}

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
    loop_stack = (char **) malloc(loop_size);
    loop_top = loop_stack;

    int i = 0;
    rewind(bf);
    while((ch=fgetc(bf)) != EOF) {
        //read a char, put in array
        if(ch=='<' || ch=='>' || ch=='-' || ch=='+' || ch=='.' || ch==',' || ch=='[' || ch==']') {
            program[i++] = ch;
        }
    }
    fclose(bf);
    //printf("read");
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
                //printf("%d", ch = *data_ptr);
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
                } else {
                    push(inst_ptr);
                }
                break;
            case ']':
                if(*data_ptr != 0) {
                    inst_ptr = pop();
                }
                break;
        }

        //debug data tracker
		//if(DEBUG) printf("\x1B[33m<DEBUG: data_ptr: %d (%d), after instruction %d (%c)>\x1B[0m", data_ptr-data_start, *data_ptr, inst_ptr-program, *(inst_ptr));
        inst_ptr++;

		#ifdef _WIN32
        //Sleep(1000);
		#else
        //sleep(1);
		#endif
    }
    free(data_start);
    free(loop_stack);
    return 0;
}
