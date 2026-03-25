# Word-Frequency-Analysis-Multiprocessing-Multithreading

This project was developed for the course **ENCS3390 – Operating System** at **Birzeit University**.

The goal of this project is to analyze the **enwik8 dataset** and determine the **Top 10 most frequent words** using three different approaches. The performance of each approach is measured and compared based on execution time.

--------------------------------------------------

## Project Objective

The objective of this project is to explore how **parallelism** improves program performance when processing large datasets.

Three different approaches are implemented:

1. Naive (Sequential) Approach
2. Multiprocessing Approach
3. Multithreading Approach

Each approach processes the dataset and calculates the **10 most frequent words**, then measures the **execution time**.

--------------------------------------------------

## Dataset

The dataset used in this project is:

enwik8

This dataset contains a large text corpus extracted from Wikipedia and is commonly used for benchmarking text processing.

The program reads the dataset from the file:

text88.txt

--------------------------------------------------

## Implemented Approaches

### 1. Naive Approach

The naive approach processes the dataset sequentially using a single thread.

Characteristics:

- Simple implementation
- No parallelism
- Processes the file word by word
- Stores words and frequencies in an array
- Uses linear search to update frequencies
- Sorts results using qsort

This method is the **slowest** because it does not utilize multiple CPU cores.

--------------------------------------------------

### 2. Multiprocessing Approach

This approach uses multiple child processes to process the dataset in parallel.

Key features:

- Uses fork() to create child processes
- Divides the dataset into chunks
- Each process handles a different part of the file
- Shared memory is used to combine results
- Words and frequencies are merged after processing

This approach improves performance by using multiple CPU cores.

--------------------------------------------------

### 3. Multithreading Approach

This approach uses POSIX threads (pthreads) to process the dataset in parallel.

Key features:

- Uses pthread_create() to create threads
- Threads share the same memory space
- A mutex is used to protect shared data
- Each thread processes a chunk of the file
- Results are merged into a shared array

Multithreading allows faster communication between threads but requires synchronization.

--------------------------------------------------

## Performance Comparison

Execution time was measured for each approach.

General observations:

- The **Naive approach** is the slowest.
- **Multiprocessing** significantly improves performance.
- **Multithreading** also improves performance and is often slightly faster due to lower overhead.

--------------------------------------------------

## Amdahl’s Law

The project analyzes the results using **Amdahl’s Law** to estimate the theoretical speedup when using multiple processors.

The serial part of the program limits the maximum achievable speedup even when using many CPU cores.

## Output

Each program prints:

- Top 10 most frequent words
- Word frequency
- Execution time

Example output:

Top 10 most frequent words:
the : 1061377
of : 593668
and : 416621
one : 411757
in : 372200
a : 325869
to : 316573
zero : 264973
nine : 250429
two : 192644

--------------------------------------------------

## Technologies Used

Programming Language: C  
Operating System: Linux / Ubuntu  
Libraries:
- pthread
- shared memory (mmap)
- standard C libraries
