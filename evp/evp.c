#include <stdio.h>
#include <string.h>
#include <openssl/evp.h>

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv, unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    ctx = EVP_CIPHER_CTX_new();
    EVP_EncryptInit(ctx, EVP_aes_256_cbc(), key, iv);
    EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len);
    ciphertext_len = len;
    EVP_EncryptFinal_ex(ctx, ciphertext + len,  &len);
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    printf("Encrypted Done! ");
    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key, unsigned char *iv, unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    ctx = EVP_CIPHER_CTX_new();
    EVP_DecryptInit(ctx, EVP_aes_256_cbc(), key, iv);
    EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len);
    plaintext_len = len;
    EVP_DecryptFinal(ctx, ciphertext + len,  &len);
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);
    printf("Decrypted Done! ");
    return plaintext_len;
}

int main() {
    char *key = "01234567890123456789012345678901";
    char *iv = "01234567890123456"; //If key is one time pass then iv can be null
    char plaintext[] = "The quick brown fox jumps over the lazy dog";
    char ciphertext[128];
    char decryptedtext[128];

    int ciphertext_len;
    int decryptedtext_len;

    ciphertext_len = encrypt(plaintext, strlen(plaintext), key, iv, ciphertext);
    printf("Ciphertext is:\n");
    BIO_dump_fp (stdout, (const char *)ciphertext, ciphertext_len);

    decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv, decryptedtext);
    decryptedtext[decryptedtext_len] = '\0';

    printf("Decrypted text is:\n");
    printf("%s\n", decryptedtext);

    return 0;
}
