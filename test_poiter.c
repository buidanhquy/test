#include <stdio.h>
#include <stdlib.h>

void test_pointer(int *a) {
    int b = 5;
    a = &b;
}

int main() {
    int a = 5;
    int *p = &a;
    printf("Value of a: %d\n", a);
    printf("Direction of a: %p\n", &a);

    printf("Value of b: %p\n", p);
    printf("Direction of b: %p\n", &p);
    return 0;
}
