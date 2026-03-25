//Lara Abu Asfour 1221484
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/time.h>

#define MAX_WORDS 1000000
#define MAX_WORD_LENGTH 100
#define NUM_PROCESSES 2

typedef struct {
    char word[MAX_WORD_LENGTH];
    int count;
} WordCount;

// Shared word array
WordCount *words;
int *shared_wordCount;

void toLowerCase(char *str) {
    while (*str) {
        *str = tolower((unsigned char)*str);
        str++;
    }
}

int findWordIndex(const char *word, int *wordCount) {
    for (int i = 0; i < *wordCount; i++) {
        if (strcmp(words[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

void processFilePart(FILE *file, long start, long end, int process_id) {
    char word[MAX_WORD_LENGTH];
    fseek(file, start, SEEK_SET);
    while (ftell(file) < end && fscanf(file, "%s", word) != EOF) {
        toLowerCase(word);
        int index = findWordIndex(word, shared_wordCount);
        if (index == -1) {
            strcpy(words[*shared_wordCount].word, word);
            words[*shared_wordCount].count = 1;
            (*shared_wordCount)++;
        } else {
            words[index].count++;
        }
    }
}

int compare(const void *a, const void *b) {
    return ((WordCount *)b)->count - ((WordCount *)a)->count;  // Sort in descending order
}

void printTop10() {
    for (int i = 0; i < 10 && i < *shared_wordCount; i++) {
        printf("%s: %d\n", words[i].word, words[i].count);
    }
}

int main() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    int shm_fd = shm_open("words", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd, sizeof(WordCount) * MAX_WORDS);
    words = mmap(0, sizeof(WordCount) * MAX_WORDS, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int shm_fd_count = shm_open("wordCount", O_CREAT | O_RDWR, 0666);
    ftruncate(shm_fd_count, sizeof(int));
    shared_wordCount = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd_count, 0);
    *shared_wordCount = 0;

    FILE *file = fopen("text88.txt", "r");
    if (!file) {
        perror("File opening failed");
        return 1;
    }

    long file_size = 0;
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fclose(file);

    int chunk_size = file_size / NUM_PROCESSES;

    for (int i = 0; i < NUM_PROCESSES; i++) {
        int start = i * chunk_size;
        int end = (i == NUM_PROCESSES - 1) ? file_size : (i + 1) * chunk_size;

        pid_t pid = fork();
        if (pid == 0) {
            file = fopen("text88.txt", "r");
            if (file == NULL) {
                perror("File opening failed");
                exit(1);
            }
            processFilePart(file, start, end, i);
            fclose(file);
            exit(0);
        }
    }

    for (int i = 0; i < NUM_PROCESSES; i++) {
        wait(NULL);
    }

    qsort(words, *shared_wordCount, sizeof(WordCount), compare);
    printf("Top 10 most frequent words using multiprocessing (2 childs):\n");
    printTop10();

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1000000.0;
    printf("Execution time: %.2f seconds\n", elapsed);

    munmap(words, sizeof(WordCount) * MAX_WORDS);
    shm_unlink("words");
    munmap(shared_wordCount, sizeof(int));
    shm_unlink("wordCount");

    return 0;
}
