#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define INITIAL_DATA_SIZE 1000
#define RESIZE_CHANGE_ADD 1000

char *program;
int program_length;

char *data_start;
int data_size = INITIAL_DATA_SIZE;
char *data_ptr;
char *inst_ptr;

int main(int argc, char **argv) {
    if(argc < 2) fprintf(stderr, "Usage: bf filename\n");
    //load program
    FILE *bf = fopen(argv[1], "r");
    char ch;
    program_length = 0;
    while((ch=fgetc(bf)) != EOF) {
        //read a char, add to count
        if(ch=='<' || ch=='>' || ch=='-' || ch=='+' || ch=='.' || ch==',' || ch=='[' || ch==']') {
            program_length++;
        }
    }
    program = malloc(program_length);
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
        #ifdef DEBUG
        //debug data tracker
		printf("\x1B[33m<DEBUG: data_ptr: %d (%d), before instruction %d (%c)>\x1B[0m", data_ptr-data_start, *data_ptr, inst_ptr-program, *(inst_ptr));
		#endif
        switch(*inst_ptr) {
            case '>':
                ++data_ptr;
                if(data_ptr - data_start >= data_size) {
                    data_start = (char *) realloc(data_start, data_size+RESIZE_CHANGE_ADD);
                    memset(data_start+data_size, 0, RESIZE_CHANGE_ADD); //zeros
                    data_size+=RESIZE_CHANGE_ADD;
                    fprintf(stderr, "RESIZED ARRAY TO %d BYTES\n", data_size);
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
                    prev_ins = inst_ptr;
                }
                break;
            case ']':
                if(*data_ptr != 0) {
                    inst_ptr = prev_ins;
                }
                break;
        }

        #ifdef DEBUG
        //debug data tracker
		//printf("\x1B[33m<DEBUG: data_ptr: %d (%d), after instruction %d (%c)>\x1B[0m", data_ptr-data_start, *data_ptr, inst_ptr-program, *(inst_ptr));
        #endif
        inst_ptr++;
		
		#ifdef _WIN32
        //Sleep(1000);
		#else
        //sleep(1);
		#endif
    }
    free(data_start);
    return 0;
}
