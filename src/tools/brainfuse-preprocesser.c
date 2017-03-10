#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DIRECTIVE_CHAR '~'

//declare functions
char* include(char* operand, bool std);
char* put(char* operand);
char* loadDirective(char* start, int length);
char* process(char* code);

void pstrn(const char* string, int length);

//implement functions

char* loadDirective(char* start, int length) {
	printf("loadDirective(\"%s\", %d)\n", start, length);
	int i;
	char* operand_start = NULL;
	int operand_length = 0;
	int directive_length = 0;
	int in_operand = 0;
	bool std;
	for(i=0; i < length; i++) {
		// printf("mode: %d\n", in_operand);
		if((start[i] == '<' || start[i] == '"') && in_operand == 0) {
			in_operand = 1;
			operand_start = start + i + 1;
			std = start[i] == '<';

			//remove trailing spaces from directive pointer length

			directive_length = i;

			int k = i;
			while(start[--k]==' ') {
				directive_length--;
			}

		} else if ((start[i] == '>' || start[i] == '"') && in_operand == 1) {
			operand_length = start + i - operand_start;
			in_operand = -1; // no more operand
		}// else if(in_operand == 1) {
		// 	//inside operand
		// 	// operand_length++;
		// } else if(in_operand == 0) {
		// 	//not in operand, add to directive
		// 	// directive_length++;
		// } else {
		// 	//after operand
		// }
	}

	//now that we have directive and operand

	if(operand_start == NULL) {
		fprintf(stderr, "OPERAND NOT SPECIFIED IN DIRECTIVE\n");
		return NULL;
	}

	// printf("OpL: %d, OpS: %s, DirL: %d, dirS: %s, dir: %s\n", operand_length, operand_start, directive_length, start, start);
	char* op = (char*) malloc(operand_length+1); //leave space for 0
	strncpy(op, operand_start, operand_length); // copy operand
	op[operand_length] = '\0'; // ensure 0

	//call correct directive functions with operand

	if(strncmp(start, "include", directive_length) == 0) {
		//include direction
		free(start); // free mem
		return include(op, std);
	} else if(strncmp(start, "put", directive_length) == 0) {
		//put directive
		free(start); //free mem
		return put(op);
	}

	return "{DIRECTIVE LOAD ERROR}";
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: bfp source destination\n");
		return 1;
	}

	FILE* source = fopen(argv[1], "r");
	if(source == NULL) {
		printf("Error 1");
		return -1;
	}

	//find length
	fseek(source, 0L, SEEK_END);
	int length = ftell(source);
	printf("Length: %d\n", length);
	rewind(source);
	//allocate mem for file
	char* src = (char*) malloc(length+1);
	src[length] = '\0'; //ensure end is 0

	int r = fread(src, 1, length, source); // read file into src (doesn't overwrite end)
	fclose(source); //close source file

	if(r != length) {
		printf("ERROR 2");
		return -1;
	}


	FILE* dest = fopen(argv[2], "w");
	if(dest == NULL) {
		printf("ERROR 3");
		return -1;
	}

	printf("source:\n%s\n", src);

	char* out = process(src); //process src
	free(src);

	fputs(out, dest); // put string into file
	free(out);
	fclose(dest);

	return 0;
}

char* process(char* code) {
	char ch;
	bool in_dir = false;
	char* dir_start;
	int dir_length = 0;
	int srcChar;
	int dstChar;
	int dst_length = 20;
	char* dst_start = (char*) malloc(20);

	printf("processing:\n");
	for(srcChar=0; (ch=code[srcChar]) != '\0'; srcChar++) {
		putchar(ch);
		if(ch==DIRECTIVE_CHAR && !in_dir) {
			//open new directive
			dir_start = (char*) malloc(20);
			in_dir = true;
		} else if (ch==DIRECTIVE_CHAR && in_dir) {
			//close directive
			//insert directive contents
			dir_start[dir_length] = '\0';
			char* dir_contents = loadDirective(dir_start, dir_length);
			//fputs(dir_contents, dest);

			printf("Got directive: %s\n", dir_contents);


			dst_start[dstChar] = '\0';
			printf("after 0");
			int len = strlen(dir_contents);
			printf("after 1");
			dstChar+=len;
			printf("after 2");
			dst_start = (char*) realloc(dst_start, (dst_length += len));
			printf("after 3");
			dst_start = strncat(dst_start, dir_contents, len);
			printf("after 4");

			//free mem
			free(dir_start);
			printf("after 6");
			free(dir_contents);
			printf("after 6");

			//reset
			in_dir = false;
			dir_start = 0;
			dir_length = 0;
		} else if(in_dir) {
			//inside directive
			//putchar(ch);
			dir_start[dir_length] = ch;
			dir_length++;
			//resize if needed
			if(dir_length > 15) dir_start = (char*) realloc(dir_start, dir_length+10);
		} else {
			//outside directive
			if(dstChar - 1 >= dst_length) dst_start = (char*) realloc(dst_start, (dst_length += 20));
			dst_start[dstChar] = ch;
			dstChar++;
		}
	}

	//resize to minimum
	dst_start = (char*) realloc(dst_start, dstChar);
	dst_start[dstChar] = '\0';

	return dst_start;
}

// DIRECTIVES

char* include(char* operand, bool std) {
	//printf("Loading '%s'\n", operand);

	//FILE* src = fopen(operand, "r");



	return "{included file}";
}

char* put(char* operand) {
	printf("Operand: '%s'\n", operand);
	return "{put generated code}";
}

void pstrn(const char* string, int length) {
	int i;
	for(i=0; i<length; i++) {
		putchar(string[i]);
	}
}
