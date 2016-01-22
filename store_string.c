#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char strings[3][256];   
    scanf("%s %s %s", strings[0], strings[1], strings[2]);
    printf("%s\n%s\n%s\n", strings[0], strings[1], strings[2]);
}
