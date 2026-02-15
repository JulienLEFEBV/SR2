#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

int tube[2],NBS,num_fils=0,cpt[2];
pid_t pid;
time_t t;

void erreur(void)
{
    perror("Erreur :");
    exit(1);
}

void envoi(int s){
    printf("    Capteur %d (%d): Valeur %d écrite dans le tube à %s",num_fils,pid,cpt[1],(time(&t), ctime(&t)));
    while(write(tube[1],cpt,2*sizeof(int))==-1);
    if(alarm(NBS)==-1) erreur();
}

void fin(int s){
    printf("    Capteur %d (%d): Terminer \n",num_fils,pid);
    close(tube[1]);
    exit(0);
}

int main(int argc, char*argv[])
{
    if (argc!=4){
        fprintf(stderr,"Usage: %s [nb capteur] [nb limte] [temps entre chaque envoi]\n",argv[0]);
        exit(1);
    }
    int NBF=atoi(argv[1]),NBMAX=atoi(argv[2]);
    NBS=atoi(argv[3]);
    pid_t *fils;
    fils=malloc(NBF*sizeof(pid_t));
    if(pipe(tube)==-1) erreur();
    pid=getpid();
    for(int i=0; i<NBF; i++)
    {
        switch(fils[i]=fork())
        {
            case -1: erreur();
            case 0: close(tube[0]);
                    free(fils);
                    num_fils=i;
                    cpt[0]=i;
                    cpt[1]=0;
                    sigset_t mask,mask_fin;
                    sigemptyset(&mask);
                    if(sigprocmask(SIG_SETMASK,&mask,NULL)==-1) erreur();
                    struct sigaction action_envoi,action_fin;
                    action_envoi.sa_handler=envoi;
                    action_envoi.sa_mask=mask;
                    action_envoi.sa_flags=0;
                    if(sigaction(SIGALRM,&action_envoi,NULL)==-1) erreur();
                    sigemptyset(&mask_fin);
                    sigaddset(&mask_fin,SIGALRM);
                    action_fin.sa_handler=fin;
                    action_fin.sa_mask=mask;
                    action_fin.sa_flags=0;
                    if(sigaction(SIGUSR1,&action_fin,NULL)==-1) erreur();
                    if(alarm(NBS)==-1) erreur();
                    pid=getpid();
                    char c;
                    while (1)
                    {
                        while(read(0,&c,sizeof(char))==-1) if(errno!=EINTR) erreur();
                        cpt[1]++;
                    }
            default: break;
        }
    }
    close(tube[1]);
    while(read(tube[0],cpt,2*sizeof(int))!=0){
        printf("Père (%d) - Capteur %d : nombre de vehicule = %d\n",pid,cpt[0],cpt[1]);
        if (cpt[1]>=NBMAX){
            printf("Père (%d): Signale au capteur %d (%d) sa fin\n",pid,cpt[0],fils[cpt[0]]);
            while(kill(fils[cpt[0]],SIGUSR1)==-1);
        }
    }
    close(tube[0]);
    for(int i=0; i<NBF; i++) while(wait(NULL)==-1) if(errno!=EINTR) erreur();
    printf("Père (%d): Je me termine\n",pid);
    free(fils);
    return 0;
}
