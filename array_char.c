#include <stdio.h>
#include <stdlib.h>

int main(void)
{
        char *CharPtr[3];
        char a[4]="abc";
        char b[4]="def";
        char c[4]="ghi";
        CharPtr[0]=a;
        CharPtr[1]=b;
        CharPtr[2]=c;

        printf("\n content of CharPtr[0] =%s",CharPtr[0]);
        printf("\n content of CharPtr[1] =%s",CharPtr[1]);
        printf("\n content of CharPtr[2] =%s\n",CharPtr[2]);

        printf(" \n content of char a[4]=%s",a);
        printf(" \n content of char b[4]=%s",b);
        printf(" \n content of char c[4]=%s\n",c);
}
