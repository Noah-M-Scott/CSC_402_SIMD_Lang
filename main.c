#include <stdio.h>
#include <stdlib.h>

extern void* frontEnd(char *in);
extern int middleEnd(void* in);

int main(){
	backEnd("out.s", frontEnd("demo.txt"));

	return 0;
}
