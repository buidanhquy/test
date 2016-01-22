#include <stdio.h>
#include <stdlib.h>

int main() {
    char a[10] = "abc";
    char *text = a;
    printf("sizeof 123456: %d\n", sizeof(text));
    printf("sizeof a: %d\n", sizeof(a));
    printf("sizeof 123456: %d\n", sizeof("abc"));
    printf("strlen 123456: %d\n", strlen(text)); 
    return 0;
}
