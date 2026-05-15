#ifndef INDEXADA_H
#define INDEXADA_H

#include "registro.h"

bool BuscaSequencialIndexada(FILE* arquivo, int quantidade, char* situacao, 
                             Registro* resultado, Dados* dados, int TAM_PAGINA);
bool pesquisaSequencial(int *chaves, int pos, char* situacao, FILE* arquivo, Registro* resultado, Dados* dados, int TAM_PAGINA);

void CriaArquivo(int quantidade, char* situacao);

#endif