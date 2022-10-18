#ifndef THREADS_H
#define THREADS_H

#include <time.h>
#include <pthread.h>


void * GUI(void *args);
void *generate_data(void *args); 
void *locate_object(void *args);
void *locate_object_pthread(void *args);
void *locate_object_omp(void *args);
void *locate_object_omp_for(void *args);
void *locate_object_pthread_affinity(void *args);
void timespec_add(const struct timespec *a, const struct timespec *b, struct timespec * res);
pthread_mutex_t* getMut(void);
pthread_barrier_t* getStart_barrier(void);
pthread_barrier_t* getEnd_barrier(void);

struct threadArgs{
    int id;
    int numThreads;
};

#endif
