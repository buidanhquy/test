#include <stdio.h> 

int main()
{
    int i, j;
    float x;
    int y;
    char str[] = "a, b,c, d";
    char str1[4];
    char str2[4];
    sscanf(str, "%3s%s%d%f", str1, str2, &i, &x);
     
    printf("i = %d x = %.f str1 = %s str2 = %s\n", i, x, str1, str2);
    return 0;
}
