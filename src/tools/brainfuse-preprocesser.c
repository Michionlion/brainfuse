#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define DIRECTIVE_CHAR '~'

bool debug = false;

//declare functions
char* include(char* operand, char* name, bool std);
char* put(char* operand);
char* loadDirective(char* start, int length, char* name);
char* process(char* code, char* name);

void pstrn(const char* string, int length);

//implement functions

char* loadDirective(char* start, int length, char* name) {
	if(debug) printf("loadDirective(\"%s\", %d) - ", start, length);
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
		return include(op, name, std);
	} else if(strncmp(start, "put", directive_length) == 0) {
		//put directive
		return put(op);
	}

	//make heap string so can be freed
	char* load_error = (char*) malloc(23);
	load_error[22] = '\0';
	strcpy(load_error, "{DIRECTIVE LOAD ERROR}");

	return load_error;
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: bfp source destination\n");
		return 1;
	}

	FILE* source = fopen(argv[1], "r");
	if(source == NULL) {
		fprintf(stderr, "Error 1\n");
		return -1;
	}

	//find length
	fseek(source, 0L, SEEK_END);
	int length = ftell(source);
	if(debug) printf("Length: %d\n", length);
	rewind(source);
	//allocate mem for file
	char* src = (char*) malloc(length+1);
	src[length] = -1; //ensure end is -1

	int r = fread(src, 1, length, source); // read file into src (doesn't overwrite end)
	fclose(source); //close source file

	if(r != length) {
		fprintf(stderr, "ERROR 2\n");
		return -1;
	}


	FILE* dest = fopen(argv[2], "w");
	if(dest == NULL) {
		fprintf(stderr, "ERROR 3\n");
		return -1;
	}

	// printf("source:\n%s\n", src);

	char* out = process(src, argv[1]); //process src
	free(src);

	fputs(out, dest); // put string into file
	free(out);
	fclose(dest);


	printf("Finished preprocess pass!\n");
	return 0;
}

char* process(char* code, char* name) {
	char ch;
	bool in_dir = false;

	int src_pos= 0 ;

	char* dir_start;
	int dir_length = 0;
	int dir_pos = 0;

	char* dst_start = (char*) malloc(20);
	int dst_length = 20;
	int dst_pos = 0;

	// printf("processing:\n");
	for(src_pos=0; (ch=code[src_pos]) != -1; src_pos++) {
		if(debug) printf("\nchar: '%c'\nstatus: ", ch);
		if(ch==DIRECTIVE_CHAR && !in_dir) {
			//open new directive
			dir_start = (char*) malloc((dir_length = 10));
			in_dir = true;
			if(debug) printf("dir-start\n");
		} else if (ch==DIRECTIVE_CHAR && in_dir) {
			if(debug) printf("dir-end ");
			//close directive
			//insert directive contents
			dir_start[dir_pos] = '\0';
			char* dir_contents = loadDirective(dir_start, dir_pos, name);

			dst_start[dst_pos] = '\0';
			int len = strlen(dir_contents);
			dst_pos+=len;
			dst_start = (char*) realloc(dst_start, (dst_length += len+1));
			dst_start = strncat(dst_start, dir_contents, len);

			if(debug) printf(" - dst skipped to (%d/%d)\n", dst_pos, dst_length);

			//free mem
			free(dir_start);
			free(dir_contents);

			//reset
			in_dir = false;
			dir_start = 0;
			dir_length = 0;
			dir_pos = 0;
		} else if(in_dir) {
			if(debug) printf("dir-in (%d/%d)\n", dir_pos, dir_length);

			//resize if needed
			if(dir_pos + 1 >= dir_length) dir_start = (char*) realloc(dir_start, (dir_length +=10));

			dir_start[dir_pos] = ch;
			dir_pos++;
		} else {
			if(debug) printf("non-dir (%d/%d)\n", dst_pos, dst_length);
			//outside directive
			if(dst_pos + 1 >= dst_length) {
				//should be able to calc this in the future so don't need to do on-the-fly realloc
				dst_start = (char*) realloc(dst_start, (dst_length += 20));
				if(debug) printf("RESIZED DST TO %d", dst_length);
			}
			dst_start[dst_pos] = ch;
			dst_pos++;
		}
	}

	//resize to minimum
	dst_start = (char*) realloc(dst_start, dst_pos+1);
	dst_start[dst_pos] = '\0';

	return dst_start;
}

// DIRECTIVES

char* include(char* operand, char* name, bool std) {
	if(strcmp(operand, name) == 0) {
		//recursive include
		fprintf(stderr, "Error 4 - Recursive include statements not allowed\n");
		char* str = (char*) malloc(43);
		str[42] = '\0';
		strcpy(str, "{recursive include statements not allowed}");
		free(operand);
		return str;
	}
	if(debug) printf("Loading '%s', ", operand);

	FILE* source;

	if(std) {
		char* INCLUDE_LIB = getenv("BF_LIB");
		if(INCLUDE_LIB == NULL) {
			fprintf(stderr, "Error 5 - could not read environment variable 'BF_LIB'\n");
			char* str = (char*) malloc(23);
			str[22] = '\0';
			strcpy(str, "{error reading BF_LIB}");
			free(operand);
			return str;
		}
		int len1 = strlen(INCLUDE_LIB);
		int len = len1 + strlen(operand)+1;
		char* fn = (char*) malloc(len+1);
		strcpy(fn, INCLUDE_LIB);
		if(fn[len1-1] != '/') strcat(fn, "/"); //ensure slash
		strcat(fn, operand);
		fn[len] = '\0'; // ensure string null terminator

		source = fopen(fn, "r");
		if(source == NULL) {
			fprintf(stderr, "Error 6 - could not find library '%s'\n", fn);
			char* str = (char*) malloc(21);
			str[20] = '\0';
			strcpy(str, "{error reading file}");
			free(operand);
			return str;
		}
	} else {
		source = fopen(operand, "r");
	}
	if(source == NULL) {
		fprintf(stderr, "Error 7\n");
		char* str = (char*) malloc(21);
		str[20] = '\0';
		strcpy(str, "{error reading file}");

		free(operand);
		return str;
	}

	//find length
	fseek(source, 0L, SEEK_END);
	int length = ftell(source);
	if(debug) printf("length: %d ", length);
	rewind(source);
	//allocate mem for file
	char* src = (char*) malloc(length+1);
	src[length] = -1; //ensure end is -1

	int r = fread(src, 1, length, source); // read file into src (doesn't overwrite end)
	fclose(source); //close source file

	if(r != length) {
		fprintf(stderr, "ERROR 8\n");
		char* str = (char*) malloc(21);
		str[20] = '\0';
		strcpy(str, "{error reading file}");
		free(src);
		free(operand);
		return str;
	}

	char* out = process(src, operand); //process src
	free(src);
	free(operand);

	return out;
}

char* put(char* operand) {
	if(debug) printf("Operand: '%s'", operand);
	char* str = (char*) malloc(21);
	str[20] = '\0';
	strcpy(str, "{put generated code}");
	free(operand);
	return str;
}

void pstrn(const char* string, int length) {
	int i;
	for(i=0; i<length; i++) {
		putchar(string[i]);
	}
}
