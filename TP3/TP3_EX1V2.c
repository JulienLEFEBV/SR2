#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h>

struct parametre{
    int num;
    int NBF;
};

void erreur(void)
{
    perror("Erreur ");
    exit(1);
}

void * afficher(void *vparam){
    struct parametre *param=(struct parametre *) vparam;
    pthread_t id;
    id=pthread_self();
    for(int j=0;j<param->NBF;j++) printf("Thread %d : identifiant = %lu\n",param->num,id);
    int *retour=malloc(sizeof(int));
    if(retour==NULL) erreur();
    *retour=param->num;
    pthread_exit((void *) retour);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr,"Usage: %s [Nb activités parallèles] [Nb affichage]\n",argv[0]);
        exit(1);
    }
    int NA,NF;
    NA=atoi(argv[1]);
    NF=atoi(argv[2]);
    pthread_t *tid;
    tid=malloc(sizeof(pthread_t)*NA);
    if(tid==NULL) erreur();
    struct parametre *tparam;
    tparam=malloc(sizeof(struct parametre)*NA);
    if(tparam==NULL) erreur();
    for(int i=0;i<NA;i++){
        (tparam[i]).num=i;
        (tparam[i]).NBF=NF;
        if(pthread_create(&(tid[i]),NULL,afficher,(void *)(&(tparam[i])))==-1) erreur();
    }
    int *retour;
    for(int i=0;i<NA;i++){
        if(pthread_join(tid[i],(void *)(&retour))!=0) erreur();
        printf("Valeur retournee par le fils %lu : %d\n",tid[i],*retour);
        free(retour);
    }
    free(tid);
    free(tparam);
    return 0;
}