#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

//Igor Malfacini - 20.2.8035
//João Vitor Tacchi - 20.2.8088

typedef struct Funcionario{
    int cod;
    char nome[40];
    char cpf[15];
    char nascimento[15];
    double salario;
} Funcionario;

typedef struct KeyArray{
    int cod;
    int RRN;
} KeyArray;

void escreve(Funcionario *funcionario, FILE *out){
    fwrite(&funcionario->cod, sizeof(int), 1, out);
    fwrite(funcionario->nome, sizeof(char), sizeof(funcionario->nome), out);
    fwrite(funcionario->cpf, sizeof(char), sizeof(funcionario->cpf), out);
    fwrite(funcionario->nascimento, sizeof(char), sizeof(funcionario->nascimento), out);
    fwrite(&funcionario->salario, sizeof(double), 1, out);
}

Funcionario *leitura(FILE *arq){
    Funcionario *funcionario = (Funcionario*) malloc(sizeof(Funcionario));
    if (fread(&funcionario->cod, sizeof(int), 1, arq) <= 0) {
        free(funcionario);
        return NULL;
    }

    fread(funcionario->nome, sizeof(char), sizeof(funcionario->nome), arq);
    fread(funcionario->cpf, sizeof(char), sizeof(funcionario->cpf), arq);
    fread(funcionario->nascimento, sizeof(char), sizeof(funcionario->nascimento), arq);
    fread(&funcionario->salario, sizeof(double), 1, arq);

    return funcionario;
}

Funcionario *buscaBinaria(int cod, FILE *arq){
    int esquerda = 0, direita = 100;
    int cont = 1; //Contador inicializa como 1 devido a comparacao inicial com o termo do meio

    do{
        int meio = (esquerda + direita)/2;
        fseek(arq, meio*sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = leitura(arq);

        if(cod == funcionario->cod){
            printf("\nNumero de comparacoes da busca binaria = %d", cont);
            return funcionario;
        }else if(funcionario->cod < cod){
            esquerda = meio + 1;
        }else{
            direita = meio - 1;
        }
        cont++;
    }while(esquerda <= direita);
    return NULL;
}

Funcionario *buscaSequencial(FILE *arq, int cod) {
    Funcionario *func = (Funcionario *) malloc(sizeof(Funcionario));
    int i = 0;
    int cont = 0;

    do {
        fseek(arq, i, SEEK_SET);

        if (0 >= fread(&func->cod, sizeof(int), 1, arq)) {
            free(func);
            return NULL;
        }

        fread(func->nome, sizeof(char), sizeof(func->nome), arq);
        fread(func->cpf, sizeof(char), sizeof(func->cpf), arq);
        fread(func->nascimento, sizeof(char), sizeof(func->nascimento), arq);
        fread(&func->salario, sizeof(double), 1, arq);
        i+=sizeof(Funcionario);
        cont++;
    } while (func->cod != cod || i<100);
    printf("\nNumero de comparacoes da busca sequencial = %d", cont);
    return func;
}

//a funcao preenche era inicialmente para ser colocada no main, mas precisei colocar como uma funcao para funcionar
void preenche(FILE *arq){
    srand(time(NULL));
    for(int i=1; i<=100; i++){
        Funcionario funcs;
        funcs.cod = rand() % 100;
        sprintf(funcs.nome, "Pedro %d", i);
        sprintf(funcs.cpf, "111-111-111.11");
        sprintf(funcs.nascimento, "03/03/2022");
        funcs.salario = i*1000;

        fseek(arq, (i-1)*sizeof(Funcionario), SEEK_SET);

        escreve(&funcs, arq);
    }
}

int tamanho_arquivo(FILE *arq) {
    fseek(arq, 0, SEEK_END);
    int tam = trunc(ftell(arq) / sizeof(Funcionario));
    return tam;
}

void insertion_sort(FILE *arq, int tam) {
    rewind(arq); //posiciona cursor no inicio do arquivo
    int i;
    for (int j = 2; j <= tam; j++) {
        fseek(arq, (j-1) * sizeof(Funcionario), SEEK_SET);
        Funcionario *fj = le(arq);
        i = j - 1;
        fseek(arq, (i-1) * sizeof(Funcionario), SEEK_SET);
        Funcionario *fi = le(arq);
        while ((i > 0) && (fi->cod > fj->cod)) {
            fseek(arq, i * sizeof(Funcionario), SEEK_SET);
            escreve(fi, arq);
            i = i - 1;
            fseek(arq, (i-1) * sizeof(Funcionario), SEEK_SET);
            fi = le(arq);
        }
        fseek(arq, (i) * sizeof(Funcionario), SEEK_SET);
        escreve(fj, arq);
    }
    fflush(arq);
}

void key_sorting(FILE *arq){
    fseek(arq, 0, SEEK_SET);
    KeyArray vetorKey[100];
    for (int i = 0; i < 100; i++) {
        fseek(arq, i*sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = le(arq);
        vetorKey[i].cod = funcionario->cod;
        vetorKey[i].RRN = i;
    }

    for (int i = 0; i < 100; i++) {
        for (int j = i + 1; j < 100; j++) {
            if (vetorKey[i].cod > vetorKey[j].cod) {

                KeyArray aux = vetorKey[i];
                vetorKey[i] = vetorKey[j];
                vetorKey[j] = aux;
            }
        }
    }
    FILE *arqKeySorting;
    arqKeySorting = fopen("dbFuncionarioKeySorting.dat", "wb+");

    for (int i = 0; i < 100; i++) {
        fseek(arq, vetorKey[i].RRN * sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = le(arq);

        fseek(arq, i * sizeof(Funcionario), SEEK_SET);
        escreve(funcionario, arqKeySorting);
    }
}

//--------------------inicio funções para seleção com substituição

void selecaoSub(char *nomeArquivo, int M){
    int final = 0;
    FILE *arq;

    if((arq = fopen(nome_arq_entrada, "rb")) == NULL){
        printf("[ERRO] nao foi possivel abrir o arquivo");
    }else{
        FILE *particao //arquivo para partições

        Funcionario *memoria[M]; //array dos funcionarios em memoria
        char congelado[M]; //array de congelados

        int f = 0;
        while(!feof(arq) && f < M){
            memoria[f] = leitura(arq);
            f++;
        }
    }

    
}

//--------------------fim funções para seleção com substituição



int main() {
    FILE *arq;
    arq = fopen("buscaFuncionario.dat", "wb+");
    Funcionario *funcionario;
    int loop = 0, escolha;

    if(arq == NULL){
        printf("Nao foi possivel abrir o arquivo");
        return 1;
    }

    preenche(arq);

    do{
        printf("----------------------\n");
        printf("Digite a opcao de operacao\n1-Busca Sequencial\n2-Busca Binaria\n3-Insertion Sort\n");
        scanf("%d", &escolha);
        switch(escolha){
        case 1: ;
            clock_t tempoSequencialI = clock();
            funcionario = buscaSequencial(arq, 20);
            clock_t tempoSequencialF = clock();
            double tempoFuncaoSequencial = ((double)tempoSequencialF - tempoSequencialI)/CLOCKS_PER_SEC;

            if(funcionario == NULL){
                printf("\nFuncionario nao encontrado");
            }else{
            printf("\nFuncionario encontrado\n");
            printf("\nCodigo: %d\n", funcionario->cod);
            printf("\nNome: %s", funcionario->nome);
            printf("\nSalario: %.2lf", funcionario->salario);
            printf("\nCPF: %s", funcionario->cpf);
            printf("\nNascimento: %s", funcionario->nascimento);
            }

            free(funcionario);

            printf("\n\nO tempo gasto na funcao de busca sequencial foi %f", tempoFuncaoSequencial);
            break;

        case 2: ;
            clock_t tempoBinarioI = clock();
            funcionario = buscaBinaria(20, arq);
            clock_t tempoBinarioF = clock();
            double tempoFuncaoBinario = ((double)tempoBinarioF - tempoBinarioI)/CLOCKS_PER_SEC;

            if(funcionario == NULL){
                printf("\nFuncionario nao encontrado");
            }else{
            printf("\nFuncionario encontrado\n");
            printf("\nCodigo: %d\n", funcionario->cod);
            printf("\nNome: %s", funcionario->nome);
            printf("\nSalario: %.2lf", funcionario->salario);
            printf("\nCPF: %s", funcionario->cpf);
            printf("\nNascimento: %s", funcionario->nascimento);
            }

            free(funcionario);

            printf("\n\nO tempo gasto na funcao de busca binaria foi %f", tempoFuncaoBinario);
            break;

        case 3: ;
            clock_t tempoOrdenacaoI = clock();
            insertion_sort(arq, tamanho_arquivo(arq));
            clock_t tempoOrdenacaoF = clock();
            double tempoOrdenacao = ((double)tempoOrdenacaoF - tempoOrdenacaoI)/CLOCKS_PER_SEC;
            printf("\nO tempo gasto na funcao de ordenacao foi %f", tempoOrdenacao);
            break;

        default:
            printf("Número invalido");
        }

        printf("\n----------------------");

        printf("\n\nDeseja continuar operando?\nSe sim, digite 0\nSe nao digite 1\n");
        scanf("%d", &loop);

        system("cls");

    }while(loop == 0);



}
