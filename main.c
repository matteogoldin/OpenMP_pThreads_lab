#include "threads.h"
#include <pthread.h>
#include <stdlib.h>

#define AFFINITY

int main(int argc, char **argv)
{

    srand(time(NULL));
     
    pthread_t gui_th, gendata_th;
    pthread_mutex_t* mut = getMut();
    pthread_barrier_t* start_barrier = getStart_barrier();
    pthread_barrier_t* end_barrier = getEnd_barrier();

    pthread_create(&gui_th, NULL, GUI, NULL);
    pthread_create(&gendata_th, NULL,generate_data , NULL);
    
    #ifdef PTHREAD
    //pthread without affinity
    int nThread = 4;  
    pthread_t threads_array[nThread];
    struct threadArgs targsArray[nThread];
    pthread_mutex_init(mut, NULL);
    pthread_barrier_init (start_barrier, NULL, nThread);
    pthread_barrier_init (end_barrier, NULL, nThread);  

    for(int i = 0; i < nThread; i++){
        targsArray[i].id = i;
        targsArray[i].numThreads = nThread;
        pthread_create(&threads_array[i],NULL,locate_object_pthread,&targsArray[i]);
    }
    for(int i = 0; i < nThread; i++){
        pthread_join(threads_array[i], NULL);
    }


    #elif defined(OPENMP)
    //Open MP
    pthread_t locate_th;
    pthread_create(&locate_th,NULL,locate_object_omp,NULL);
    pthread_join(locate_th, NULL);

    #elif defined(OPENMP_FOR)
    //Open MP for
    pthread_t locate_th;
    pthread_create(&locate_th,NULL,locate_object_omp_for,NULL);
    pthread_join(locate_th, NULL);
    
    
    #elif defined(AFFINITY)
    //pthread affinity
    int nThread = 4;  
    pthread_t threads_array[nThread];
    struct threadArgs targsArray[nThread];
    pthread_mutex_init(mut, NULL);
    pthread_barrier_init (start_barrier, NULL, nThread);
    pthread_barrier_init (end_barrier, NULL, nThread);  
    pthread_attr_t attr_array[nThread];
    struct sched_param param_array[nThread];

    for(int i = 0; i < nThread; i++){
        pthread_attr_init(&attr_array[i]);
        pthread_attr_setschedpolicy(&attr_array[i], SCHED_FIFO);
        param_array[i].sched_priority = 1;
        pthread_attr_setschedparam(&attr_array[i], &param_array[i]);
    }

    for(int i = 0; i < nThread; i++){
        targsArray[i].id = i;
        targsArray[i].numThreads = nThread;
        pthread_create(&threads_array[i],&attr_array[i],locate_object_pthread_affinity,&targsArray[i]);
    }

    for(int i = 0; i < nThread; i++){
        pthread_join(threads_array[i], NULL);
    }

    for(int i = 0; i < nThread; i++){
        pthread_attr_destroy(&attr_array[i]);
    }
  
    #endif
    pthread_join(gendata_th, NULL);
    pthread_join(gui_th, NULL);
}
