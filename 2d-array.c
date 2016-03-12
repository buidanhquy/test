#include <stdio.h>
#include <string.h>

void print_array(char ary[][10]) {
    int i;

    for (i = 0; i < 5; i++) {
        printf("%s\n", ary[i]);
    }
}

int main(void)
{
    char newarray[4][10];

    strcpy(newarray[0], "zzzz");
    strcpy(newarray[1], "vvvv");
    strcpy(newarray[2], "xxxx");
    strcpy(newarray[3], "qqqq");

    print_array(newarray);    

    return 0;
}
