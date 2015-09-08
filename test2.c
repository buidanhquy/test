#include <stdio.h>
#include <string.h>
 
int count_words(char *src, const char *word) {
    int count = 0;
    int word_len = strlen(word);
    char *p = src;
    while ((p = strstr(p, word)) != NULL) {
        count++;
        p += word_len * sizeof(char);
    }
    return count;
}
 
int main() {
    char str[10] = {'me', 'me'};
    char *test;
    strcpy(test,str);
    printf("%d\n", count_words(test, "me"));
}
