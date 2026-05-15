#include "arvoreBE.h"

/* ------------------------------------------------------------------ */
/*  Auxiliares                                                          */
/* ------------------------------------------------------------------ */

void InicializaBE(TipoPagina *Ap) {
    Ap->Pt          = Folha;          // começa como folha vazia
    Ap->UU.U1.ne    = 0;
    Ap->UU.U1.prox  = NULL;
}

/* Libera recursivamente toda a árvore */
void LiberaArvore(TipoPagina *Ap) {
    if (Ap == NULL) return;
    if (Ap->Pt == Interna) {
        for (int i = 0; i <= Ap->UU.U0.ni; i++)
            LiberaArvore(Ap->UU.U0.pi[i]);
    }
    free(Ap);
}

/* ------------------------------------------------------------------ */
/*  Pesquisa                                                            */
/* ------------------------------------------------------------------ */

void PesquisaBE(Registro *x, TipoPagina **Ap, Dados *dados) {
    int i;
    TipoPagina *pag = *Ap;

    if (pag == NULL) {
        printf("Registro não encontrado\n");
        return;
    }

    if (pag->Pt == Interna) {
        i = 1;
        while (i < pag->UU.U0.ni && x->chave > pag->UU.U0.ri[i - 1]) {
            dados->comparacoes.pesquisa++;
            i++;
        }
        dados->comparacoes.pesquisa++;  // comparação que parou o laço

        if (x->chave < pag->UU.U0.ri[i - 1])
            PesquisaBE(x, &pag->UU.U0.pi[i - 1], dados);
        else
            PesquisaBE(x, &pag->UU.U0.pi[i], dados);
        return;
    }

    /* Página folha */
    i = 1;
    while (i < pag->UU.U1.ne && x->chave > pag->UU.U1.re[i - 1]->chave) {
        dados->comparacoes.pesquisa++;
        i++;
    }
    dados->comparacoes.pesquisa++;  // comparação que parou o laço

    if (i <= pag->UU.U1.ne && x->chave == pag->UU.U1.re[i - 1]->chave)
        *x = *pag->UU.U1.re[i - 1];  // CORRIGIDO: modifica o original via ponteiro
    else
        printf("Registro não está presente na árvore\n");
}

/* ------------------------------------------------------------------ */
/*  Inserção — Árvore B*                                               */
/* ------------------------------------------------------------------ */

/*
 * Diferença central da B* em relação à B:
 *   1. Ocupação mínima: 2/3 em vez de 1/2
 *   2. Antes de fazer split, tenta redistribuir com irmão adjacente
 *   3. Split cria 3 nós a partir de 2 cheios (2→3), mantendo ~2/3 de ocupação
 */

/* Insere reg ordenado na página folha Ap */
static void InsereNaFolha(TipoPagina *Ap, Registro *reg) {
    int k = Ap->UU.U1.ne;
    while (k > 0 && Ap->UU.U1.re[k - 1]->chave > reg->chave) {
        Ap->UU.U1.re[k] = Ap->UU.U1.re[k - 1];
        k--;
    }
    Ap->UU.U1.re[k] = reg;
    Ap->UU.U1.ne++;
}

/* Insere chave separadora e filho em página interna na posição correta */
static void InsereNaInterna(TipoPagina *Ap, int chave, TipoPagina *filho) {
    int k = Ap->UU.U0.ni;
    while (k > 0 && Ap->UU.U0.ri[k - 1] > chave) {
        Ap->UU.U0.ri[k]     = Ap->UU.U0.ri[k - 1];
        Ap->UU.U0.pi[k + 1] = Ap->UU.U0.pi[k];
        k--;
    }
    Ap->UU.U0.ri[k]     = chave;
    Ap->UU.U0.pi[k + 1] = filho;
    Ap->UU.U0.ni++;
}

/* Cria nova página folha inicializada */
static TipoPagina *NovaFolha(void) {
    TipoPagina *p = malloc(sizeof(TipoPagina));
    p->Pt         = Folha;
    p->UU.U1.ne   = 0;
    p->UU.U1.prox = NULL;
    return p;
}

/* Cria nova página interna inicializada */
static TipoPagina *NovaInterna(void) {
    TipoPagina *p  = malloc(sizeof(TipoPagina));
    p->Pt          = Interna;
    p->UU.U0.ni    = 0;
    for (int i = 0; i <= MM; i++)
        p->UU.U0.pi[i] = NULL;
    return p;
}

/*
 * Redistribui entre dois irmãos folha via pai.
 * Retorna true se conseguiu redistribuir (irmão tinha espaço).
 */
static bool RedistribuiFolha(TipoPagina *pai, int idxPai,
                              TipoPagina *esq, TipoPagina *dir) {
    int total = esq->UU.U1.ne + dir->UU.U1.ne;
    if (total < 2 * MM)  // ambos cheios, não redistribui
        return false;

    /* Junta tudo em buffer temporário */
    Registro *buf[2 * MM + 1];
    int n = 0;
    for (int i = 0; i < esq->UU.U1.ne; i++) buf[n++] = esq->UU.U1.re[i];
    for (int i = 0; i < dir->UU.U1.ne; i++) buf[n++] = dir->UU.U1.re[i];

    int metade = n / 2;

    /* Redistribui */
    esq->UU.U1.ne = 0;
    for (int i = 0; i < metade; i++) {
        esq->UU.U1.re[i] = buf[i];
        esq->UU.U1.ne++;
    }
    dir->UU.U1.ne = 0;
    for (int i = metade; i < n; i++) {
        dir->UU.U1.re[i - metade] = buf[i];
        dir->UU.U1.ne++;
    }

    /* Atualiza separador no pai */
    pai->UU.U0.ri[idxPai] = dir->UU.U1.re[0]->chave;
    return true;
}

/*
 * Split 2→3 em folhas (política B*):
 * dois irmãos cheios geram três nós com ~2/3 de ocupação.
 * Retorna a terceira página criada; sobe chave em *chaveSobe.
 */
static TipoPagina *Split2para3Folha(TipoPagina *pai, int idxPai,
                                     TipoPagina *esq, TipoPagina *dir,
                                     Registro *novoReg, int *chaveSobe) {
    /* Agrupa todos os registros em buffer */
    Registro *buf[2 * MM + 1];
    int n = 0;
    for (int i = 0; i < esq->UU.U1.ne; i++) buf[n++] = esq->UU.U1.re[i];
    for (int i = 0; i < dir->UU.U1.ne; i++) buf[n++] = dir->UU.U1.re[i];

    /* Insere o novo registro ordenado no buffer */
    int pos = n;
    while (pos > 0 && buf[pos - 1]->chave > novoReg->chave) {
        buf[pos] = buf[pos - 1];
        pos--;
    }
    buf[pos] = novoReg;
    n++;

    /* Divide em três partes iguais */
    int t1 = n / 3;
    int t2 = (n - t1) / 2;

    TipoPagina *meio = NovaFolha();

    /* Página esquerda: primeiros t1 */
    esq->UU.U1.ne = 0;
    for (int i = 0; i < t1; i++) {
        esq->UU.U1.re[i] = buf[i];
        esq->UU.U1.ne++;
    }

    /* Página do meio: próximos t2 */
    for (int i = 0; i < t2; i++) {
        meio->UU.U1.re[i] = buf[t1 + i];
        meio->UU.U1.ne++;
    }

    /* Página direita: restantes */
    dir->UU.U1.ne = 0;
    for (int i = 0; i < n - t1 - t2; i++) {
        dir->UU.U1.re[i] = buf[t1 + t2 + i];
        dir->UU.U1.ne++;
    }

    /* Encadeia folhas */
    esq->UU.U1.prox  = meio;
    meio->UU.U1.prox = dir;

    /* Atualiza separador existente no pai e sobe nova chave */
    pai->UU.U0.ri[idxPai] = meio->UU.U1.re[0]->chave;
    *chaveSobe             = dir->UU.U1.re[0]->chave;

    return meio;  // novo nó que sobe para o pai
}

/*
 * Função recursiva de inserção.
 * cresceu  → true se este nível produziu um nó que precisa subir
 * chaveSubiu / noSubiu → chave e ponteiro que sobem para o pai
 */
static void InsRec(Registro *reg, TipoPagina *Ap, TipoPagina *pai,
                   int idxNoPai, bool *cresceu,
                   int *chaveSubiu, TipoPagina **noSubiu,
                   Dados *dados) {

    *cresceu = false;

    /* ── Página folha: inserção efetiva ── */
    if (Ap->Pt == Folha) {
        if (Ap->UU.U1.ne < MM) {
            /* Há espaço: insere diretamente */
            InsereNaFolha(Ap, reg);
            return;
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

        /* Tenta redistribuir com irmão direito */
        if (pai != NULL && idxNoPai < pai->UU.U0.ni) {
            TipoPagina *irmaoDir = pai->UU.U0.pi[idxNoPai + 1];
            if (irmaoDir->UU.U1.ne < MM) {
                InsereNaFolha(Ap, reg);
                RedistribuiFolha(pai, idxNoPai, Ap, irmaoDir);
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

void InsereBE(Registro reg, TipoPagina **raiz, char *situacao, Dados *dados) {
    bool cresceu;
    int chaveSubiu;
    TipoPagina *noSubiu;

    InsRec(&reg, *raiz, NULL, 0, &cresceu, &chaveSubiu, &noSubiu, dados);

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

/* ------------------------------------------------------------------ */
/*  Entry point                                                         */
/* ------------------------------------------------------------------ */

void ArvoreBE(FILE *arquivo, int quantidade, char *situacao,
              Registro *resultado, Dados *dados) {

    if (arquivo == NULL) {
        printf("Erro: arquivo inválido\n");
        return;
    }

    TipoPagina *raiz = NovaFolha();  // árvore começa com uma folha vazia
    Registro reg;

    for (int i = 0; i < quantidade; i++) {
        if (fread(&reg, sizeof(Registro), 1, arquivo) != 1) {
            printf("Erro ao ler registro %d\n", i);
            break;
        }
        InsereBE(reg, &raiz, situacao, dados);
    }

    PesquisaBE(resultado, &raiz, dados);
    LiberaArvore(raiz);
}