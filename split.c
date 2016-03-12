#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char a[100] = "123=qwe";
    char b[100];
    char *split;

    strcpy(b, a);

    split = strtok(b, "=");

    while (split != NULL) {
        printf ("%s\n",split);
        split = strtok (NULL, "=");
    }    
    printf("a: %s \nb: %s\n", a, b);
    return 0;
}
