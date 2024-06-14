#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stack>
#include <set>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <set>
#include <sys/shm.h>
#include <sys/ipc.h>



#define STACK_SIZE 10

typedef struct TNoABP
{
    int chave;
    struct TNoABP *esq;
    struct TNoABP *dir;
}TNoABP;

void insert(int numbers[STACK_SIZE], int value,int index);
void drop(TNoABP **tree,int numbers[STACK_SIZE]);
bool isPrime(int number);
bool generator(int numbers[STACK_SIZE]);
bool consume(TNoABP **tree, int numbers[STACK_SIZE]);
void insereABP (TNoABP **r, int key);
int achaMaiorEsq(TNoABP **no1);
void removeABP (TNoABP **r, int key);
void printABP (TNoABP *raiz);





int main(){
    pid_t pid;
    int numbers_shared = shmget(IPC_PRIVATE,sizeof(int)*STACK_SIZE,SHM_R|SHM_W|IPC_CREAT);
    int *numbers;
    bool ready_shared = shmget(IPC_PRIVATE,sizeof(bool),SHM_R|SHM_W|IPC_CREAT);
    bool *ready = (bool*)shmat(numbers_shared,NULL,0);
    numbers = (int*)shmat(numbers_shared,NULL,0);
    TNoABP *tree = NULL;
    pid= fork();
    if(pid<0){
        printf("Inicio Falhou");
    }
    else if(pid==0){
        numbers = (int*)shmat(numbers_shared,NULL,0);
        for(int i=0;i<STACK_SIZE-1;i++)
            numbers[i]=0;
        *ready = (bool*)shmat(numbers_shared,NULL,0);
        *ready=false;
        *ready=generator(numbers);
    }else{
        while(*ready!=true);
        *ready=consume(&tree,numbers);
    }
}
 

void insert(int numbers[STACK_SIZE], int value,int index){   
    if(numbers[index]==0){
        numbers[index]=value;
        printf("\nStacked %d in %d\n",value,index);
    }
    
}

void drop(TNoABP **tree,int numbers[STACK_SIZE]){
    int value;
    for(int i=0;i<STACK_SIZE;i++){
        if(numbers[i]!=0){
            //value = numbers[i];
            //printf("\nDroped %d from the stack",value);
            insereABP(tree,numbers[i]);
            printf("\nInserted %d in the tree %d",numbers[i],i);
            numbers[i] = 0;
        }else{
            printf("\noi");
        }
    }
    printABP(*tree);

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

bool generator(int numbers[STACK_SIZE]){
    int cont = 0;
    bool notInserted = false;
    int numberInserteds[STACK_SIZE];
    while(cont<STACK_SIZE){
        int value = 0;
        value = rand()%200;
        notInserted=true;
        if(isPrime(value)){
            for(int i=0;i<cont+1;i++){
                if(value==numberInserteds[i]){
                    notInserted=false;
                    break;
                }
            }
            if(notInserted){
            insert(numbers,value,cont);
            numberInserteds[cont]=value;
            cont++;
            }
        }
    }
    return true;
}

bool consume(TNoABP **tree, int numbers[STACK_SIZE]){
    if (numbers[0] != 0){
            drop(tree,numbers);
        }else{
            printf("\nStack is empty right now");
        }
    return true;
}


void insereABP (TNoABP **r, int key) 
{
    if (*r == NULL)
    {
        TNoABP *r2;
        r2 = (TNoABP*)malloc(sizeof(TNoABP));
        if(r2==NULL) //verifica se não houve problema na alocação da árvore.
        {
            printf("\nSO nao liberou memoria!"); 
            return;
        }
        r2->chave = key;
        r2->esq = NULL;
        r2->dir = NULL;
        *r = r2;
    }
    else if ((*r)->chave > key)
        insereABP(&(*r)->esq, key);
    else
        insereABP(&(*r)->dir, key);
}

int achaMaiorEsq(TNoABP **no1)
{
    
    int x;
    TNoABP *r3;

    if((*no1)->dir != NULL)
        return(achaMaiorEsq(&(*no1)->dir));
    else
    {
        r3 = *no1;
        x = (*no1)->chave;
        *no1 = (*no1)->esq;
        free(r3);
        r3 = NULL;
        return(x);
    }
}




void removeABP (TNoABP **r, int key)
{
    TNoABP *r2;
        if(*r != NULL)
        {
            if((*r)->chave == key)
            {
                r2 = *r;
                if((*r)->esq == NULL)
                {
                    *r = (*r)->dir;
                    free(r2);
                    r2 = NULL;
                }
                
                else if((*r)->dir == NULL)
                {
                    *r = (*r)->esq;
                    free(r2);
                    r2 = NULL;  
                }
                
                else ((*r)->chave = achaMaiorEsq(&(*r)->esq));
            }
        else if((*r)->chave < key)
            removeABP(&(*r)->dir,key);
        else 
            removeABP(&(*r)->esq,key);
        }
}


void printABP (TNoABP *raiz) //travessia em ordem
{
    if (raiz==NULL){
        return;
    } 
    printABP (raiz->esq);
    printf("\n%d", raiz->chave);
    printABP (raiz->dir);
}