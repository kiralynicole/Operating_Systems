#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include "a2_helper.h"
#define NR_THREADS 4


typedef struct {
int nb_prc;
int nb_th;
sem_t* logSem1;
sem_t* logSem2;
}TH_STRUCT;

void* thread_func_sync(void* arg){
    TH_STRUCT*threads = (TH_STRUCT *)arg;

    info(BEGIN, threads->nb_prc, threads->nb_th);

    info(END, threads->nb_prc, threads->nb_th);



return NULL;
}



void process_hierarchy(){
    pthread_t tid[NR_THREADS];
    TH_STRUCT params[NR_THREADS];
    sem_t logSem1, logSem2;
    pid_t pid2 = -1, pid3 = -1, pid4 = -1, pid5 = -1, pid6 = -1, pid7 = -1, pid8 = -1, pid9 = -1;
    int status2 = 0, status3 = 0, status4 = 0, status5 = 0, status6 = 0, status7 = 0, status8 = 0,status9 = 0;

    pid2 = fork();
    if(pid2 == -1){
        perror("Could not create child process");
        exit(-1);
    }else if(pid2 == 0){
        info(BEGIN, 2, 0);

        pid4 = fork();
        if(pid4 == -1){
            perror("Could not create child process");
            exit(-1);
        }else if(pid4 == 0){
            info(BEGIN,4, 0);

            pid6 = fork();
            if(pid6 == -1){
            perror("Could not create child process");
            exit(-1);
        }else if(pid6 == 0){
            info(BEGIN,6, 0);
            info(END, 6,0);
            exit(6);
        }else{
            waitpid(pid6, &status6, 0);
        }

        info(END, 4, 0);
        exit(4);
    }else{
        pid5 = fork();
        if(pid5 == -1){
        perror("Could not create child process");
        exit(-1);
        }else if (pid5 == 0){
        info(BEGIN, 5, 0);
        info(END, 5,0);
        exit(5);
        }else{
            waitpid(pid4, &status5, 0);
            waitpid(pid5, &status4, 0);
        }

    }
    info(END, 2, 0);
    exit(2);

    }else{

        pid3 = fork();
        if(pid3 == -1){
        perror("Could not create child process");
        exit (-1);
        }else if(pid3 == 0){
            info(BEGIN, 3, 0);

            pid8 = fork();
            if(pid8 == -1){
                perror("Could not create child process");
                exit(-1);
            }else if(pid8 == 0){
                info(BEGIN, 8, 0);

                pid9 = fork();
                if(pid9 == -1){
                     perror("Could not create child process");
                     exit(-1);
                }else if(pid9 == 0){
                    info(BEGIN, 9, 0);
                    info(END, 9,0);
                    exit(9);
                }else{
                    waitpid(pid9, &status9, 0);
                }

                info(END, 8, 0);
                exit(8);
            }else{
                waitpid(pid8, &status8, 0);
            }
            info(END, 3, 0);
            exit(3);

        }else{

            pid7 = fork();

            if(pid7 == -1){
                perror("Could not create child process");
                 exit(-1);
            }else if(pid7 == 0){
            info(BEGIN, 7,0);

            sem_init(&logSem1, 0, 0);
            sem_init(&logSem2, 0, 0); 


            for(int i=0; i < NR_THREADS; i++){
                params[i].nb_prc = 7;
                params[i].nb_th = i+1;
                params[i].logSem1 = &logSem1;
                params[i].logSem2 = &logSem2;
                pthread_create(&tid[i], NULL, thread_func_sync, &params[i]);
            }

            for(int i = 0; i < NR_THREADS; i++){
                pthread_join(tid[i], NULL);
            }

            info(END, 7, 0);
            exit(7);
            }
            else{
                waitpid(pid2, &status7, 0);
                waitpid(pid3, &status3, 0);
                waitpid(pid7, &status2, 0);
            }

        }
    }



}



int main(){
    init();

    info(BEGIN, 1, 0);

    process_hierarchy();
    
    info(END, 1, 0);
    return 0;
}