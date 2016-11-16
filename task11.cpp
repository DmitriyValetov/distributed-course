#include <iostream>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>
#include <limits.h>
#include <mutex>
#include <math.h>
#include <malloc.h>
#include <stdio.h>
#include <time.h>

int* matrix;
int pthread_count, matrix_size;
pthread_mutex_t mp = PTHREAD_MUTEX_INITIALIZER;

struct result {
    int maximum, minimum;
    int* max_coor; int* min_coor;
    int max_counter, min_counter;
};

struct timespec begin, end; // use -lrt
double elapsed;
result answer;

void* funct(void* arg) {
    int ind = *((int*)arg);
    int l = matrix_size / (pthread_count + 1) * ind;
    int r;
    int loc_counter_max = 0, loc_counter_min = 0;
    int *loc_max_coor = (int*)malloc(sizeof(int));
    int *loc_min_coor = (int*)malloc(sizeof(int));
    pthread_mutex_init(&mp, NULL);

    if (matrix_size < matrix_size / (pthread_count + 1) + ind)
        r = matrix_size;
    else
        r = matrix_size / (pthread_count + 1) + ind;

    if (r <= l) {
        return 0;
    }
    int loc_max = matrix[l];
    int loc_min = matrix[l];
    loc_max_coor[0] = l; loc_min_coor[0] = l;
    loc_counter_max = 1; loc_counter_min = 1;

    for (int j = l; j < r; ++j) {
        if ((matrix[j] == loc_max) && (j != l))
        {
            loc_max_coor = (int*)realloc(loc_max_coor, (loc_counter_max + 1) * sizeof(int));
            loc_counter_max++;
            loc_max_coor[loc_counter_max - 1] = j;
        }
        if (matrix[j] > loc_max)
        {
            loc_max = matrix[j];
            loc_max_coor = (int*)realloc(loc_max_coor, sizeof(int));
            loc_counter_max = 1;
            loc_max_coor[0] = j;
        }
        if ((matrix[j] == loc_min) && (j != l))
        {
            loc_min_coor = (int*)realloc(loc_min_coor, (loc_counter_min + 1) * sizeof(int));
            loc_counter_min++;
            loc_min_coor[loc_counter_min- 1] = j;
        }
        if (matrix[j] > loc_min)
        {
            loc_min = matrix[j];
            loc_min_coor = (int*)realloc(loc_min_coor, sizeof(int));
            loc_counter_min = 1;
            loc_min_coor[0] = j;
        }
    }

    pthread_mutex_lock(&mp);
    if (answer.maximum == loc_max)
    {
        answer.max_coor = (int*)realloc(answer.max_coor, (loc_counter_max + answer.max_counter) * sizeof(int));
        for (int k = answer.max_counter; k < (loc_counter_max + answer.max_counter); ++k)
            answer.max_coor[k] = loc_max_coor[k - answer.max_counter];
        answer.max_counter += loc_counter_max;
    }
    if (answer.maximum < loc_max)
    {
        answer.maximum = loc_max;
        answer.max_coor = (int*)realloc(answer.max_coor, loc_counter_max * sizeof(int));
        answer.max_counter = loc_counter_max;
        for (int k = 0; k < loc_counter_max; ++k)
            answer.max_coor[k] = loc_max_coor[k];
    }
    if (answer.minimum == loc_min)
    {
        answer.min_coor = (int*)realloc(answer.min_coor, (loc_counter_min + answer.min_counter) * sizeof(int));
        for (int k = answer.min_counter; k < (loc_counter_min + answer.min_counter); ++k)
            answer.min_coor[k] = loc_min_coor[k - answer.min_counter];
        answer.min_counter += loc_counter_min;
    }
    if (answer.minimum < loc_min)
    {
        answer.minimum = loc_min;
        answer.min_coor = (int*)realloc(answer.min_coor, loc_counter_min * sizeof(int));
        answer.min_counter = loc_counter_min;
        for (int k = 0; k < loc_counter_min; ++k)
            answer.min_coor[k] = loc_min_coor[k];
    }
    pthread_mutex_unlock(&mp);

    return 0;
}

int main(int argc, char** argv) {

    pthread_count = atoi(argv[1]);
    int size = atoi(argv[2]);

    matrix_size = size * size;
    matrix = (int*)malloc(matrix_size * sizeof(int));

    pthread_t* threads = (pthread_t*)malloc(pthread_count * sizeof(pthread_t));

    answer.maximum = 0;
    answer.minimum = 0;

    for (int i = 0; i < matrix_size; ++i)
        matrix[i] = rand();

    clock_gettime(CLOCK_REALTIME, &begin);
    for (int i = 0; i < pthread_count; ++i)
    {
        int fine = pthread_create(&(*(threads + i)), NULL, funct, (void*) &i);
        if (fine) {
            exit(1);
        }
    }

    for (int i = 0; i < pthread_count; ++i)
    {
        int fine = pthread_join(threads[i], NULL);
        if (fine)
            exit(1);
    }

    clock_gettime(CLOCK_REALTIME, &end);
    elapsed = (end.tv_sec - begin.tv_sec)+(end.tv_nsec - begin.tv_nsec) / 1000000000.0;
    
    printf("%f\n", elapsed);

    free(matrix);
    free(threads);
    pthread_mutex_destroy(&mp);
    return 0;
}
