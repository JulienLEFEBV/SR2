#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>


int NBF,solde;

void erreur(void)
{
    perror("Erreur ");
    exit(1);
}

void* crediter(void * arg){
    int credit;
    pthread_t id=pthread_self();
    for(int i=0;i<NBF;i++){
        credit=rand()%100;
        solde+=credit;
        printf("Credit %lu: credit(%d) => solde=%d\n",id,credit,solde);
    }
    return NULL;
}

void* debiter(void * arg){
    int debit;
    pthread_t id=pthread_self();
    for(int i=0;i<NBF;i++){
        debit=rand()%100;
        if(solde<debit) printf("    Debit %lu: Si debit %d alors solde(%d) sera debiteur\n",id,debit,solde);
        solde-=debit;
        printf("Debit %lu: debit(%d) => solde=%d\n",id,debit,solde);
    }
    return NULL;
}

int main(int argc, char *argv[]){
    if (argc!=4){
        fprintf(stderr,"Usage : %s [NB thread] [solde initial] [NB operation]\n",argv[0]);
        exit(1);
    }
    int NBT=atoi(argv[1]);
    solde=atoi(argv[2]);
    NBF=atoi(argv[3]);
    pthread_t *tid;
    tid=malloc(sizeof(pthread_t)*NBT);
    for(int i=0;i<NBT;i++){
        if (i%2==0) if(pthread_create(&(tid[i]),NULL,crediter,NULL)==-1) erreur();
        if (i%2==1) if(pthread_create(&(tid[i]),NULL,debiter,NULL)==-1) erreur();
    }
    for(int i=0;i<NBT;i++) if (pthread_join(tid[i],NULL)!=0) erreur();
    printf("Solde = %d\n",solde);
    return 0;
}