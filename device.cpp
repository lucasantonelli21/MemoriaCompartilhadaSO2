#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h> 
 





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

 int main(){
    while(true){
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
        printf("%d\n",primo);
        fptr=fopen("response.txt","w");
        fclose(fptr);
    }
    return 0;
 }
 