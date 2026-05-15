#ifndef REGISTRO_H
#define REGISTRO_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>  

#define TAM_PAGINA 10

typedef struct registro {
    int chave;
    long int dado1;
    char dado2[1001];
    char dado3[5001];
}Registro;

typedef struct transferencia{
    int indexacao;//transferencias no processo de indexação
    int pesquisa;//transferencias no processo de pesquisa
} Transferencia;

typedef struct comparacao{
    int indexacao;//comparações no processo de indexação
    int pesquisa;//comparações no processo de pesquisa
} Comparacao;

typedef struct dados{
    Transferencia transferencias;
    Comparacao comparacoes;
} Dados;



#endif