#ifndef ARVOREBE_H
#define ARVOREBE_H

#include "registro.h"

typedef struct TipoPagina TipoPagina;

#define M 4//ordem da árvore B, ou seja, o número mínimo de registros em uma página é M e o número máximo de chaves é MM
#define MM (2*M)//número máximo de registros em uma página
#define MM2 (2*M)//número maximo de chaves em uma página externa

typedef enum {Interna, Externa} TipoIntExt;

typedef struct TipoPagina {
    TipoIntExt Pt;
    union {
        struct {
            int ni;
            int ri[MM];
            TipoPagina* pi[MM + 1];
        } U0;
        struct {
            int ne;
            Registro re[MM2];
        } U1;
    } UU;
} TipoPagina;


void InicializaBE(TipoPagina *Ap);
void InsereBE(Registro x, TipoPagina **Ap, int *Cresceu, Registro *RegRetorno, TipoPagina **ApRetorno);
void PesquisaBE(Registro *x, TipoPagina **Ap, Dados* dados);
void ArvoreBE(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA);


#endif