// Lara Abu Asfour 1221484
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_WORD_LEN 50
#define INITIAL_CAPACITY 1000000
#define NUM_THREADS 8

typedef struct {
    char word[MAX_WORD_LEN];
    int frequency;
} WordFreq;

//shared data structure
WordFreq *sharedArray;
int sharedSize = 0;
pthread_mutex_t mutex;

//find a word in an array
int findWord(WordFreq *array, int size, const char *word) {
    for (int i = 0; i < size; i++) {
        if (strcmp(array[i].word, word) == 0) {
            return i;
        }
    }
    return -1;
}

//add a word to the shared array
void addWord(const char *word, int frequency) {
    pthread_mutex_lock(&mutex);

    int index = findWord(sharedArray, sharedSize, word);
    if (index != -1) {
        sharedArray[index].frequency += frequency;
    } else {
        if (sharedSize >= INITIAL_CAPACITY) {
            fprintf(stderr, "Exceeded shared array capacity\n");
            pthread_mutex_unlock(&mutex);
            exit(EXIT_FAILURE);
        }
        strcpy(sharedArray[sharedSize].word, word);
        sharedArray[sharedSize].frequency = frequency;
        sharedSize++;
    }

    pthread_mutex_unlock(&mutex);
}

//pass arguments to threads
typedef struct {
    long start;
    long end;
} ThreadArg;

//for threads to process a chunk of the file
void *processChunk(void *arg) {
    ThreadArg *threadArg = (ThreadArg *)arg;

    // open a separate file pointer for this thread
    FILE *file = fopen("text88.txt", "r");
    if (!file) {
        perror("Error opening file");
        pthread_exit(NULL);
    }

    fseek(file, threadArg->start, SEEK_SET);
    long end = threadArg->end;

    WordFreq *localArray = malloc(INITIAL_CAPACITY * sizeof(WordFreq));
    if (!localArray) {
        fprintf(stderr, "Memory allocation failed\n");
        fclose(file);
        pthread_exit(NULL);
    }
    int localSize = 0;
    int localCapacity = INITIAL_CAPACITY;

    char word[MAX_WORD_LEN];
    while (ftell(file) < end && fscanf(file, "%49s", word) != EOF) {
        int index = findWord(localArray, localSize, word);
        if (index != -1) {
            localArray[index].frequency++;
        } else {
            if (localSize >= localCapacity) {
                localCapacity *= 2;
                WordFreq *newArray = realloc(localArray, localCapacity * sizeof(WordFreq));
                if (!newArray) {
                    fprintf(stderr, "Realloc failed\n");
                    free(localArray);
                    fclose(file);
                    pthread_exit(NULL);
                }
                localArray = newArray;
            }
            strcpy(localArray[localSize].word, word);
            localArray[localSize].frequency = 1;
            localSize++;
        }
    }

    //merge results into the shared array
    for (int i = 0; i < localSize; i++) {
        addWord(localArray[i].word, localArray[i].frequency);
    }

    free(localArray);
    fclose(file);
    return NULL;
}

//comparison function for sorting
int compare(const void *a, const void *b) {
    const WordFreq *wordA = (const WordFreq *)a;
    const WordFreq *wordB = (const WordFreq *)b;
    return wordB->frequency - wordA->frequency; // Descending order
}

int main() {
    struct timeval start, end;
    gettimeofday(&start, NULL);

    FILE *file = fopen("text88.txt", "r");
    if (!file) {
        perror("Error opening file");
        return EXIT_FAILURE;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    rewind(file);

    //initialize shared data
    sharedArray = malloc(INITIAL_CAPACITY * sizeof(WordFreq));
    if (!sharedArray) {
        fprintf(stderr, "Failed to allocate shared array\n");
        return EXIT_FAILURE;
    }
    pthread_mutex_init(&mutex, NULL);

    //create threads
    pthread_t threads[NUM_THREADS];
    ThreadArg threadArgs[NUM_THREADS];
    long chunkSize = fileSize / NUM_THREADS;

    for (int i = 0; i < NUM_THREADS; i++) {
        threadArgs[i].start = i * chunkSize;
        threadArgs[i].end = (i == NUM_THREADS - 1) ? fileSize : (i + 1) * chunkSize;

        if (pthread_create(&threads[i], NULL, processChunk, &threadArgs[i]) != 0) {
            perror("Failed to create thread");
            return EXIT_FAILURE;
        }
    }

    //wait for threads to finish
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    //sort the shared array
    qsort(sharedArray, sharedSize, sizeof(WordFreq), compare);

    printf("Top 10 most frequent words using nultithreading (8):\n");
    for (int i = 0; i < 10 && i < sharedSize; i++) {
        printf("%s: %d\n", sharedArray[i].word, sharedArray[i].frequency);
    }

    //cleanup
    pthread_mutex_destroy(&mutex);
    free(sharedArray);
    fclose(file);

    gettimeofday(&end, NULL);
    double elapsed = (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;
    printf("Execution time: %.6f seconds\n", elapsed);
    return 0;
}
