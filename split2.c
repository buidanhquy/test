#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *get_value_by_name(char *name, char *url) {
    char *split, *split2, *split3;

    split = strstr(url, name);
    
    if (split == NULL) {
        return NULL;
    }

    split2 = strstr(split, "=");
    split3 = strtok(split2, "=&");

    return split3;
}

int main() {
    char a[100] = "?action=add_permission&acc_1=acc1&acc_2=123";
    char *split, *split2, *split3, *ret;
    int n;
    char buff[50];

    scanf("%s", buff);

    split3 = get_value_by_name(buff, a);

    printf("split: %s\n", split3);

    return 0;
}
