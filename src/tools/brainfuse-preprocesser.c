#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define DIRECTIVE_CHAR '~'

char* loadDirective(char* start, int length) {
	int i;
	for(i=0; i < length; i++) {
		//printout directive
		putchar(start[i]);
	}

	//load directive body (whatever it is)


	return "{dir_body}";
}

int main(int argc, char** argv) {
	if(argc < 2) {
		fprintf(stderr, "Usage: bfp source destination\n");
		return 1;
	}

	FILE* source = fopen(argv[1], "r");
	FILE* dest = fopen(argv[2], "w");
	if(source == NULL || dest == NULL) {
		printf("Error");
		return -1;
	}

	char ch;
	bool in_dir = false;
	char* dir_start;
	int dir_length = 0;
	while((ch=fgetc(source)) != EOF) {
		if(ch==DIRECTIVE_CHAR && !in_dir) {
			//open new directive
			dir_start = malloc(20);
			in_dir = true;
		} else if (ch==DIRECTIVE_CHAR && in_dir) {
			//close directive
			//insert directive contents
			dir_start[dir_length++] = '\0';
			char* dir_contents = loadDirective(dir_start, dir_length);
			fputs(dir_contents, dest);

			//reset
			in_dir = false;
			dir_start = 0;
			dir_length = 0;
		} else if(in_dir) {
			//inside directive
			//putchar(ch);
			dir_start[dir_length++] = ch;
			//resize if needed
			if(dir_length > 15) dir_start = realloc(dir_start, dir_length+10);
		} else {
			//outside directive
			fputc(ch, dest);
		}
	}

	fclose(source);
	fclose(dest);

	//write output


	return 0;
}
