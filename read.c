#include <stdio.h>

main() {

    FILE *fp;
    char *dummy, buff[255];
    char field[10], value[10];

    fp = fopen("device.conf", "r");

    while (fscanf(fp, "%10[^=]=%10[^=]", field, value) != EOF) {
        fprintf(stdout, "%s,%s\n", field, value);
    }

    fclose(fp);

}
