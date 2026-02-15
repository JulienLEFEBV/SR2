#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>

#define NBMAX 20

char ** tab;
int *depose;
int i_depot, i_conso, NBF_prod, NBF_conso,taille_tab;

void erreur(void)
{
    perror("Erreur ");
    exit(1);
}

void produire(int num,int appel_num,pthread_t id){
    char msg[NBMAX];
    sprintf(msg,"Bonjour %2d prod %2d",appel_num,num);
    printf("Prod %d (%lu): Message depose = %s\n",num,id,msg);
    tab[i_depot]=strcpy(tab[i_depot],msg);
    i_depot=(i_depot+1)%taille_tab;
}

void consommer(int num,pthread_t id){
    char *msg;
    if((msg=malloc(sizeof(char)*NBMAX))==NULL) erreur();
    msg=strcpy(msg,tab[i_conso]);
    printf("Conso %d (%lu): Message lu = %s\n",num,id,msg);
    for(int i;i<NBMAX;i++) tab[i_conso][i]='x';
    i_conso=(i_conso+1)%taille_tab;
    free(msg);
}

void * consommateur(void* arg){
    int *num=(int *) arg;
    pthread_t id=pthread_self();
    for (int i=0;i<NBF_conso;i++) consommer(*num,id);
    return NULL;
}

void * producteur(void* arg){
    int *num=(int *) arg;
    pthread_t id=pthread_self();
    for (int i=0;i<NBF_prod;i++) produire(*num,i,id);
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
    pthread_t *tab_id;
    if((tab_id=malloc((NBP+NBC)*sizeof(pthread_t)))==NULL) erreur();
    int *tab_i;
    if((tab_i=malloc((NBP+NBC)*sizeof(int)))==NULL) erreur();
    if((depose=malloc(taille_tab*sizeof(int)))==NULL) erreur();
    if((tab=malloc(taille_tab*sizeof(char *)))==NULL) erreur();
    for(int i=0;i<taille_tab;i++) if((tab[i]=malloc(NBMAX*sizeof(char)))==NULL) erreur();
    for(int i; i<NBP;i++){
        tab_i[i]=i+1;
        pthread_create(&(tab_id[i]),NULL,producteur,(void *)&(tab_i[i]));
    }
    for(int i; i<NBC;i++){
        tab_i[i+NBP]=i+1;
        pthread_create(&(tab_id[i+NBP]),NULL,consommateur,(void *)&(tab_i[i+NBP]));
    }
    for(int i;i<(NBC+NBP);i++) pthread_join(tab_id[i],NULL);
    free(tab_i);
    for(int i=0;i<taille_tab;i++) free(tab[i]);
    free(tab);
    free(tab_id);
    return 0;
}