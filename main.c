#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "indexada.h"
#include "arvore.h"

void CalculaMedia(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int metodo);
void imprimirNoArvore(FILE *arquivo, int indice);
void imprimirArvore(const char *nomeArquivo);


int main(int argc, char *argv[]){//pesquisa <método> <quantidade> <situação> <chave> [-P] 

    srand(time(NULL));

    if(argc != 5 && argc != 6){
        printf("Sintaxe: %s <método> <quantidade> <situação> <chave> [-P]\n", argv[0]);
        return 1;
    }

    Dados dados;
    dados.comparacoes.indexacao = 0;
    dados.comparacoes.pesquisa = 0;
    dados.transferencias.indexacao = 0;
    dados.transferencias.pesquisa = 0;

    clock_t inicio, fim;
    Registro resultado;
    resultado.chave = atoi(argv[4]);
    FILE* arquivo = NULL;

    CriaArquivo(atoi(argv[2]), argv[3]);//função para criar o arquivo com a quantidade de registros especificada

    if(atoi(argv[1]) == 1){//Busca Sequencial Indexada
        inicio = clock();
        BuscaSequencialIndexada(arquivo, atoi(argv[2]), argv[3], &resultado, &dados);
        fim = clock();
    }
    else if(atoi(argv[1]) == 2){//Arvore Binária de Pesquisa
        inicio = clock();
        ArvoreBinariaDePesquisa(arquivo, atoi(argv[2]), argv[3], &resultado, &dados);
        fim = clock();
        //imprimirArvore("arvore.bin");
    }
    else if(atoi(argv[1]) == 3){//Árvore B
        inicio = clock();
        //ArvoreB(arquivo, atoi(argv[2]), &resultado, &dados);
        fim = clock();
    }
    else if(atoi(argv[1]) == 4){//Árvore B*
        inicio = clock();
        //ArvoreBEstrela(arquivo, atoi(argv[2]), &resultado, &dados);
        fim = clock();
    }
    else{
        printf("Metodo de busca desconhecido: %s\n", argv[1]);
        return 1;
    }

    if(argc == 6 && strcmp(argv[5], "-P") == 0) {//
        printf("a");
    }

    double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;
     //calcula o tempo gasto na busca
    printf("Comparacoes na indexacao: %d\n", dados.comparacoes.indexacao);
    printf("Comparacoes na pesquisa: %d\n", dados.comparacoes.pesquisa);
    printf("Tempo: %lf s\n", tempo);
    printf("Transferencias na indexacao: %d\n", dados.transferencias.indexacao);
    printf("Transferencias na pesquisa: %d\n", dados.transferencias.pesquisa);

    printf("----------------------------\n");

    CalculaMedia(arquivo, atoi(argv[2]), argv[3], &resultado, &dados, atoi(argv[1]));
    return 0;
}

//OBS: a calcula media deve valer para todas as formas de busca, não apenas a indexada
//Apenas a que a pessoa escolher
void CalculaMedia(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int metodo){
    int totalComparacoesIndexacao = 0;
    int totalComparacoesPesquisa = 0;
    int totalTransferenciasIndexacao = 0;
    int totalTransferenciasPesquisa = 0;
    double totalTempo = 0.0;

    for(int i = 0; i < 10; i++){
        resultado->chave = rand() % quantidade; //gera uma chave aleatória para a busca
        clock_t inicio = clock();
        if(metodo == 1)
            BuscaSequencialIndexada(arquivo, quantidade, situacao, resultado, dados);
        else if(metodo == 2)
            ArvoreBinariaDePesquisa(arquivo, quantidade, situacao, resultado, dados);
        else if (metodo == 3){
            //ArvoreB(arquivo, quantidade, resultado, dados);
            printf("Arvore b\n");
        }
        else if (metodo == 4){
            //ArvoreBEstrela(arquivo, quantidade, resultado, dados);
            printf("Arvore b estrela\n");
        }
        else{
            printf("Metodo de busca desconhecido: %d\n", metodo);
            return;
        }
            
        clock_t fim = clock();
        double tempo = (double)(fim - inicio) / CLOCKS_PER_SEC;

        totalComparacoesIndexacao += dados->comparacoes.indexacao;
        totalComparacoesPesquisa += dados->comparacoes.pesquisa;
        totalTransferenciasIndexacao += dados->transferencias.indexacao;
        totalTransferenciasPesquisa += dados->transferencias.pesquisa;
        totalTempo += tempo;

        //Reseta as contagens para a próxima iteração
        dados->comparacoes.indexacao = 0;
        dados->comparacoes.pesquisa = 0;
        dados->transferencias.indexacao = 0;
        dados->transferencias.pesquisa = 0;
    }

    printf("Media de Comparacoes na Indexacao: %d\n", totalComparacoesIndexacao / 10);
    printf("Media de Comparacoes na Pesquisa: %d\n", totalComparacoesPesquisa / 10);
    printf("Media de Transferencias na Indexacao: %d\n", totalTransferenciasIndexacao / 10);
    printf("Media de Transferencias na Pesquisa: %d\n", totalTransferenciasPesquisa / 10);
    printf("Media de Tempo: %lf s\n", totalTempo / 10);
}