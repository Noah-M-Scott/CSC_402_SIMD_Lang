#include <stdio.h>


extern char * hello(long mode);
extern float foo(long count);

int main(){

    printf("%s\n", hello(1));

    printf("%s\n", hello(0));

    printf("%f\n", foo(4));

    return 0;
}