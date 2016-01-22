#include <stdio.h>
#include <string.h>
#include <openssl/rsa.h>

int main()
{
    unsigned char in[] = "abc123";
    unsigned char out[20];

    SHA1(in, strlen(in), out);
RSA_public_encrypt(strlen(in), )
    int i;
    for (i = 0; i < 20; i++) {
        printf("%02x ", out[i]);
    }
    printf("\n");

    return 0;

}
