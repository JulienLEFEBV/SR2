#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

int N,n_arrive=0;
sem_t semaphore;
pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;

void erreur(void)
{
    perror("Erreur ");
    pthread_exit(NULL);
}

void * traitement(void* arg)
{
    int *num=(int*) arg;
    pthread_t id=pthread_self();
    printf("Thread %d (%lu): Je commence mon traitement en parallele avec les autres\n",*num,id);
    for(int i=0;i<100000;i++);
    printf("Thread %d (%lu): J'arrive au Rdv\n",*num,id);
    if (pthread_mutex_lock(&mutex)==-1) erreur();
    n_arrive++;
    int moi=n_arrive;
    if(pthread_mutex_unlock(&mutex)==-1) erreur();
    if(moi<N){
        printf("Thread %d (%lu): Je ne suis pas le dernier au Rdv\n",*num,id);
        if(sem_wait(&semaphore)==-1) erreur();
    }
    else{
        printf("Thread %d (%lu): Je suis le dernier au Rdv\n",*num,id);
        for(int i=0;i<N-1;i++) if(sem_post(&semaphore)==-1) erreur();
    }
    printf("Thread %d (%lu): Je passe le point de Rdv\n",*num,id);
    printf("Thread %d (%lu): Je commence mon traitement en parallele avec les autres\n",*num,id);
    for(int i=0;i<100000;i++);
    printf("Thread %d (%lu): Je termine mon traitement\n",*num,id);
    return NULL;
}

int main(int argc, char * argv[]){
    if (argc!=2){
        fprintf(stderr,"Usage : %s [NB Thread]\n",argv[0]);
        exit(1);
    }
    N=atoi(argv[1]);
    if (sem_init(&semaphore,0,0)==-1) erreur();
    pthread_t *thread;
    if((thread=malloc(N*sizeof(pthread_t)))==NULL) erreur();
    int *num;
    if((num=malloc(N*sizeof(int)))==NULL) erreur();
    for(int i=0;i<N;i++){
        num[i]=i+1;
        if(pthread_create(&(thread[i]),NULL,traitement,(void *)(&(num[i])))==-1) erreur();
    }
    for(int i=0;i<N;i++) if(pthread_join(thread[i],NULL)==-1) erreur();
    free(thread);
    free(num);
    sem_destroy(&semaphore);
    pthread_mutex_destroy(&mutex);
    printf("Fin de l'execution du thread principal\n");
    return 0;
}