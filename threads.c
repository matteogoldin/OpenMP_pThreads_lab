#define _GNU_SOURCE
#include "threads.h"
//#include "sdl_utils.h"
#include "play_utils.h"
 
#include<stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <stdlib.h>
//#include <SDL.h>
#include <pthread.h>
#include <omp.h>

#define PERIOD_MS 50
#define NUM_CORES 4

bool data[MAP_WIDTH][MAP_HEIGHT];
int quit = 0;
int xrange,yrange,is,js;
pthread_mutex_t lock;
bool object_detect = false;
pthread_mutex_t mut;
pthread_barrier_t   start_barrier, end_barrier;

void timespec_add(const struct timespec *a, const struct timespec *b, struct timespec *res)
{
    res->tv_nsec = a->tv_nsec + b->tv_nsec;
    res->tv_sec = a->tv_sec + b->tv_sec;
    while (res->tv_nsec >= 1000000000)
    {
        res->tv_nsec -= 1000000000;
        res->tv_sec += 1;
    }
}

void *GUI(void *args)
{

    //SDL_init();
    play_init();

    struct timespec next, period = {0, PERIOD_MS * 1000000};
    clock_gettime(CLOCK_MONOTONIC, &next);

    while (quit == 0)
    {
        timespec_add(&next, &period, &next);

        /*SDL_PumpEvents();
        const uint8_t *keystate = SDL_GetKeyboardState(NULL);
        if (keystate[SDL_SCANCODE_ESCAPE])
        {
            quit = 1;
        }

        SDL_clear_map();

        */
        play_move_ball();
        /*
        SDL_draw_ball();
        SDL_draw_map();

        if (is != -10)
        {
	  SDL_draw_detection(is, js, xrange, yrange);
        }



        SDL_refresh();*/

        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
    //SDL_destroy();
}

void *generate_data(void *args)
{
    struct timespec next, period = {0, PERIOD_MS * 1000000};
    clock_gettime(CLOCK_MONOTONIC, &next);
    while (quit == 0)
    {
        timespec_add(&next, &period, &next);
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                data[i][j] = ((i >= getBall()->x) && (i <= getBall()->x + BALL_WIDTH) && (j >= getBall()->y) && (j >= getBall()->y + BALL_HEIGHT)) ? true : false;
            }
        }
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next, NULL);
    }
}

void *locate_object_omp_for(void *args){
    double wcet = 0.0;
    clock_t start, end;
    while (quit == 0)
    {
        object_detect = false;
        start = clock();
        #pragma omp parallel for shared(object_detect)
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if(data[i][j]){
                    #pragma omp critical
                    {
                        object_detect = true;
                    }
                    //break;
                    j = MAP_HEIGHT;
                    i = MAP_WIDTH;
                } 
            }
        }
    
        end = clock();
        double execTime = (double) (end - start) / CLOCKS_PER_SEC;
        if(wcet < execTime) wcet = execTime; 
        printf("WCET: %lf \n", wcet);
    }
}

void *locate_object_omp(void *args){
    double wcet = 0.0;
    clock_t start, end;
    while (quit == 0)
    {
        object_detect = false;
        start = clock();
        #pragma omp parallel shared(object_detect)
        {
            for (int i = omp_get_thread_num(); i < MAP_WIDTH; i+=omp_get_num_threads())
            {
                for (int j = 0; j < MAP_HEIGHT; j++)
                {
                    if(data[i][j]){
                        #pragma omp critical
                        {
                            object_detect = true;
                        }
                    } 
                    if(object_detect) break;
                }
                if(object_detect) break;
            } 
        }
        end = clock();
        double execTime = (double) (end - start) / CLOCKS_PER_SEC;
        if(wcet < execTime) wcet = execTime; 
        printf("WCET: %lf \n", wcet);
    }
}

void *locate_object(void *args){
    double wcet = 0.0;
    clock_t start, end;
    while (quit == 0)
    {
        object_detect = false;
        start = clock();
        for (int i = 0; i < MAP_WIDTH; i++)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if(data[i][j]){
                    object_detect = true;
                    break;
                }    
            }
            if(object_detect) break;
        }
        end = clock();
        double execTime = (double) (end - start) / CLOCKS_PER_SEC;
        if(wcet < execTime) wcet = execTime; 
        printf("WCET: %lf \n", wcet);
    }
}

void *locate_object_pthread(void *args){
    struct threadArgs * th = (struct threadArgs *) args;
    double wcet = 0.0;
    clock_t start, end;
    while (quit == 0)
    {
        pthread_barrier_wait (&start_barrier);
        if(th->id==0){
            object_detect = false;
            start = clock();
        } 
        for (int i = th->id; i < MAP_WIDTH; i+=th->numThreads)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                if(data[i][j]){
                    pthread_mutex_lock(&mut);
                    object_detect = true;
                    pthread_mutex_unlock(&mut);
                } 
                if(object_detect) break;
            }
            if(object_detect) break;
        }
        printf("Task%d terminates \n", th->id);
        pthread_barrier_wait (&end_barrier);
        if(th->id == 0){
            end = clock();
            double execTime = (end - start) / (double) CLOCKS_PER_SEC;
            if(wcet < execTime) wcet = execTime; 
            printf("WCET: %lf \n", wcet);
        }
    } 
}

void *locate_object_pthread_affinity(void *args){
    struct threadArgs * th = (struct threadArgs *) args;
    double wcet = 0.0;
    clock_t start, end;
    cpu_set_t cpuset;
    int cpu = th->id % NUM_CORES;
    CPU_ZERO(&cpuset);
    CPU_SET( cpu , &cpuset);
    sched_setaffinity(0, sizeof(cpuset), &cpuset);
    while (quit == 0)
    {
        pthread_barrier_wait (&start_barrier);
        if(th->id==0){
            object_detect = false;
            start = clock();
        } 
        for (int i = th->id; i < MAP_WIDTH; i+=th->numThreads)
        {
            for (int j = 0; j < MAP_HEIGHT; j++)
            {
                
                
                if(data[i][j] == true){
                    pthread_mutex_lock(&mut);
                    object_detect = true;
                    pthread_mutex_unlock(&mut);
                } 
                if(object_detect) break;
            }
            if(object_detect) break;
        }
        printf("Task%d terminates \n", th->id);
        pthread_barrier_wait (&end_barrier);
        if(th->id == 0){
            end = clock();
            double execTime = (end - start) / (double) CLOCKS_PER_SEC;
            if(wcet < execTime) wcet = execTime; 
            printf("WCET: %lf \n", wcet);
        }
    } 
}

pthread_mutex_t* getMut(void){
        return &mut;
}

pthread_barrier_t* getStart_barrier(void){
    return &start_barrier;
}

pthread_barrier_t* getEnd_barrier(void){
    return &end_barrier;
}
