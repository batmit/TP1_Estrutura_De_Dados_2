#ifndef ARVOREB_H
#define ARVOREB_H

#include "registro.h"

#define M 4//ordem da árvore B, ou seja, o número mínimo de registros em uma página é M e o número máximo de chaves é MM
#define MM (2*M)//número máximo de registros em uma página

typedef struct TipoPagina{
    int n;//número de registros na página
    Registro r[MM];//registros da página
    struct TipoPagina* p[MM + 1];//ponteiros para as páginas filhas
} TipoPagina;

void Inicializa(TipoPagina** raiz);
void Pesquisa(Registro* x, TipoPagina* Ap, char* situacao, Dados* dados);
void InsereNaPagina(TipoPagina* Ap, Registro reg, TipoPagina* ApDir);
void Ins(Registro reg, TipoPagina* Ap, bool *cresceu, Registro* resultado, TipoPagina** ApRetorno, Dados* dados);
void Insere(Registro reg, TipoPagina** Ap, char* situacao, Dados* dados);
#endif