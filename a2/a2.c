#include <stdio.h>
#include <unistd.h>
#include<stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "a2_helper.h"
#define NR_THREADS1 4
#define NR_THREADS2 46
#define NR_THREADS3 4
sem_t*sem1 = NULL,*sem2 = NULL;


typedef struct {
int nb_prc;
int nb_th;
sem_t* logSem1;
sem_t* logSem2;
}TH_STRUCT;

typedef struct {
int nb_prc;
int nb_th;
sem_t* logSem1;
}TH_STRUCT2;

typedef struct {
int nb_prc;
int nb_th;
sem_t* logSem1;
}TH_STRUCT3;


void* thread_func_sync(void* arg){
    TH_STRUCT*threads = (TH_STRUCT *)arg;

    if(threads->nb_th == 1){
        sem_wait(threads->logSem1);
    }
     if( threads->nb_th == 2 ){
        sem_wait(threads->logSem1);
    }
     if( threads->nb_th == 4){
        sem_wait(threads->logSem1);
    }

    info(BEGIN, threads->nb_prc, threads->nb_th);

    if(threads->nb_th == 3){
        sem_post(threads->logSem1);
        sem_post(threads->logSem1);
        sem_post(threads->logSem1);
        sem_wait(threads->logSem2);
    }


    info(END, threads->nb_prc, threads->nb_th);

    if(threads->nb_th == 2){
        sem_post(threads->logSem2);
    }

return NULL;
}

void* thread_func_bar(void* arg){
    TH_STRUCT2*threads = (TH_STRUCT2 *)arg;

    sem_wait(threads->logSem1);

    info(BEGIN, threads->nb_prc, threads->nb_th);

    info(END, threads->nb_prc, threads->nb_th);

    sem_post(threads->logSem1);


return NULL;
}


void* thread_func3(void* arg){
    TH_STRUCT3*threads = (TH_STRUCT3 *)arg;

    info(BEGIN, threads->nb_prc, threads->nb_th);

    info(END, threads->nb_prc, threads->nb_th);


return NULL;
}



void process_hierarchy(){
    pthread_t tid[NR_THREADS1];
    TH_STRUCT params[NR_THREADS1];
     pthread_t tid2[NR_THREADS2];
    TH_STRUCT2 params2[NR_THREADS2];
    pthread_t tid3[NR_THREADS3];
    TH_STRUCT3 params3[NR_THREADS3];
    sem_t logSem1, logSem2;
    sem_t logSem_5;
    sem_t logSem_8;
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

        sem_init(&logSem_5, 0, 5);


         for(int i=0; i < NR_THREADS2; i++){
                params2[i].nb_prc = 5;
                params2[i].nb_th = i+1;
                params2[i].logSem1 = &logSem_5;
                pthread_create(&tid2[i], NULL, thread_func_bar, &params2[i]);
            }

            for(int i = 0; i < NR_THREADS2; i++){
                pthread_join(tid2[i], NULL);
            }

            sem_destroy(&logSem_5);


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
                sem_init(&logSem_8,0,0);

            for(int i=0; i < NR_THREADS3; i++){
                params3[i].nb_prc = 8;
                params3[i].nb_th = i+1;
                params3[i].logSem1 = &logSem_8;
                pthread_create(&tid3[i], NULL, thread_func3, &params3[i]);
            }

            for(int i = 0; i < NR_THREADS3; i++){
                pthread_join(tid3[i], NULL);
            }

            sem_destroy(&logSem_8);
                

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

            for(int i=0; i < NR_THREADS1; i++){
                params[i].nb_prc = 7;
                params[i].nb_th = i+1;
                params[i].logSem1 = &logSem1;
                params[i].logSem2 = &logSem2;
                pthread_create(&tid[i], NULL, thread_func_sync, &params[i]);
            }

            for(int i = 0; i < NR_THREADS1; i++){
                pthread_join(tid[i], NULL);
            }

            sem_destroy(&logSem1);
            sem_destroy(&logSem2);

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

//     sem1 = sem_open("semaphore1", O_CREAT, 0644, 1);
// if(sem1 == NULL) {
//      perror("Could not aquire the semaphore");
//  }

// sem2 = sem_open("semaphore2", O_CREAT, 0644, 1);
// if(sem2 == NULL) {
//      perror("Could not aquire the semaphore");
//  }
    info(BEGIN, 1, 0);

    process_hierarchy();
    
    info(END, 1, 0);

    sem_close(sem1);
    sem_close(sem2);

    return 0;
}