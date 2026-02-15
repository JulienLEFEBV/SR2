#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>

void erreur(void)
{
    perror("Erreur ");
    exit(1);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr,"Usage: %s [Nb activités parallèles] [Nb affichage]\n",argv[0]);
        exit(1);
    }
    int NA,NF;
    pid_t pid;
    NA=atoi(argv[1]);
    NF=atoi(argv[2]);
    for(int i=0;i<NA;i++){
        switch(fork()){
            case -1: erreur();
            case 0: pid=getpid();
                    for(int j=0;j<NF;j++) printf("Activité rang %d : identifiant = %d\n",i,pid);
                    exit(i);
        }
    }
    pid_t fils;
    int retour;
    while(((fils=wait(&retour))!=-1)&&(errno!=EINTR)) if(fils!=-1) printf("Valeur retournee par le fils %d : %d\n",fils,WEXITSTATUS(retour));
    return 0;
}