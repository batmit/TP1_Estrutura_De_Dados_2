#include "arvoreBE.h"

void ArvoreBE(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA) {

    TipoPagina *ArvoreBE = malloc(sizeof(TipoPagina));
    InicializaBE(ArvoreBE);
    Registro reg;


    for(int i = 0; i < quantidade; i++){
        fread(&reg, sizeof(Registro), 1, arquivo);
        //InsereBE(reg, &ArvoreBE, situacao, dados);
    }

    PesquisaBE(resultado, &ArvoreBE, dados);
    
    free(ArvoreBE);
}

void PesquisaBE(Registro *x, TipoPagina **Ap, Dados* dados) {
    int i;
    TipoPagina* pag;
    pag = *Ap;

    if((*Ap)->Pt == Interna){
        i = 1;
        while(i < pag->UU.U0.ni && x->chave > pag->UU.U0.ri[i - 1]){
            dados->comparacoes.pesquisa++;
            i++;
        }

        if(x->chave < pag->UU.U0.ri[i - 1])
            PesquisaBE(x, &pag->UU.U0.pi[i - 1], dados);
        else
            PesquisaBE(x, &pag->UU.U0.pi[i], dados);
        return;
    }
    i = 1;

    while(i < pag->UU.U1.ne && x->chave > pag->UU.U1.re[i - 1]->chave){
        dados->comparacoes.pesquisa++;
        i++;
    }

    if(x->chave == pag->UU.U1.re[i - 1]->chave)
        *x = pag->UU.U1.re[i - 1];
    else
        printf("Registro não está presente na árvore\n");
}

void InicializaBE(TipoPagina *Ap){
    Ap->UU.U0.ni = 0;
    Ap->Pt = Interna;
}

void InsereBE(Registro reg, TipoPagina **raiz, char *situacao, Dados *dados){
    
    bool cresceu;
    int chaveSubiu;
    TipoPagina *noSubiu;

    InsRec(&reg, &ArvoreBE, NULL, 0, &cresceu, &chaveSubiu, &noSubiu, dados);

    /* Raiz cresceu: cria nova raiz interna */
    if (cresceu) {
        TipoPagina *novaRaiz = NovaInterna();
        novaRaiz->UU.U0.ni      = 1;
        novaRaiz->UU.U0.ri[0]   = chaveSubiu;
        novaRaiz->UU.U0.pi[0]   = *raiz;
        novaRaiz->UU.U0.pi[1]   = noSubiu;
        *raiz = novaRaiz;
    }
    
    
}

static void InsRec(Registro *reg, TipoPagina *Ap, TipoPagina *pai,
                   int idxNoPai, bool *cresceu,
                   int *chaveSubiu, TipoPagina **noSubiu,
                   Dados *dados) {

    *cresceu = false;

    /* ── Página folha: inserção efetiva ── */
    if (Ap->Pt == Externa) {
        if (Ap->UU.U1.ne < MM) {
            /* Há espaço: insere diretamente */
            InsereNaFolha(Ap, reg);
            return;
        }

        /* Tenta redistribuir com irmão direito */
        if (pai != NULL && idxNoPai < pai->UU.U0.ni) {
            TipoPagina *irmaoDir = pai->UU.U0.pi[idxNoPai + 1];
            if (irmaoDir->UU.U1.ne < MM) {
                InsereNaFolha(Ap, reg);
                RedistribuiFolha(pai, idxNoPai, Ap, irmaoDir);
                return;
            }
        }

        /* Folha cheia: tenta redistribuir com irmão esquerdo */
        if (pai != NULL && idxNoPai > 0) {
            TipoPagina *irmaoEsq = pai->UU.U0.pi[idxNoPai - 1];
            if (irmaoEsq->UU.U1.ne < MM) {
                InsereNaFolha(Ap, reg);
                RedistribuiFolha(pai, idxNoPai - 1, irmaoEsq, Ap);
                return;
            }
        }

        /* Ambos irmãos cheios (ou não existem): split 2→3 */
        if (pai != NULL && idxNoPai > 0) {
            TipoPagina *irmaoEsq = pai->UU.U0.pi[idxNoPai - 1];
            TipoPagina *noMeio;
            int chaveMeio;
            noMeio   = Split2para3Folha(pai, idxNoPai - 1,
                                         irmaoEsq, Ap, reg, &chaveMeio);
            *cresceu  = true;
            *chaveSubiu = chaveMeio;
            *noSubiu    = noMeio;
            return;
        }

        /* Sem pai ou sem irmão esquerdo: split simples 1→2 */
        TipoPagina *novaDir = NovaFolha();
        InsereNaFolha(Ap, reg);

        int metade = Ap->UU.U1.ne / 2;
        novaDir->UU.U1.ne = 0;
        for (int i = metade; i < Ap->UU.U1.ne; i++) {
            novaDir->UU.U1.re[i - metade] = Ap->UU.U1.re[i];
            novaDir->UU.U1.ne++;
        }
        Ap->UU.U1.ne      = metade;
        novaDir->UU.U1.prox = Ap->UU.U1.prox;
        Ap->UU.U1.prox      = novaDir;

        *cresceu    = true;
        *chaveSubiu = novaDir->UU.U1.re[0]->chave;
        *noSubiu    = novaDir;
        return;
    }

    /* ── Página interna: desce para filho correto ── */
    int i = 1;
    while (i < Ap->UU.U0.ni && reg->chave > Ap->UU.U0.ri[i - 1]) {
        dados->comparacoes.indexacao++;
        i++;
    }
    dados->comparacoes.indexacao++;

    int idxFilho = (reg->chave < Ap->UU.U0.ri[i - 1]) ? i - 1 : i;

    int chaveFilho;
    TipoPagina *noFilho;
    InsRec(reg, Ap->UU.U0.pi[idxFilho], Ap, idxFilho,
           cresceu, &chaveFilho, &noFilho, dados);

    if (!*cresceu) return;

    /* Filho cresceu: tenta inserir na página interna */
    if (Ap->UU.U0.ni < MM) {
        InsereNaInterna(Ap, chaveFilho, noFilho);
        *cresceu = false;
        return;
    }

    /* Página interna cheia: split simples (B* em folhas já garante 2/3) */
    TipoPagina *novaDir = NovaInterna();
    InsereNaInterna(Ap, chaveFilho, noFilho);  // insere temporariamente

    int meio = Ap->UU.U0.ni / 2;

    novaDir->UU.U0.ni = 0;
    novaDir->UU.U0.pi[0] = Ap->UU.U0.pi[meio + 1];
    for (int j = meio + 1; j < Ap->UU.U0.ni; j++) {
        novaDir->UU.U0.ri[j - meio - 1] = Ap->UU.U0.ri[j];
        novaDir->UU.U0.pi[j - meio]     = Ap->UU.U0.pi[j + 1];
        novaDir->UU.U0.ni++;
    }

    *chaveSubiu = Ap->UU.U0.ri[meio];
    Ap->UU.U0.ni = meio;
    *noSubiu     = novaDir;
    *cresceu     = true;
}
