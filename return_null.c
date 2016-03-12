#include <stdlib.h>
#include <stdio.h>
#include <string.h>

char *return_null() {
    return NULL;
}

int main() {
    char *pph;

    pph = return_null();
    
    if (pph == NULL) {
        printf("null\n");
    }

}
