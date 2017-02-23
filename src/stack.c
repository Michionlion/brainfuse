#include <stdio.h>
#include <stdlib.h>

typedef struct node {
	char *data;
	void *below;
} node;

node *top = NULL;
int size = 0;

void push(char *d) {
	if(size < 1) {
		top = (node *) malloc(sizeof(node));
		top->data = d;
		size = 1;
	} else {
		node *tmp = top;
		top = (node *) malloc(sizeof(node));
		top->data = d;
		top->below = tmp;
		size++;
	}
}

char * pop() {
	if(size>1) {
		node *tmp = top;
		top = (node *) top->below;
		size--;
		char *r = tmp->data;
		free(tmp);
		return r;
	} else {
		
		size = 0;
		char *r = top->data;
		free(top);
		return r;
	}
}

void stack_free() {
	while(top!=NULL) {
		node *tmp = top;
		top = top->below;
		free(tmp);
	}
}

int main() {
	char in;

	char list[] = {'a','b','c','d','e','f','g','h','i','j','k'};
	int done = 1;
	while(done) {
		printf("Enter <op> (+ is push, - is pop, = is display, q is quit): ");
		scanf("%c", &in);
		switch(in) {
			case '+':
				printf("Enter 0-10 to push: ");
				int i;
				scanf("%d", &i);
				printf("Pushing %c\n", list[i]);
				push(list+i);
				break;
			case '-':
				printf("popped '%c'\n", *pop());
				break;
			case '=':
				printf("size=%d, val=%c, addr=%ld\n", size, *(top->data), top->data);
				break;
			case 'q':
				done = 0;
				break;
		}
	}
	stack_free();
	return 0;
}
