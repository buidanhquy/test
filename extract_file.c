#include <stdlib.h>
#include <stdio.h>
#include <string.h>

void usage(char *app) {
    printf("%s <file_dir>\n", app);
    exit(1);
}

int main(int argc, char *argv[]) {
    FILE  *file;
    char s[1000];
    char *pch;
    int i;

    if (argc < 2) {
        usage(argv[0]);
    }

    file = fopen(argv[1], "r");
    if (file == NULL)
    {
        perror("fopen()");
        return 1;
    }

    while (fgets(s,1000,file)!=NULL) {
        if (s[0] != ';' & (strstr(s, "username=") || strstr(s, "secret="))) {
            pch = strtok(s, "=");
            pch = strtok (NULL, "=");

            i = strlen(pch);
            if (pch[i-1] == '\n') {
                pch[--i] = '\0';
            }
            printf("%s -- %s\n", s, pch);
        }
    }

    fclose(file);

    return 0;
}
