#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/rsa.h>
#include <openssl/evp.h>
#include <openssl/bio.h>
#include <openssl/err.h>
#include <stdio.h>

int padding = RSA_PKCS1_PADDING;

RSA *createRSA(unsigned char * key,int public) {
    RSA *rsa= NULL; 
    BIO *keybio ;
    keybio = BIO_new_mem_buf(key, -1);
    if (keybio==NULL) {
        printf( "Failed to create key BIO");
        return 0;
    }
    if (public) {
        rsa = PEM_read_bio_RSA_PUBKEY(keybio, &rsa,NULL, NULL);
    }
    else {
        rsa = PEM_read_bio_RSAPrivateKey(keybio, &rsa,NULL, NULL);
    }
    if(rsa == NULL) {
        printf( "Failed to create RSA");
    }
    return rsa;
}

int public_encrypt(unsigned char * data,int data_len,unsigned char * key, unsigned char *encrypted) {
    RSA *rsa = createRSA(key,1);
    int result = RSA_public_encrypt(data_len,data,encrypted,rsa,padding);
    return result;
}

int private_decrypt(unsigned char * enc_data,int data_len,unsigned char * key, unsigned char *decrypted) {
    RSA * rsa = createRSA(key,0);
    int  result = RSA_private_decrypt(data_len,enc_data,decrypted,rsa,padding);
    return result;
}

int main() {
    char plainText[50] = "Some text here 1234";

    char publicKey[]="-----BEGIN PUBLIC KEY-----\n"\
    "MIGfMA0GCSqGSIb3DQEBAQUAA4GNADCBiQKBgQsY175qJ/iOGFofaqR6ejDeGTV5\n"\
    "imsSuorjsgYc5uGdiiXioC2W0Hytkci2n0DlBkoPeMbuM70Cj24oaFC8JYn7TBcc\n"\
    "MGEjP02vUrwiobVx7oLNG1ukJ+bOaPq1QbPKqkw9ot97x0l23Q6+N8iyStdfT5WW\n"\
    "Py3kuxSUdIJPNjFVqwIDAQAB\n"\
        "-----END PUBLIC KEY-----\n";
    char privateKey[]="-----BEGIN RSA PRIVATE KEY-----\n"\
    "MIICXwIBAAKBgQsY175qJ/iOGFofaqR6ejDeGTV5imsSuorjsgYc5uGdiiXioC2W\n"\
    "0Hytkci2n0DlBkoPeMbuM70Cj24oaFC8JYn7TBccMGEjP02vUrwiobVx7oLNG1uk\n"\
    "J+bOaPq1QbPKqkw9ot97x0l23Q6+N8iyStdfT5WWPy3kuxSUdIJPNjFVqwIDAQAB\n"\
    "AoGBApIChux3za68HUPho69q1h7bn/NDCIISC1GqTpgrMl8XPKc9itHAAOEDP4Zt\n"\
    "ZM6nokfrJcjO2xzYtB9P6kNqPoYWG9dyTNOlTkyh89B4/TPCzgqgCvo9OfdnSOoy\n"\
    "HsNtCAZIil9Ct9/QDj6msqinfHI1l+NUcWa4F95ZMEZ44DphAkEDoO9PXRskHx2Z\n"\
    "aFOyqSh09QwDz3DLG7/d8K81ikBUHSUert/pJjyMd7oaMt3T3QmIYPyg5zW1rIBv\n"\
    "dB98ZUgQjQJBAw7kYld8IcNEOP04kcDezRRqnrInymfj8Zh3dLzDk35019f+F5Ge\n"\
    "CnBtJkNcV7+LKc1wzkfEdVJ25PeBQJ3vfRcCQQNxggbvFKVnfaApN1vQWFr8eyVW\n"\
    "gfPL5nwHPUzyX+8yJRqD2mmWZljp+MmJwaWeGMC5QKK0Jey7wLmDGanjNsq1AkEC\n"\
    "RbmMHUd+avKxie7T1sV3C1miK+0HKCUm6sywcGa+nPNDQxpp9ujBBwMDsH/YL9q/\n"\
    "KAlPLeKb7rersQFYu0It5wJBAZ3OROwHO/k3LyjEP0oP/Wbhp5asx5+xXkbXNRJJ\n"\
    "SYqOSy7gDLYF/LYuZ12iHqD+HMT1CZQ/rLmLlKNM/pH4d4s=\n"\
    "-----END RSA PRIVATE KEY-----\n";

    char encrypted[100];
    char decrypted[100];

    int encrypted_length= public_encrypt(plainText,strlen(plainText), publicKey,encrypted);

    if(encrypted_length == -1) {
        printf("Public Encrypt failed\n");
        exit(0);
    }
    printf("\nEncrypted length = %d\n",encrypted_length);

    int decrypted_length = private_decrypt(encrypted,encrypted_length, privateKey, decrypted);

    if(decrypted_length == -1) {
        printf("Private Decrypt failed\n");
        exit(0);
    }
    printf("Decrypted Text = %s\n",decrypted);
    printf("Decrypted Length = %d\n",decrypted_length);

    return 0;
}
