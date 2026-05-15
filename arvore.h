#ifndef ARVORE_H
#define ARVORE_H

#include "registro.h"

typedef struct No {
    Registro reg;
    int indiceEsq; // Índice do filho esquerdo no arquivo
    int indiceDir; // Índice do filho direito no arquivo
} No;

void ArvoreBinariaDePesquisa(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA);

bool ArvoreBuscaBinaria(FILE* arquivo, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA);

bool Busca(FILE* arquivo, No* no, int indice, No* pagina, Registro* resultado, Dados* dados, int *paginaCarregada, int TAM_PAGINA);
void criarArvore(char *situacao, int quantidade, int TAM_PAGINA);

void inserirArvore(FILE *arquivoArvore, No *noCopia);




#endif