#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

int nb_voiture_1=0,nb_voiture_2=0,NB;
pid_t pid;

void traiter_signal(int signal){
    if(signal==SIGUSR1) 
    {
        nb_voiture_1+=NB;
        printf("Père (%d) - Capteur 0 : nombre de vehicule = %d\n",pid,nb_voiture_1);
    }
    if(signal==SIGUSR2){
        nb_voiture_2+=NB;
        printf("Père (%d) - Capteur 1 : nombre de vehicule = %d\n",pid,nb_voiture_2);
    }
}

void erreur(void)
{
    perror("Erreur :");
    exit(1);
}

int main(int argc, char*argv[])
{
    if (argc!=3){
        fprintf(stderr,"Nombre d'argument invalide \n");
        exit(1);
    }
    int NBL=atoi(argv[1]);
    NB=atoi(argv[2]);
    sigset_t mask, mask_attente;
    sigemptyset(&mask_attente);
    if(sigprocmask(SIG_SETMASK,&mask_attente,NULL)==-1) erreur();
    sigemptyset(&mask);
    sigaddset(&mask,SIGUSR1);
    sigaddset(&mask,SIGUSR2);
    struct sigaction action;
    action.sa_handler=traiter_signal;
    action.sa_mask=mask;
    action.sa_flags=0;
    if(sigaction(SIGUSR1,&action,NULL)==-1) erreur();
    if(sigaction(SIGUSR2,&action,NULL)==-1) erreur();
    pid=getpid();
    for(int i=0; i<2; i++)
    {
        switch(fork())
        {
            case -1: erreur();
            case 0: int cpt=0;
                    pid_t ppid=getppid();
                    char c;
                    while (cpt<NBL)
                    {
                        read(0,&c,sizeof(char));
                        cpt++;
                        if (cpt%NB==0)
                        {
                            if(i==0) kill(ppid,SIGUSR1);
                            if(i==1) kill(ppid,SIGUSR2);
                        }
                    }
                    exit(0);
            default: break;
        }
    }
    for(int i=0; i<2; i++) while(wait(NULL)==-1) if(errno!=EINTR) erreur();
    return 0;
}