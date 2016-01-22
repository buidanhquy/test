#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char a[10] = "abc123";
    char b[10];

    strcpy(b, a);
    printf("b: %s\n", b);

    strcpy(a, "zzzz");
    printf("b: %s\n", b);

    return 0;
}
