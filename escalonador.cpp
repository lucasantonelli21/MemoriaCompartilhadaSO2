#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>



#define qntdFilas 8
#define tamanhoFilas 10


typedef struct Processo{
    int tempo;
    int id;
}Processo;



typedef struct Fila{
    int prioridade;
    Processo processos[tamanhoFilas];
}Fila;


typedef struct Escalonador
{
    Fila *manager;
    Fila filas[qntdFilas];
}Escalonador;



void inicializarFilas(Escalonador *escalonador);
int solicitarProcesso();
void escalonar(Escalonador *escalonar);
void gerenciar(Escalonador *gerenciar);
bool isPrime(int number);
void arrumarFilas(Fila **manager);




int main(){
    key_t key = ftok("/tmp", 'S');
    int escalonador_shared= shmget(IPC_PRIVATE,sizeof(struct Escalonador),SHM_R|SHM_W|IPC_CREAT);
    Escalonador *escalonador;
    escalonador=(Escalonador*)shmat(escalonador_shared,NULL,0);
    pid_t pid;
    pid=fork();
    if(pid<0){
        printf("Inicio Falhou");
    }
    else if(pid==0){
        escalonador=(Escalonador*)shmat(escalonador_shared,NULL,0);
        inicializarFilas(escalonador);
        escalonar(escalonador);
    }else{
        gerenciar(escalonador);
    }
   
}




void inicializarFilas(Escalonador *escalonador){
    Fila filaAux;
    Processo processo;
    for(int i=0;i<qntdFilas;i++){
        filaAux.prioridade=i+1;
        processo.id=-1;
        processo.tempo=-1;
        for(int j=0;j<tamanhoFilas-1;j++)
            filaAux.processos[j]=processo;
        escalonador->filas[i]=filaAux;

    }
    

}



void escalonar(Escalonador *escalonador){
    while(true){
        int primo = solicitarProcesso();
        Processo processo;
        processo.id=primo;
        processo.tempo=primo;
        for(int i=0;i<qntdFilas-1;i++){
            Fila *aux = escalonador->filas;
            int salvar[2];
            for(int j=0;j<tamanhoFilas-1;j++){
                Processo *processosAtual=aux[i].processos;
                if(processosAtual[j].id==-1){
                    processosAtual[j]=processo;
                    salvar[1]=j;
                    salvar[0]=i;
                    j=tamanhoFilas;
                    i=qntdFilas;
                }
                
                    printf("%d\n",aux[salvar[0]].processos[salvar[j]].id);
            }
        }
    }
}


void gerenciar(Escalonador *gerenciar){
    while (true)
    {
        gerenciar->manager=gerenciar->filas;
        Fila **manager = &gerenciar->manager;
        for(int i=0;i<qntdFilas-1;i++){
            if((manager)[i]->processos->id!=-1){
                sleep((*manager)->processos->tempo);
                printf("\nprocesso terminou");
                i=qntdFilas;
                arrumarFilas(manager);
            }
        }
    }
    
}




bool isPrime(int number){
    if(number <= 1){
        return false;
    }else{
        for(int i=2;i*i<=number;i++){
            if(number%i==0)
                return false;
        }
        return true;
    }
}




int solicitarProcesso(){
    FILE *fptr;
    int primo=0;
    fptr=fopen("request.txt","a");
    if(fptr==NULL){
        printf("ERRO");
        exit(1);
    }
    fprintf(fptr,"%d",1);
    fclose(fptr);
    sleep(3);
    while(isPrime(primo)!=true){
        fptr=fopen("response.txt","r");
        fscanf(fptr,"%d",&primo);
        fclose(fptr);
    }
    return primo;

}


void arrumarFilas(Fila **manager){
    for(int i=0;i<qntdFilas-1;i++){
        for(int j=0;j<tamanhoFilas-1;j++){
            if(manager[i]->processos[j].id==-1)
                return;
            if(j+1<tamanhoFilas)
                manager[i]->processos[j]=manager[i]->processos[j+1];
        }
        manager[i]->processos[tamanhoFilas-1]=manager[i+1]->processos[0];
    }
    
}