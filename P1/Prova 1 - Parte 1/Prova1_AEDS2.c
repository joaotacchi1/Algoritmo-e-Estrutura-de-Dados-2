#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct Funcionario{
    int cod;
    char nome[40];
    char cpf[15];
    char nascimento[15];
    double salario;
}Func;

void escreve(Func *funcionario, FILE *out){
    fwrite(&funcionario->cod, sizeof(int), 1, out);
    fwrite(funcionario->nome, sizeof(char), sizeof(funcionario->nome), out);
    fwrite(funcionario->cpf, sizeof(char), sizeof(funcionario->cpf), out);
    fwrite(funcionario->nascimento, sizeof(char), sizeof(funcionario->nascimento), out);
    fwrite(&funcionario->salario, sizeof(double), 1, out);
}

Func *leitura(FILE *dentro){
    Func *funcionario = (Func*) malloc(sizeof(Func));
    if (fread(&funcionario->cod, sizeof(int), 1, dentro) <= 0) {
        free(funcionario);
        return NULL;
    }

    fread(funcionario->nome, sizeof(char), sizeof(funcionario->nome), dentro);
    fread(funcionario->cpf, sizeof(char), sizeof(funcionario->cpf), dentro);
    fread(funcionario->nascimento, sizeof(char), sizeof(funcionario->nascimento), dentro);
    fread(&funcionario->salario, sizeof(double), 1, dentro);

    return funcionario;
}

Func *buscaBinaria(int cod, FILE *arq){
    int esquerda = 0, direita = 99;
    int cont = 1; //Contador inicializa como 1 devido a comparação inicial com o termo do meio

    printf("-----------------------\nBusca Binaria\n");
    printf("Numero de comparacoes da busca binaria= %d\n", cont);
    do{
        int meio = (esquerda + direita)/2;
        fseek(arq, meio*sizeof(Func), SEEK_SET);
        Func *funcionario = leitura(arq);

        if(cod == funcionario->cod){
            return funcionario;
        }else if(funcionario->cod < cod){
            esquerda = meio + 1;
        }else{
            direita = meio -  1;
        }
        cont++;
        printf("Numero de comparacoes da busca binaria= %d\n", cont);
    }while(esquerda <= direita);

    return NULL;
}

//a função preenche era inicialmente para ser colocada no main, mas precisei colocar como uma função para funcionar
void preenche(FILE *arq){
    //srand(time(NULL));
    for(int i=1; i<=100; i++){
        Func funcs;
        funcs.cod =i; //rand() % 100;
        sprintf(funcs.nome, "Pedro %d", i);
        sprintf(funcs.cpf, "111-111-111.11");
        sprintf(funcs.nascimento, "3/03/2022");
        funcs.salario = i*1000;

        fseek(arq, (i-1)*sizeof(Func), SEEK_SET);

        escreve(&funcs, arq);
    }
}

int main()
{
    //Calculo de tempo gasto na função de busca binaria
    clock_t tempo;
    double tempoFuncao;

    FILE *arq;
    arq = fopen("buscaFuncionario.dat", "wb+");
    Func *funcionario;

    if(arq == NULL){
        printf("Não foi possivel abrir o arquivo");
        return 1;
    }

    preenche(arq);
    tempo = clock();
    funcionario = buscaBinaria(50, arq);
    tempo = clock() - tempo;

    tempoFuncao = ((double)tempo)/CLOCKS_PER_SEC;

    if(funcionario == NULL){
        printf("\nFuncionario nao encontrado");
    }else{
        printf("\nFuncionario encontrado\n");
        printf("Codigo: %d\n", funcionario->cod);
        printf("Nome: %s", funcionario->nome);
    }

    printf("\n\nO tempo gasto na funcao de busca binaria foi %f", tempoFuncao);
    printf("\n-----------------------");

    free(funcionario);
    fclose(arq);
}
