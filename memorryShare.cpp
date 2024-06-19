#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stack>
#include <set>
#include <cstdlib>
#include <ctime>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/ipc.h>



#define STACK_SIZE 10




bool isPrime(int n);
void generator(int numbers[]);
void consume(int arvore[], int numbers[]);
void inserirArvore(int arvore[],int value,int index);
void recebeMSG(int arvore[]);
int acharMenorArvore(int arvore[],int index);





int main(){
    pid_t pid;
    int numbers_shared = shmget(IPC_PRIVATE,sizeof(int)*STACK_SIZE,SHM_R|SHM_W|IPC_CREAT);
    int *numbers;
    numbers = (int*)shmat(numbers_shared,NULL,0);
    int arvore[STACK_SIZE];
    for(int i=0;i<STACK_SIZE-1;i++)
        arvore[i]=-2;//posicao sem lixo
    pid= fork();
    if(pid<0){
        printf("Inicio Falhou");
    }
    else if(pid==0){
        numbers = (int*)shmat(numbers_shared,NULL,0);
        numbers[0]=0;  // permitir acesso a pilha
        numbers[1]=-2; //permitir inserir
        generator(numbers);
    }else{
        consume(arvore,numbers);
    }
    return 0;
}
 

bool isPrime(int n) {
  if (n <= 1) return false;  // 1 e números menores que 1 não são primos
  if (n <= 3) return true;  // 2 e 3 são primos
  if (n % 2 == 0 || n % 3 == 0) return false;  // Números divisíveis por 2 ou 3 não são primos

  int i;
  for (i = 5; i * i <= n; i += 6) {
    if (n % i == 0 || n % (i + 2) == 0) return true;  // Testar divisibilidade por primos ímpares até a raiz quadrada de n
  }
  return 1;  // Se nenhum divisor for encontrado, n é primo
}

void generator(int numbers[STACK_SIZE]){
    int cont = 0;
    while(true){
        if(numbers[0]==0){
            int value = 0;
            value = rand()%200;
            if(isPrime(value)){
                    int i=1;
                    while(i>0){
                        if(numbers[i]==-2){ //fim da pilha  
                            numbers[i]=value;
                            numbers[i+1]=-2;//novo fim da pilha
                            i=0;
                        }else{
                            i++;
                        }
                    }
                    numbers[0]=-1; //permitir arvore inserir
                    cont++;
            }
        }
    }
    
}

void consume(int arvore[], int numbers[]){
    int cont = 0;
    int i=STACK_SIZE-1;
    while(true){
        if (numbers[0] == -1){
                if(i>0){
                    if(numbers[i]==-2){
                        int index= cont-1;
                        inserirArvore(arvore,numbers[i-1],index);
                        cont++;
                        numbers[i-1] = -2; //numero fim da pilha
                        numbers[i]=-3; // depois do fim da fila e vazio
                        numbers[0]=0; // permitir pilha inserir
                        recebeMSG(arvore);
                    }else{
                        i--;
                    }
                }else{
                    i=STACK_SIZE-1;
                }
        }else{
            //printf("\nStack is running");
        }
    }
    
}

void inserirArvore(int arvore[],int value,int index){
    if(index==-1){
        arvore[index+1]=value;
        return;
    }else{
        if(arvore[index]==-2){
            arvore[index]=value;
        }
        if(arvore[index]>value){
            int proximoFilho = (index*2)+1;
            if(proximoFilho<STACK_SIZE)
                inserirArvore(arvore,value,proximoFilho);
        }else{
            int proximoFilho=0;
            if(index==0){
                proximoFilho=2;
            }
            else{proximoFilho=(index*2);}
            if(proximoFilho<STACK_SIZE)
                inserirArvore(arvore,value,proximoFilho);
        }
    }
}




void recebeMSG(int arvore[]){
        FILE *fptr;
        int ready=0;
       
        fptr=fopen("request.txt","r");
        if(fptr==NULL){
            printf("ERRO");
            exit(1);
        }
        fscanf(fptr,"%d",&ready);
        sleep(3);
        fclose(fptr);
        if(ready==1){
            fptr=fopen("response.txt","w");
            int primo = acharMenorArvore(arvore,0);
            fprintf(fptr,"%d",primo);
            fclose(fptr);
            fptr=fopen("request.txt","w");
            fclose(fptr);
        }
        
        
    
} 

int acharMenorArvore(int arvore[],int index){
    if(arvore[(index*2)+1]==-2&&(index*2)+1<STACK_SIZE){
        return arvore[index];
        arvore[index]=-2;
    }
    else{
        if((index*2)+1<STACK_SIZE)
            return acharMenorArvore(arvore,(index*2)+1);
    }
}