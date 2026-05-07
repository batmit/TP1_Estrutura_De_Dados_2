#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "indexada.h"
#include "arvore.h"

int main(int argc, char *argv[]){//pesquisa <método> <quantidade> <situação> <chave> [-P] 
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
    }
    else if(atoi(argv[1]) == 3){//Árvore B
        inicio = clock();
        //ArvoreB(arquivo, atoi(argv[2]), &resultado, &dados);
        fim = clock();
    }
    else if(atoi(argv[1]) == 4){//Árvore B*
        inicio = clock();
        //ArvoreB*(arquivo, atoi(argv[2]), &resultado, &dados);
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

    CalculaMedia(arquivo, atoi(argv[2]), argv[3], &resultado, &dados);
    return 0;
}