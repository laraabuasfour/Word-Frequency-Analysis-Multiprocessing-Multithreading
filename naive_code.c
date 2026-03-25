// Lara Abu Asfour 1221484
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/time.h>

#define MAX_WORDS 1000000
#define MAX_WORD_LENGTH 100

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

WordCount words[MAX_WORDS];
int wordCount = 0;

void toLowerCase(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

int findWordIndex(const char *word) {
    for (int i = 0; i < wordCount; i++) {
        if (strcmp(words[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

void processFile(const char *filename) {
    FILE *file = fopen("text88.txt", "r");
    if (!file) {
        perror("File opening failed");
        return;
    }

    char word[MAX_WORD_LENGTH];
    while (fscanf(file, "%s", word) != EOF) {
        toLowerCase(word);
        int index = findWordIndex(word);
        if (index == -1) {
            strcpy(words[wordCount].word, word);
            words[wordCount].count = 1;
            wordCount++;
        } else {
            words[index].count++;
        }
    }
    fclose(file);
}

int compare(const void *a, const void *b) {
    return ((WordCount *)b)->count - ((WordCount *)a)->count;
}

void printTop10() {
    for (int i = 0; i < 10 && i < wordCount; i++) {
        printf("%s: %d\n", words[i].word, words[i].count);
    }
}

int main() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    processFile("text88.txt");
    qsort(words, wordCount, sizeof(WordCount), compare);
    printf("Top 10 most frequent words using Naive approach:\n");
    printTop10();

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution time: %.2f seconds\n", elapsed);
    return 0;
}
