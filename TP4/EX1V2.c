#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

sem_t *sem;
int NBL,NBM,NBT;

void erreur(void)
{
    perror("Erreur ");
    pthread_exit(NULL);
}

void * affichage(void * param){
    int * num = (int *) param;
    pthread_t id=pthread_self();
    for(int i=0;i<NBM;i++){
        if(sem_wait(&(sem[(*num)]))==-1) erreur();
        for(int j=0;j<NBL;j++) printf("Afficheur %d (%lu), j'affiche la ligne %d/%d du message %d/%d \n",*num,id,j,NBL,i,NBM);
        if(i==NBM-1) printf("Afficheur %d (%lu), je me termine\n",*num,id);
        if(sem_post(&(sem[((*num)+1)%NBT]))==-1) erreur();
    }
    return NULL;
}

int main(int argc, char* argv[]){
    if(argc!=4){
        fprintf(stderr,"Usage : %s [nb thread] [nb_message] [nb_ligne]\n",argv[0]);
    }
    NBT=atoi(argv[1]);
    NBM=atoi(argv[2]);
    NBL=atoi(argv[3]);
    pthread_t *tid;
    int *param;
    sem=malloc(sizeof(sem_t)*NBT);
    if(sem==NULL) erreur();
    tid=malloc(sizeof(pthread_t)*NBT);
    if(tid==NULL) erreur();
    param=malloc(sizeof(int)*NBT);
    if(param==NULL) erreur();
    for(int i=0;i<NBT;i++){
        if (i==0) {if (sem_init(&(sem[i]),0,1)==-1) erreur();}
        else if (sem_init(&(sem[i]),0,0)==-1) erreur();
        param[i]=i;
        if(pthread_create(&(tid[i]),NULL,affichage,(void *)&(param[i]))==-1) erreur();
    }
    for(int i=0;i<NBT;i++) if(pthread_join(tid[i],NULL)==-1) erreur();
    for(int i=0;i<NBT;i++) if(sem_destroy(&(sem[i]))==-1) erreur();
    free(sem);
    free(param);
    free(tid);
    printf("Fin de l'execution du thread principal\n");
    return 0;
}