#ifndef INDEXADA_H
#define INDEXADA_H

#include "registro.h"

bool BuscaSequencialIndexada(FILE* arquivo, int quantidade, char* situacao, 
                             Registro* resultado, Dados* dados);
bool pesquisaSequencial(int *chaves, int pos, char* situacao, FILE* arquivo, Registro* resultado, Dados* dados);

void CriaArquivo(int quantidade, char* situacao);

//função para calcular a média de comparações, transferências e tempo de 10 operações de busca com chaves aleatórias
void CalculaMedia(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados);

#endif