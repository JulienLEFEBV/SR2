#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <semaphore.h>

#define NBMAX 20

char ** tab;
int *depose;
int i_depot, i_conso, NBF_prod, NBF_conso,taille_tab;
sem_t sem_depot,sem_conso;
pthread_mutex_t mutex_i_depot=PTHREAD_MUTEX_INITIALIZER, mutex_i_conso=PTHREAD_MUTEX_INITIALIZER;

void erreur(void)
{
    perror("Erreur ");
    pthread_exit(NULL);
}

void produire(char *msg,pthread_t id){
    tab[i_depot]=strcpy(tab[i_depot],msg);
    i_depot=(i_depot+1)%taille_tab;
}

void consommer(char * msg,pthread_t id){
    msg=strcpy(msg,tab[i_conso]);
    for(int i;i<NBMAX;i++) tab[i_conso][i]='x';
    i_conso=(i_conso+1)%taille_tab;
}

void * consommateur(void* arg){
    int *num=(int *) arg;
    char *msg;
    pthread_t id=pthread_self();
    if((msg=malloc(sizeof(char)*NBMAX))==NULL) erreur();
    for (int i=0;i<NBF_conso;i++){
        sprintf(msg,"Bonjour %2d prod %2d",i,*num);
        if(sem_wait(&sem_conso)==-1) erreur();
        if(pthread_mutex_lock(&mutex_i_conso)==-1) erreur();
        consommer(msg,id);
        if(pthread_mutex_unlock(&mutex_i_conso)==-1) erreur();
        if(sem_post(&sem_depot)==-1) erreur();
        printf("Conso %d (%lu): Message lu = %s\n",*num,id,msg);
    }
    free(msg);
    return NULL;
}

void * producteur(void* arg){
    int *num=(int *) arg;
    char *msg;
    if((msg=malloc(sizeof(char)*NBMAX))==NULL) erreur();
    pthread_t id=pthread_self();
    for (int i=0;i<NBF_prod;i++){
        sprintf(msg,"Bonjour %2d prod %2d",i,*num);
        if(sem_wait(&sem_depot)==-1) erreur();
        if(pthread_mutex_lock(&mutex_i_depot)==-1) erreur();
        produire(msg,id);
        if(pthread_mutex_unlock(&mutex_i_depot)==-1) erreur();
        if(sem_post(&sem_conso)==-1) erreur();
        printf("Prod %d (%lu): Message depose = %s\n",*num,id,msg);
    }
    free(msg);
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc!=6){
        fprintf(stderr,"Usage : %s [NB producteur] [NB consommateur] [NB production] [NB consomation] [taille du tableau]\n",argv[0]);
        exit(1);
    }
    int NBP=atoi(argv[1]),NBC=atoi(argv[2]);
    NBF_prod=atoi(argv[3]);
    NBF_conso=atoi(argv[4]);
    taille_tab=atoi(argv[5]);
    if(sem_init(&sem_depot,0,taille_tab)==-1) erreur();
    if(sem_init(&sem_conso,0,0)==-1) erreur();
    pthread_t *tab_id;
    if((tab_id=malloc((NBP+NBC)*sizeof(pthread_t)))==NULL) erreur();
    int *tab_i;
    if((tab_i=malloc((NBP+NBC)*sizeof(int)))==NULL) erreur();
    if((depose=malloc(taille_tab*sizeof(int)))==NULL) erreur();
    if((tab=malloc(taille_tab*sizeof(char *)))==NULL) erreur();
    for(int i=0;i<taille_tab;i++) if((tab[i]=malloc(NBMAX*sizeof(char)))==NULL) erreur();
    for(int i=0; i<NBP;i++){
        tab_i[i]=i+1;
        if(pthread_create(&(tab_id[i]),NULL,producteur,(void *)&(tab_i[i]))==-1) erreur();
    }
    for(int i=0; i<NBC;i++){
        tab_i[i+NBP]=i+1;
        if(pthread_create(&(tab_id[i+NBP]),NULL,consommateur,(void *)&(tab_i[i+NBP]))==-1) erreur();
    }
    for(int i=0;i<(NBC+NBP);i++) if(pthread_join(tab_id[i],NULL)==-1) erreur();
    free(tab_i);
    for(int i=0;i<taille_tab;i++) free(tab[i]);
    free(tab);
    free(tab_id);
    if(pthread_mutex_destroy(&mutex_i_conso)==-1) erreur();
    if(pthread_mutex_destroy(&mutex_i_depot)==-1) erreur();
    if(sem_destroy(&sem_conso)==-1) erreur();
    if(sem_destroy(&sem_depot)==-1) erreur();
    return 0;
}