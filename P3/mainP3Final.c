#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define TAM_HASH_S 100
#define TAM_HASH_C 7

// Igor Malfacini - 20.2.8035
// João Vitor Tacchi - 20.2.8088

typedef struct Funcionario
{
    int cod;
    char nome[40];
    char cpf[15];
    char nascimento[15];
    double salario;
    struct Funcionario *prox;
} Funcionario;

typedef Funcionario *Hash;

typedef struct KeyArray
{
    int cod;
    int RRN;
} KeyArray;

typedef struct Nomes
{
    char *nome;
    struct Nomes *prox;
} Nomes;

void escreve(Funcionario *funcionario, FILE *out)
{
    fwrite(&funcionario->cod, sizeof(int), 1, out);
    fwrite(funcionario->nome, sizeof(char), sizeof(funcionario->nome), out);
    fwrite(funcionario->cpf, sizeof(char), sizeof(funcionario->cpf), out);
    fwrite(funcionario->nascimento, sizeof(char), sizeof(funcionario->nascimento), out);
    fwrite(&funcionario->salario, sizeof(double), 1, out);
}

Funcionario *leitura(FILE *arq)
{
    Funcionario *funcionario = (Funcionario *)malloc(sizeof(Funcionario));
    if (fread(&funcionario->cod, sizeof(int), 1, arq) <= 0)
    {
        free(funcionario);
        return NULL;
    }

    fread(funcionario->nome, sizeof(char), sizeof(funcionario->nome), arq);
    fread(funcionario->cpf, sizeof(char), sizeof(funcionario->cpf), arq);
    fread(funcionario->nascimento, sizeof(char), sizeof(funcionario->nascimento), arq);
    fread(&funcionario->salario, sizeof(double), 1, arq);

    return funcionario;
}

Funcionario *buscaBinaria(int cod, FILE *arq)
{
    int esquerda = 0, direita = 100;
    int cont = 1; // Contador inicializa como 1 devido a comparacao inicial com o termo do meio

    do
    {
        int meio = (esquerda + direita) / 2;
        fseek(arq, meio * sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = leitura(arq);

        if (cod == funcionario->cod)
        {
            printf("\nNumero de comparacoes da busca binaria = %d", cont);
            return funcionario;
        }
        else if (funcionario->cod < cod)
        {
            esquerda = meio + 1;
        }
        else
        {
            direita = meio - 1;
        }
        cont++;
    } while (esquerda <= direita);
    return NULL;
}

Funcionario *buscaSequencial(FILE *arq, int cod)
{
    Funcionario *func = (Funcionario *)malloc(sizeof(Funcionario));
    int i = 0;
    int cont = 0;

    do
    {
        fseek(arq, i, SEEK_SET);

        if (0 >= fread(&func->cod, sizeof(int), 1, arq))
        {
            free(func);
            return NULL;
        }

        fread(func->nome, sizeof(char), sizeof(func->nome), arq);
        fread(func->cpf, sizeof(char), sizeof(func->cpf), arq);
        fread(func->nascimento, sizeof(char), sizeof(func->nascimento), arq);
        fread(&func->salario, sizeof(double), 1, arq);
        i += sizeof(Funcionario);
        cont++;
    } while (func->cod != cod || i < 100);
    printf("\nNumero de comparacoes da busca sequencial = %d", cont);
    return func;
}

// a funcao preenche era inicialmente para ser colocada no main, mas precisei colocar como uma funcao para funcionar
void preenche(FILE *arq)
{
    srand(time(NULL));
    for (int i = 1; i <= 100; i++)
    {
        Funcionario funcs;
        funcs.cod = rand() % 100;
        sprintf(funcs.nome, "Pedro %d", i);
        sprintf(funcs.cpf, "111-111-111.11");
        sprintf(funcs.nascimento, "03/03/2022");
        funcs.salario = i * 1000;

        fseek(arq, (i - 1) * sizeof(Funcionario), SEEK_SET);

        escreve(&funcs, arq);
    }
}

int tamanho_arquivo(FILE *arq)
{
    fseek(arq, 0, SEEK_END);
    int tam = trunc(ftell(arq) / sizeof(Funcionario));
    return tam;
}

void insertion_sort(FILE *arq, int tam)
{
    rewind(arq); // posiciona cursor no inicio do arquivo
    int i;
    for (int j = 2; j <= tam; j++)
    {
        fseek(arq, (j - 1) * sizeof(Funcionario), SEEK_SET);
        Funcionario *fj = leitura(arq);
        i = j - 1;
        fseek(arq, (i - 1) * sizeof(Funcionario), SEEK_SET);
        Funcionario *fi = leitura(arq);
        while ((i > 0) && (fi->cod > fj->cod))
        {
            fseek(arq, i * sizeof(Funcionario), SEEK_SET);
            escreve(fi, arq);
            i = i - 1;
            fseek(arq, (i - 1) * sizeof(Funcionario), SEEK_SET);
            fi = leitura(arq);
        }
        fseek(arq, (i) * sizeof(Funcionario), SEEK_SET);
        escreve(fj, arq);
    }
    fflush(arq);
}

void key_sorting(FILE *arq)
{
    fseek(arq, 0, SEEK_SET);
    KeyArray vetorKey[100];
    for (int i = 0; i < 100; i++)
    {
        fseek(arq, i * sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = leitura(arq);
        vetorKey[i].cod = funcionario->cod;
        vetorKey[i].RRN = i;
    }

    for (int i = 0; i < 100; i++)
    {
        for (int j = i + 1; j < 100; j++)
        {
            if (vetorKey[i].cod > vetorKey[j].cod)
            {

                KeyArray aux = vetorKey[i];
                vetorKey[i] = vetorKey[j];
                vetorKey[j] = aux;
            }
        }
    }
    FILE *arqKeySorting;
    arqKeySorting = fopen("dbFuncionarioKeySorting.dat", "wb+");

    for (int i = 0; i < 100; i++)
    {
        fseek(arq, vetorKey[i].RRN * sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = leitura(arq);

        fseek(arqKeySorting, i * sizeof(Funcionario), SEEK_SET);
        escreve(funcionario, arqKeySorting);
    }
}
//---------------------------------------------------------

// void selecaoNatural(FILE *arq){
//     rewind(arq);

//     Funcionario* funcionarioMemoria[6];
//     Funcionario* funcionarioReservatorio[6];
//     Funcionario *funcionarioMenorCod = (Funcionario*) malloc(sizeof(Funcionario));
//     funcionarioMenorCod->cod = 999;
//     Funcionario *funcionarioMenorCodSalvo = (Funcionario*) malloc(sizeof(Funcionario));
//     int posMenorCod;
//     int ultimoFuncionarioLido = 6;

//     char filename[100];
//     sprintf(filename, "particao-%02d.dat", 1);

//     //Seleciona os 6 primeiros funcionarios
//     for (int i = 0; i < 6; i++){
//         fseek(arq,i * sizeof(Funcionario), SEEK_SET);
//         funcionarioMemoria[i] = leitura(arq);
//     }

//     //Seleciona o funcionario na memoria com menor codigo
//     for (int i = 0; i < 6; i++){
//         if (funcionarioMenorCod->cod > funcionarioMemoria[i]->cod){
//             funcionarioMenorCod = funcionarioMemoria[i];
//             posMenorCod = i;
//         }
//     }

//     //Grava em arquivo o funcionario com menor codigo
//     FILE *particao;
//     particao = fopen(filename, "wb+");
//     fseek(particao, 0, SEEK_SET);
//     escreve(funcionarioMenorCod, particao);
//     funcionarioMenorCodSalvo = funcionarioMenorCod;

//     //Le o proximo funcionario
//     fseek(arq,6 * sizeof(Funcionario), SEEK_SET);
//     funcionarioMemoria[posMenorCod] = leitura(arq);

//     //Verifica se o codigo do funcionario é menor que o ultimo registrado, caso seja é colocado no reservatório
//     int i = 0;
//     if (funcionarioMemoria[posMenorCod]->cod < funcionarioMenorCodSalvo->cod){
//         funcionarioReservatorio[i] = funcionarioMemoria[posMenorCod];
//         i++;
//     }
// }

//-------------------------------------------------
void vetorCongeladoInicial(char *vetor, int M)
{
    for (int i = 0; i < M; i++)
    {
        vetor[i] = '0';
    }
}

int vetorVazio(Funcionario **vetor, int n)
{
    for (int i = 0; i < n; i++)
    {
        if (vetor[i] != NULL)
            return 0;
    }
    return 1;
}

int pegarMinIndexCongelado(int M, Funcionario **memoria, char *congelado)
{
    int minIndex, first = 0;

    for (int k = 0; k < M;)
    {
        if (congelado[k] == 'X')
        {
            k++;
            continue;
        }
        if (first == 0)
        {
            minIndex = k;
            first++;
        }

        if (memoria[k]->cod < memoria[minIndex]->cod)
        {
            minIndex = k;
        }
        k++;
    }
    return minIndex;
}

int pegarMinimoIndex(int M, Funcionario **memoria)
{

    int minIndex, first = 0;

    for (int k = 0; k < M;)
    {
        if (memoria[k] == NULL)
        {
            k++;
            continue;
        }
        if (first == 0)
        {
            minIndex = k;
            first++;
        }

        if (memoria[k]->cod < memoria[minIndex]->cod)
        {
            minIndex = k;
        }
        k++;
    }
    return minIndex;
}

int transferirReservatorioParaMemoria(FILE *reservatorio, Funcionario **memoria, int indexReservatorio, int M)
{
    rewind(reservatorio);
    Funcionario *aux = NULL;
    int i = 0;
    while ((aux = leitura(reservatorio)) != NULL && i < M)
    {
        memoria[i] = aux;
        indexReservatorio--;
        i++;
    }
    // rewind(reservatorio);
    return indexReservatorio;
}

int vetorCongeladoX(char *vetor, int m)
{
    int countadorX = 0;
    for (int i = 0; i < m; i++)
    {
        if (vetor[i] == 'X')
        {
            countadorX++;
        }
    }
    return countadorX == m ? 1 : 0;
}

void selecaoSubs(char *nomeEntrada, Nomes *nomeSaida, int M)
{
    int final = 0;
    FILE *arq;

    if ((arq = fopen(nomeEntrada, "rb")) == NULL)
    {
        printf("Nao foi possivel abrir o arquivo");
    }
    else
    {
        FILE *p; // arquivo de partições

        Funcionario *memoria[M]; // array de memoria
        char congelado[M];       // array de congelados
        vetorCongeladoInicial(congelado, M);

        int f = 0;
        while (!feof(arq) && f < M)
        {
            memoria[f] = leitura(arq);
            f++;
        }

        while (final == 0)
        {
            char *nomeParticao = nomeSaida->nome;

            if ((p = fopen(nomeParticao, "wb")) == NULL)
            {
                printf("Nao foi possivel fazer o arquivo de saída");
            }
            else
            {
                while ((vetorCongeladoX(congelado, M) != 1))
                {
                    // enquanto nao tiver todo o array congelado ou for o fim do arquivo, pega o index do menor codigo dentro da memoria
                    int minIndex = pegarMinIndexCongelado(M, memoria, congelado);

                    // salva na partição
                    Funcionario *minFuncionario = memoria[minIndex]; // minFunc fica como ultimo cliente salvo na partição
                    escreve(minFuncionario, p);

                    // Pega o proximo registro
                    if (final == 0)
                    {
                        // Verifica se esta no final do arq de entrada, se estiver o alg nao carregara novos funcionarios
                        Funcionario *minFuncAux = leitura(arq);
                        if (minFuncAux != NULL)
                        {
                            memoria[minIndex] = minFuncAux;
                        }
                        else
                        {
                            // quando o funcionario carregado for null é porque ja esta no final do arquivo, entao ele gela a posição
                            final = 1;
                            congelado[minIndex] = 'X';
                        }
                    }
                    else
                    {
                        congelado[minIndex] = 'X';
                    }

                    if (congelado[minIndex] != 'X')
                    {
                        // faz com q o alg nao seja quebrado quando na memoria ja tiver clientes com NULL
                        if (memoria[minIndex]->cod < minFuncionario->cod)
                        {
                            congelado[minIndex] = 'X';
                        }
                    }
                }

                fclose(p);                           // fecha partição
                vetorCongeladoInicial(congelado, M); // descongela

                nomeSaida = nomeSaida->prox; // pega o proximo nome de partição e coloca como atual
            }
        }
    }
}

void selecaoNatural(char *nome_arq_entrada, Nomes *nome_arqs_saida, int M, int n)
{
    // Vars de controle para saber se o arq de entrada terminou/ fim
    int indexReservatorio = 0, isEmptyEntrada = 0, fimParticao = 0;
    ;

    FILE *arq;          // ponteiro para arq de entrada
    FILE *reservatorio; // ponteiro para arq reservatorio

    // abre o arquivo para leitura
    arq = fopen(nome_arq_entrada, "rb");
    if (arq == NULL)
    {
        printf("[ERRO1] - Nao foi possivel ler o arquivo\n");
    }
    else
    {
        FILE *p; // arq de partições

        Funcionario *memoria[M]; // array em memoria

        // Le os M primeiros registros
        int z = 0;
        while (!feof(arq) && z < M)
        {
            memoria[z] = leitura(arq);
            z++;
        }

        while ((isEmptyEntrada == 0 && vetorVazio(memoria, M) == 0))
        { // enquanto nao for o fim fo arquivo de entrada e fim do alg
            reservatorio = fopen("reservatorio.dat", "wb+");
            // abre arquivo de partição
            char *nome_particao = nome_arqs_saida->nome; // pega o nome da primeira partição
            if ((p = fopen(nome_particao, "wb")) == NULL)
            {
                printf("[ERRO2] - Nao foi possivel criar arquivo de saida\n");
            }
            else
            {
                fimParticao = 0;
                while (fimParticao == 0)
                { // enquanto nao tiver todo o array NULL ou for o fim do arquivo
                    if (vetorVazio(memoria, M) == 1 || indexReservatorio > 5 || isEmptyEntrada != 0)
                    {
                        fimParticao = 1;
                    }
                    if (fimParticao == 1)
                    { // esvazia a memoria
                        while (vetorVazio(memoria, M) != 1)
                        {
                            int minIndex = pegarMinimoIndex(M, memoria);
                            escreve(memoria[minIndex], p);
                            memoria[minIndex] = NULL;
                        }
                        break;
                    }
                    // pega o index do menor codigo dentro da memoria
                    int minIndex = pegarMinimoIndex(M, memoria);

                    // salva na partição
                    Funcionario *minFunc = memoria[minIndex]; // minFunc fica como ultimo cliente salvo na partição
                    escreve(minFunc, p);
                    memoria[minIndex] = NULL;

                    // Pega o proximo registro
                    if (isEmptyEntrada == 0)
                    { // se estiver puxando funcionarios NULL
                        Funcionario *minFuncNovoR;
                        do
                        {
                            if (indexReservatorio > 5)
                            {
                                break;
                            }
                            minFuncNovoR = leitura(arq);
                            if (minFuncNovoR != NULL)
                            {
                                if (minFuncNovoR->cod < minFunc->cod)
                                {
                                    escreve(minFuncNovoR, reservatorio);
                                    indexReservatorio++; // indica quantos elementos estao dentro do reservatorio
                                }
                                else
                                {
                                    memoria[minIndex] = minFuncNovoR;
                                }
                            }
                            else
                            {
                                isEmptyEntrada = 1;
                                memoria[minIndex] = NULL;
                                break;
                            }
                        } while (minFuncNovoR->cod < minFunc->cod);
                    }
                }
                indexReservatorio = transferirReservatorioParaMemoria(reservatorio, memoria, indexReservatorio, M);
                fclose(reservatorio);
                fclose(p);                               // fecha a partição atual
                nome_arqs_saida = nome_arqs_saida->prox; // pega o proximo nome de partição e o coloca como atual
            }
        }
        if ((isEmptyEntrada != 0) && (vetorVazio(memoria, M) == 0))
        {
            char *nome_particao = nome_arqs_saida->nome;
            if ((p = fopen(nome_particao, "wb")) == NULL)
            {
                printf("[ERRO3] - nao foi possivel criar arquivo de saida\n");
            }
            else
            {
                while (vetorVazio(memoria, M) != 1)
                {
                    int minIndex = pegarMinimoIndex(M, memoria);
                    escreve(memoria[minIndex], p);
                    memoria[minIndex] = NULL;
                }
                fclose(p);
            }
        }
    }

    fclose(arq);
}

char *geradorDeString(char *str)
{
    char *s = (char *)calloc(strlen(str) + 1, sizeof(char));
    strcpy(s, str);
    return s;
}

Nomes *geradorDeNomes(char *nome, Nomes *prox)
{
    Nomes *nomes = (Nomes *)malloc(sizeof(Nomes));
    nomes->nome = nome;
    nomes->prox = prox;
    return nomes;
}

void selecSub()
{
    Nomes *nomes;

    printf("\n Gerando particoes com SELECAO SUBSTITUICAO...\n");

    nomes = geradorDeNomes(geradorDeString("p_ss1.dat"),
                           geradorDeNomes(geradorDeString("p_ss2.dat"),
                                          geradorDeNomes(geradorDeString("p_ss3.dat"),
                                                         geradorDeNomes(geradorDeString("p_ss4.dat"),
                                                                        geradorDeNomes(geradorDeString("p_ss5.dat"),
                                                                                       geradorDeNomes(geradorDeString("p_ss6.dat"),
                                                                                                      geradorDeNomes(geradorDeString("p_ss7.dat"),
                                                                                                                     geradorDeNomes(geradorDeString("p_ss8.dat"),
                                                                                                                                    geradorDeNomes(geradorDeString("p_ss9.dat"), NULL)))))))));
    printf("\n");

    selecaoSubs("buscaFuncionario.dat", nomes, 6);
}

void selecNat()
{
    Nomes *nomes;

    printf("\n Gerando particoes com selecaoNatural...\n");

    nomes = geradorDeNomes(geradorDeString("p_sn1.dat"),
                           geradorDeNomes(geradorDeString("p_sn2.dat"),
                                          geradorDeNomes(geradorDeString("p_sn3.dat"),
                                                         geradorDeNomes(geradorDeString("p_sn4.dat"),
                                                                        geradorDeNomes(geradorDeString("p_sn5.dat"),
                                                                                       geradorDeNomes(geradorDeString("p_sn6.dat"),
                                                                                                      geradorDeNomes(geradorDeString("p_sn7.dat"),
                                                                                                                     geradorDeNomes(geradorDeString("p_sn8.dat"),
                                                                                                                                    geradorDeNomes(geradorDeString("p_sn9.dat"), NULL)))))))));
    printf("\n");

    selecaoNatural("buscaFuncionario.dat", nomes, 6, 6);
}
//------------------------------------------------------------

int hash(Funcionario *f, int tam)
{
    return f->cod % tam;
}

void inicializa(Hash *tab, int m)
{
    int i;
    for (i = 0; i < m; i++)
    {
        tab[i] = NULL;
    }
}

Funcionario *aloca(Funcionario *f)
{
    Funcionario *func = (Funcionario *)malloc(sizeof(Funcionario));
    func = f;
    f->prox = NULL;
    return func;
}

void insere(Hash *tab, int m, FILE *arquivo)
{
    int h;

    for (int i = 0; i < 100; i++)
    {
        fseek(arquivo, i * sizeof(Funcionario), SEEK_SET);
        Funcionario *func = leitura(arquivo);
        h = hash(func, m);

        Funcionario *p = tab[h];
        Funcionario *anterior = NULL;

        while ((p != NULL))
        {
            anterior = p;
            p = p->prox;
        }

        if (p == NULL)
        {
            Funcionario *novo = aloca(func);
            if (!anterior)
            {
                tab[h] = novo;
            }
            else
            {
                anterior->prox = novo;
            }
        }
    }
}

void libera(Hash *tab, int m)
{
    int i;
    for (i = 0; i < m; i++)
        if (tab[i])
        {
            Funcionario *p = tab[i];
            Funcionario *q;
            while (p)
            {
                q = p;
                p = p->prox;
                free(q);
            }
        }
}

// float exclui(Hash *tab, int m, int cod) {
//     int h = hash(cod, m);
//     if (tab[h] == NULL) {
//         return -1;
//     }
//     Funcionario *p = tab[h];
//     Funcionario *anterior = NULL;
//     float cr = -1;
//     while ((p != NULL) && (p->cod != cod)) {
//         ant = p;
//         p = p->prox;
//     }
//     if (p == NULL) {
//         return cr;
//     }
//     if (anterior == NULL) {
//         tab[h] = p->prox;
//     } else {
//         anterior->prox = p->prox;
//     }
//     cr = p->cr;
//     free(p);
//     return cr;
// }

void imprimeHash(Hash *tab)
{
    int cont = 0;
    for (int i = 0; i < 100; i++)
    {
        if (tab[i] != NULL)
        {
            printf("\n %d", tab[i]->cod);
            Funcionario *aux = tab[i]->prox;

            while (aux != NULL)
            {
                printf(" -> %3d", aux->cod);
                aux = aux->prox;
            }
        }
    }
}

void alocaVetor(Funcionario *vetor[])
{
    for (int i = 0; i < TAM_HASH_S; i++)
    {
        vetor[i] = (Funcionario *)malloc(sizeof(Funcionario));
        vetor[i] = NULL;
    }
}

void hashSemTratamento(FILE *arq, Funcionario *vetorHash[])
{
    int qtdColisao = 0;
    alocaVetor(vetorHash);

    for (int i = 0; i < TAM_HASH_S; i++)
    {
        fseek(arq, i * sizeof(Funcionario), SEEK_SET);
        Funcionario *funcionario = leitura(arq);
        int pos = funcionario->cod % TAM_HASH_S;

        if (vetorHash[pos] == NULL)
        {
            vetorHash[pos] = funcionario;
        }
        else
        {
            qtdColisao++;
        }
    }
    printf("\nAconteceram %d colisoes.", qtdColisao);
}

void buscaHashSemTratamento(int cod, Funcionario *vetorHash[])
{
    int pos = cod % TAM_HASH_S;

    if (vetorHash[pos] == NULL)
    {
        printf("\nFuncionario nao encontrado!");
    }
    else
    {
        printf("\nCodigo: %d", vetorHash[pos]->cod);
        printf("\nNome: %s", vetorHash[pos]->nome);
        printf("\nSalario: %.2lf", vetorHash[pos]->salario);
        printf("\nCPF: %s", vetorHash[pos]->cpf);
        printf("\nNascimento: %s", vetorHash[pos]->nascimento);
    }
}

//------------------------------------------------------------

int main()
{
    FILE *arq;
    arq = fopen("buscaFuncionario.dat", "wb+");
    Funcionario *funcionario;
    int loop = 1, escolha;

    if (arq == NULL)
    {
        printf("Nao foi possivel abrir o arquivo");
        return 1;
    }

    preenche(arq);

    do
    {
        printf("----------------------\n");
        printf("Digite a opcao de operacao\n1-Busca Sequencial\n2-Busca Binaria\n3-Insertion Sort\n4-Ordenacao por Selecao com Substituicao\n5-Ordenacao por Selecao Natural\n6-Ordernar por Hash sem tratamento\n7-Ordernar por Hash com tratamento\n\n");
        scanf("%d", &escolha);
        switch (escolha)
        {
        case 1:;
            clock_t tempoSequencialI = clock();
            funcionario = buscaSequencial(arq, 20);
            clock_t tempoSequencialF = clock();
            double tempoFuncaoSequencial = ((double)tempoSequencialF - tempoSequencialI) / CLOCKS_PER_SEC;

            if (funcionario == NULL)
            {
                printf("\nFuncionario nao encontrado");
            }
            else
            {
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

        case 2:;
            clock_t tempoBinarioI = clock();
            funcionario = buscaBinaria(20, arq);
            clock_t tempoBinarioF = clock();
            double tempoFuncaoBinario = ((double)tempoBinarioF - tempoBinarioI) / CLOCKS_PER_SEC;

            if (funcionario == NULL)
            {
                printf("\nFuncionario nao encontrado");
            }
            else
            {
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

        case 3:;
            clock_t tempoOrdenacaoI = clock();
            insertion_sort(arq, tamanho_arquivo(arq));
            clock_t tempoOrdenacaoF = clock();
            double tempoOrdenacao = ((double)tempoOrdenacaoF - tempoOrdenacaoI) / CLOCKS_PER_SEC;
            printf("\nO tempo gasto na funcao de ordenacao foi %f", tempoOrdenacao);
            break;

        case 4:;
            selecSub();
            break;

        case 5:;
            selecNat();
            break;

        case 6:;
            Funcionario *vetorHash[TAM_HASH_S];
            hashSemTratamento(arq, vetorHash);
            int cod = 0;
            int escolha = 0;
            printf("\nDeseja pesquisar um funcionario? (1-Sim | 2-Nao) ");
            scanf("%d", &escolha);

            if (escolha == 1)
            {
                printf("\nDigite o cod do funcionario a ser buscado: ");
                scanf("%d", &cod);
                buscaHashSemTratamento(cod, vetorHash);
            }
            break;

        case 7: ;
            Hash *tabela[TAM_HASH_C];
            inicializa(tabela, TAM_HASH_C);

            printf("Inserindo codigo dos funcionarios em uma tabela hash\n");
            insere(tabela, TAM_HASH_C, arq);

            printf("Tabela Hash: \n");
            imprimeHash(tabela);

            break;

        default:
            printf("Numero invalido");
        }

        printf("\n----------------------");

        printf("\n\nDeseja continuar operando?\nSe sim, digite 1\nSe nao digite 2\n\n");
        scanf("%d", &loop);

        system("cls");

    } while (loop == 1);
}