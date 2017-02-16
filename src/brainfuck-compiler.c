#include <stdio.h>
#include <stdlib.h>

#define INC_DATA "++*ptr;\n"
#define DEC_DATA "--*ptr;\n"
#define INC_PTR "++ptr;\n"
#define DEC_PTR "--ptr;\n"
#define READ "*ptr = getchar();\n"
#define WRITE "putchar(*ptr);\n"
#define LOOP_START "while(*ptr!=0) {\n"
#define LOOP_END "}\n"
#define PROGRAM_START "#include<stdio.h>\n#include<stdlib.h>\nint main() {\nchar *sptr = calloc(%ld, 1);\nchar *ptr = sptr;\n\n//PROGRAM\n"
#define PROGRAM_END "//END PROGRAM\n\nfree(sptr);\nreturn 0;\n}"

int main(int argc, char **argv) {
	long data_size = 30000;
	if(argc < 3) {
		fprintf(stderr, "Usage: bfc filename output [DATA_SIZE]\n");
		return 1;
	}
	if(argc > 3) {
		data_size = atoi(argv[3]);
		if(data_size <= 0) data_size = 1;
	}
	FILE *bf = fopen(argv[1], "r");
	FILE *c = fopen(argv[2], "w");

	if(c==NULL) {
		printf("Error");
		return 1;
	}
	fprintf(c, PROGRAM_START, data_size);
	char ch;
	while((ch=fgetc(bf)) != EOF) {
		switch(ch) {
            case '>':
				fprintf(c, INC_PTR);
                break;
            case '<':
				fprintf(c, DEC_PTR);
                break;
            case '+':
				fprintf(c, INC_DATA);
                break;
            case '-':
				fprintf(c, DEC_DATA);
                break;
            case '.':
				fprintf(c, WRITE);
                break;
            case ',':
				fprintf(c, READ);
                break;
            case '[':
				fprintf(c, LOOP_START);
                break;
            case ']':
				fprintf(c, LOOP_END);
                break;
		}
	}

	fprintf(c, PROGRAM_END);
	fclose(bf);
	fclose(c);

	return 0;
}
