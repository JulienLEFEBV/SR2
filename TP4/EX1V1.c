#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

pthread_mutex_t *mutex;
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
        if(pthread_mutex_lock(&(mutex[(*num)]))==-1) erreur();
        for(int j=0;j<NBL;j++) printf("Afficheur %d (%lu), j'affiche la ligne %d/%d du message %d/%d \n",*num,id,j,NBL,i,NBM);
        if(i==NBM-1) printf("Afficheur %d (%lu), je me termine\n",*num,id);
        if(pthread_mutex_unlock(&(mutex[((*num)+1)%NBT]))==-1) erreur();
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
    mutex=malloc(sizeof(pthread_mutex_t)*NBT);
    if(mutex==NULL) erreur();
    tid=malloc(sizeof(pthread_t)*NBT);
    if(tid==NULL) erreur();
    param=malloc(sizeof(int)*NBT);
    if(param==NULL) erreur();
    for(int i=0;i<NBT;i++){
        if (pthread_mutex_init(&(mutex[i]),NULL)==-1) erreur();
        if(i!=0) if(pthread_mutex_lock(&(mutex[i]))==-1) erreur();
        param[i]=i;
        if(pthread_create(&(tid[i]),NULL,affichage,(void *)&(param[i]))==-1) erreur();
    }
    for(int i=0;i<NBT;i++) if(pthread_join(tid[i],NULL)==-1) erreur();
    for(int i=0;i<NBT;i++) if(pthread_mutex_destroy(&(mutex[i]))==-1) erreur();
    free(mutex);
    free(param);
    free(tid);
    printf("Fin de l'execution du thread principal\n");
    return 0;
}