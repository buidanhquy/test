#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
    char arr[10][255];

    int i, count;

    for (i = 0; i < 10; i++) {
        strcpy(arr[i], "");
    }

    strcpy(arr[0], "123");
    strcpy(arr[2], "zxc");
    strcpy(arr[7], "asd");

    for (i = 0; i < 10; i++) {
        printf("arr[%d]: %s\n", i, arr[i]);
    }

    count = 0;
    for (i = 0; i < 10; i++) {
        if (arr[i][0] != '\0') {
            count++;
        }
    }

    printf("count: %d\n", count);
    
    return 0;
}
