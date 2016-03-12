#include <stdio.h> 

int main(int argc, char *argv[])
{
    int i, j;
    char a[20];
   
    j = fgets(a, 20, stdin);
    printf("a: %s", a);
    return 0;
}
