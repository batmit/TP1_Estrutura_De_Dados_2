#ifndef ARVORE_H
#define ARVORE_H

#include "registro.h"

typedef struct No {
    Registro reg;
    int indiceEsq; // Índice do filho esquerdo no arquivo
    int indiceDir; // Índice do filho direito no arquivo
} No;

void ArvoreBinariaDePesquisa(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados);

bool ArvoreBuscaBinaria(FILE* arquivo, char* situacao, int* paginas, Registro* resultado, Dados* dados);

bool Busca(FILE* arquivo, No* no, int indice, int* paginas, No* pagina, Registro* resultado, Dados* dados, int *paginaCarregada);
void criarArvore(char *situacao, int quantidade);

void inserirArvore(FILE *arquivoArvore, No *noCopia);




#endif