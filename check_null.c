#include <stdio.h>

int main() {
    char a[10] = "zzzzz";
    char b[10] ;

    printf("b: %s\n", b);

    if (b[0] != '\0'){
        printf("zzzzzz\n");
    }
    else {
        printf("aaaa\n");
    }
    return 0;
}
