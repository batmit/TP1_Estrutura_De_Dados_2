#include "arvoreB.h"

void Inicializa(TipoPagina** raiz) {
    *raiz = NULL;
}

void Pesquisa(Registro *x, TipoPagina* Ap, char* situacao, Dados* dados) {
    long i = 1;
    if(Ap == NULL){
        printf("Registro nao encontrado\n");
        return;
    }
    while(i < Ap->n && x->chave > Ap->r[i - 1].chave){
        i++;
        dados->comparacoes.pesquisa++;//contabiliza a comparação da chave do resultado com a chave do nó atual
    }
    if(x->chave == Ap->r[i - 1].chave){
        *x = Ap->r[i - 1];
        printf("Registro encontrado: %d\n", x->chave);
        printf("Dado1: %ld\n", x->dado1);
        printf("Dado2: %s\n", x->dado2);
        printf("Dado3: %s\n", x->dado3);
        return;
    }
    if(x->chave < Ap->r[i - 1].chave)
        Pesquisa(x, Ap->p[i - 1], situacao, dados);
    else
        Pesquisa(x, Ap->p[i], situacao, dados);
}
void InsereNaPagina(TipoPagina* Ap, Registro reg, TipoPagina* ApDir){
    bool NaoAchouPosicao;
    int k;
    k = Ap->n;
    NaoAchouPosicao = (k > 0);
    while(NaoAchouPosicao){
        if(reg.chave >= Ap->r[k-1].chave){
            NaoAchouPosicao = false;
            break;
        }
        Ap->r[k] = Ap->r[k-1];
        Ap->p[k+1] = Ap->p[k];
        k--;
        if(k < 1)
            NaoAchouPosicao = false;
    }
    Ap->r[k] = reg;
    Ap->p[k+1] = ApDir;
    Ap->n++;
}

void Ins(Registro reg, TipoPagina* Ap, bool *cresceu, Registro* resultado, TipoPagina** ApRetorno, Dados* dados){
    long i = 1;
    long j;
    TipoPagina* ApTemp;
    if(Ap == NULL){
        *cresceu = true;
        (*ApRetorno) = NULL;
        *resultado = reg;
        return;
    }
    while(i < Ap->n && reg.chave > Ap->r[i - 1].chave){
        i++;
        dados->comparacoes.indexacao++;//contabiliza a comparação da chave do resultado com a chave do nó atual
    }
    if(reg.chave == Ap->r[i - 1].chave){
        printf("Registro ja existe\n");
        *cresceu = false;
        return;
    }
    if(reg.chave < Ap->r[i - 1].chave)
        i--;
    Ins(reg, Ap->p[i], cresceu, resultado, ApRetorno, dados);
    if(!*cresceu)        
        return;
    if(Ap->n < MM){
        InsereNaPagina(Ap, *resultado, *ApRetorno);
        *cresceu = false;
        return;
    }

    ApTemp = (TipoPagina*) malloc(sizeof(TipoPagina));
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;

    if(i < M + 1){
        InsereNaPagina(ApTemp, Ap->r[MM - 1], Ap->p[MM]);
        Ap->n--;
        InsereNaPagina(Ap, *resultado, *ApRetorno);
    }
    else
        InsereNaPagina(ApTemp, *resultado, *ApRetorno);

    for(j = M + 2; j <= MM; j++)
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);
    
    Ap->n = M;
    ApTemp->p[0] = Ap->p[M + 1];
    *resultado = Ap->r[M];
    *ApRetorno = ApTemp;
}

void Insere(Registro reg, TipoPagina** Ap, char* situacao, Dados* dados){
    bool cresceu;
    Registro resultado;
    TipoPagina* ApRetorno;

    Ins(reg, *Ap, &cresceu, &resultado, &ApRetorno, dados);
    if(cresceu){
        TipoPagina* ApTemp = (TipoPagina*) malloc(sizeof(TipoPagina));
        ApTemp->n = 1;
        ApTemp->r[0] = resultado;
        ApTemp->p[0] = *Ap;
        ApTemp->p[1] = ApRetorno;
        *Ap = ApTemp;
    }
}