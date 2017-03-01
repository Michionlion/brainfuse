#include <stdio.h>
char* include(char* operand) {
	printf("Directive: %s\n", operand);
	return "{included file}";
}
