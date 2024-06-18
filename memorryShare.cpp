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
void generator(int numbers[STACK_SIZE]);
void consume(TNoABP **tree, int numbers[STACK_SIZE]);
void insereABP (TNoABP **r, int key);
int achaMaiorEsq(TNoABP **no1);
void removeABP (TNoABP **r, int key);
void printABP (TNoABP *raiz);
void recebeMSG(TNoABP **raiz, int cont);
TNoABP retornarMenor(TNoABP **raiz,int index);





int main(){
    pid_t pid;
    int numbers_shared = shmget(IPC_PRIVATE,sizeof(int)*STACK_SIZE,SHM_R|SHM_W|IPC_CREAT);
    int *numbers;
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

        generator(numbers);
    }else{
        sleep(3);
        consume(&tree,numbers);
    }
}
 

void insert(int numbers[STACK_SIZE], int value,int index){   
    if(numbers[index]==0){
        numbers[index]=value;
        //printf("\nStacked %d in %d\n",value,index);
    }
    
}

void drop(TNoABP **tree,int numbers[STACK_SIZE]){
    int value;
    int i=STACK_SIZE-1;
    int cont = 0;
    while(true){
        if(i>-1){
            if(numbers[i]!=0){
                //value = numbers[i];
                //printf("\nDroped %d from the stack",value);
                insereABP(tree,numbers[i]);
                cont++;
                //printf("\nInserted %d in the tree ",numbers[i]);
                numbers[i] = 0;
                i--;
            }else{
                i--;
            }
        }else{
            i=STACK_SIZE-1;
            recebeMSG(tree,cont);
        }
    }
    //printABP(*tree);
    

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

void generator(int numbers[STACK_SIZE]){
    int cont = 0;
    bool notInserted = false;
    int numberInserteds[STACK_SIZE];
    while(true){
        int value = 0;
        value = rand()%200;
        notInserted=true;
        if(numbers[STACK_SIZE-1]==0){
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
        }else{
            while(numbers[0]!=0);
            sleep(10);
            for(int i=0;i<STACK_SIZE;i++)
                numberInserteds[i]=0;
            cont=0;
            //printf("oi");
        }
    }
    
}

void consume(TNoABP **tree, int numbers[STACK_SIZE]){
    if (numbers[0] != 0){
            drop(tree,numbers);
        }else{
            printf("\nStack is empty right now");
        }
    
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


void recebeMSG(TNoABP **raiz, int cont){
    //while(true){
        int start;
        FILE *fptr;
        int ready=0;
        while(ready!=1){
            fptr=fopen("request.txt","r");
            if(fptr==NULL){
                printf("ERRO");
                exit(1);
            }
            fscanf(fptr,"%d",&ready);
            sleep(3);
            fclose(fptr);
        }
        fptr=fopen("response.txt","w");
        TNoABP no = retornarMenor(raiz,cont);
        int primo = no.chave;
        removeABP(raiz,primo);
        fprintf(fptr,"%d",primo);
        fclose(fptr);
        fptr=fopen("request.txt","w");
        fclose(fptr);

        //break;
    //}
}   


TNoABP retornarMenor(TNoABP **raiz,int index){
    
    TNoABP *aux = (*raiz);
    
    for(int i=1;i<index;i++){
        if(aux->esq==NULL)
            return (*aux);
        if(aux->chave<aux->esq->chave){
            return (*aux);
        }else{
            aux=(*raiz)->esq;
        }

    }
}