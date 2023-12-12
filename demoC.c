#include <stdio.h>


extern char * hello(long mode);


int main(){

    printf("%s\n", hello(1));

    printf("%s\n", hello(0));

    return 0;
}