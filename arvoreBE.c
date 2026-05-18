#include "arvoreBE.h"
 

void InicializaExterna(TipoPagina* Ap){//inicializa uma página externa vazia
    Ap->Pt = Externa;
    Ap->UU.U1.ne = 0;
}
 
void InicializaInterna(TipoPagina* Ap){//inicializa uma página interna vazia
    Ap->Pt = Interna;
    Ap->UU.U0.ni = 0;
    for (int i = 0; i <= MM; i++)
        Ap->UU.U0.pi[i] = NULL;
}
 
TipoPagina* NovaExterna(){//aloca e inicializa uma nova página externa
    TipoPagina* p = (TipoPagina*) malloc(sizeof(TipoPagina));
    InicializaExterna(p);
    return p;
}
 

TipoPagina* NovaInterna(){//aloca e inicializa uma nova página interna
    TipoPagina* p = (TipoPagina*) malloc(sizeof(TipoPagina));
    InicializaInterna(p);
    return p;
}
 

void InsereNaPaginaExterna(TipoPagina* Ap, Registro reg){
    int i = Ap->UU.U1.ne;
    /* desloca registros maiores para abrir espaço */
    while (i > 0 && reg.chave < Ap->UU.U1.re[i - 1].chave) {//enquanto a chave for menor que a chave do registro na posição i-1, 
                                                            //desloca o registro para a direita
        Ap->UU.U1.re[i] = Ap->UU.U1.re[i - 1];
        i--;
    }
    Ap->UU.U1.re[i] = reg;//insere a chave na posição correta
    Ap->UU.U1.ne++;
}
 
void InsereNaPaginaInterna(TipoPagina* Ap, int chave, TipoPagina* ApDir){//realiza o mesmo processo de deslocamento para abrir espaço
    int k = Ap->UU.U0.ni;
    while (k > 0 && chave < Ap->UU.U0.ri[k - 1]) {
        Ap->UU.U0.ri[k]      = Ap->UU.U0.ri[k - 1];
        Ap->UU.U0.pi[k + 1]  = Ap->UU.U0.pi[k];//agora também desloca os ponteiros para a direita
        k--;
    }
    Ap->UU.U0.ri[k]     = chave;
    Ap->UU.U0.pi[k + 1] = ApDir;//(ApDir é o ponteiro para a página que deve ser inserida à direita da chave)
    Ap->UU.U0.ni++;
}
 
/* =========================================================
 * InicializaBE  — inicializa a raiz como NULL
 * ========================================================= */
void InicializaBE(TipoPagina* Ap){
    /* Ap é apenas usado como sinalizador; a raiz real é um ponteiro
     * gerenciado pelo chamador (TipoPagina**). Esta função existe
     * para manter a interface simétrica com a árvore B. */
    (void)Ap; /* raiz começa como NULL no chamador */
}


 
/* =========================================================
 * InsBE — função recursiva de inserção
 *
 * Parâmetros de saída:
 *   Cresceu   — indica se a página atual precisou de split
 *   RegRetorno — chave que sobe ao pai após split
 *   ApRetorno  — ponteiro para a nova página criada no split
 * ========================================================= */
 void InsBE(Registro reg, TipoPagina* Ap, bool* Cresceu, int* ChaveRetorno, TipoPagina** ApRetorno, Dados* dados){
 
    /* --------------------------------------------------
     * CASO BASE 1: árvore vazia — cria primeira folha
     * -------------------------------------------------- */
    if (Ap == NULL){
        TipoPagina* folha = NovaExterna();//cria uma nova página externa para ser a raiz da árvore
        InsereNaPaginaExterna(folha, reg);//insere o primeiro registro na página externa
        *Cresceu = true;
        *ChaveRetorno = reg.chave;//chave retorno sobe para a nova raiz da árvore, que é a chave do registro inserido
        *ApRetorno = folha;
        return;
    }
 
    /* --------------------------------------------------
     * CASO BASE 2: chegou numa página EXTERNA (folha)
     * -------------------------------------------------- */
    if (Ap->Pt == Externa){
        for (int i = 0; i < Ap->UU.U1.ne; i++) {//busca por uma chave igual na página externa, para evitar chaves duplicadas
            dados->comparacoes.indexacao++;
            if (reg.chave == Ap->UU.U1.re[i].chave) {
                printf("Registro já existe\n");
                *Cresceu = false;
                return;
            }
        }
 
        if (Ap->UU.U1.ne < MM2){//verifica se há espaço para inserção
            InsereNaPaginaExterna(Ap, reg);//insere diretamente caso haja espaço
            *Cresceu = false;
        }
        else{//se a página externa estiver cheia, é necessário dividir a página
            TipoPagina* NovaFolha = NovaExterna();
 
            Registro temp[MM2 + 1];//guarda temporariamente os registros da pagina
            int total = 0;
            
            for (int i = 0; i < Ap->UU.U1.ne; i++)//transfere os registros da página para o vetor temporário
                temp[total++] = Ap->UU.U1.re[i];
            
            int pos = total;
            while (pos > 0 && reg.chave < temp[pos - 1].chave) {//busca pela posição onde o novo registro deve ser inserido 
                temp[pos] = temp[pos - 1];//desloca os registros para a direita para abrir espaço para o novo registro
                pos--;
            }
            temp[pos] = reg;
            total++;
 
            //divide o vetor temporário ao meio para distribuir em duas páginas externas
            int meio = total / 2;          

            Ap->UU.U1.ne = meio;//primeira metade dos registros fica na página antiga
            for(int i = 0; i < meio; i++)
                Ap->UU.U1.re[i] = temp[i];
 
            NovaFolha->UU.U1.ne = total - meio;//segunda metade dos registros fica na nova página
            for(int i = 0; i < total - meio; i++)
                NovaFolha->UU.U1.re[i] = temp[meio + i];
 
            *ChaveRetorno = NovaFolha->UU.U1.re[0].chave;//a menor chave da nova página sobe para o pai
            *ApRetorno = NovaFolha;
            *Cresceu = true;//com a página dividida a chave precisa subir na árvore
        }
        return;
    }
 
    /* --------------------------------------------------
     * CASO RECURSIVO: página INTERNA — desce na árvore
     * -------------------------------------------------- */
    long i = 1;
    while(i < Ap->UU.U0.ni){//busca pela posição do filho para descer na árvore
        dados->comparacoes.indexacao++;
        if (reg.chave > Ap->UU.U0.ri[i - 1])
            i++;
        else
            break;
    }
    
    dados->comparacoes.indexacao++;
    if (reg.chave < Ap->UU.U0.ri[i - 1])
        i--;//se a chave for menor, desce pelo filho à esquerda da chave, caso contrário, desce pelo filho à direita da chave
    
    InsBE(reg, Ap->UU.U0.pi[i], Cresceu, ChaveRetorno, ApRetorno, dados);
 
    if(!*Cresceu)
        return;
 
    //se a arvore cresceu após a inserção, é necessário inserir a chave de retorno na página interna atual
    if(Ap->UU.U0.ni < MM){//se tiver espaço na página interna, insere diretamente
        InsereNaPaginaInterna(Ap, *ChaveRetorno, *ApRetorno);
        *Cresceu = false;//não gerou efeito colateral na arvore, pode parar a recursão
        return;
    }
 
    //dividir a página interna atual, caso esteja cheia, para inserir a chave de retorno


    //vetores temporários para guardar as chaves e ponteiros da página interna, mais a nova chave e ponteiro a serem inseridos 
    int tempChaves[MM + 1];
    TipoPagina* tempPtrs[MM + 2];//+2 porque sempre tem um ponteiro a mais que o número de chaves em uma página
 
    tempPtrs[0] = Ap->UU.U0.pi[0];//por ter um ponteiro a mais, é necessário realizar uma transferência a mais para compensar o loop do for

    for (int k = 0; k < Ap->UU.U0.ni; k++) {//transfere as chaves e ponteiros da página interna para os vetores temporários
        tempChaves[k] = Ap->UU.U0.ri[k];
        tempPtrs[k + 1] = Ap->UU.U0.pi[k + 1];
    }
    
    int pos = Ap->UU.U0.ni;
    while(pos > 0 && *ChaveRetorno < tempChaves[pos - 1]){//insere a nova chave e ponteiro nos vetores temporários, buscando pela posição correta
        tempChaves[pos] = tempChaves[pos - 1];
        tempPtrs[pos + 1] = tempPtrs[pos];//desloca os registros e ponteiros para a direita para abrir espaço para a nova chave e ponteiro
        pos--;
    }
    tempChaves[pos] = *ChaveRetorno;
    tempPtrs  [pos + 1] = *ApRetorno;
 
    int totalChaves = MM + 1;
    int meio = totalChaves / 2;//indice da chave que sobe para o pai, que é a chave do meio do vetor temporário
 
    InicializaInterna(Ap);
    Ap->UU.U0.pi[0] = tempPtrs[0];//atualiza a nova página interna com a primeira metade das chaves e ponteiros do vetor temporário
    for(int k = 0; k < meio; k++){
        Ap->UU.U0.ri[k] = tempChaves[k];
        Ap->UU.U0.pi[k + 1] = tempPtrs [k + 1];
        Ap->UU.U0.ni++;
    }

    TipoPagina* NovaInt = NovaInterna();//cria uma nova página interna para receber a segunda metade das chaves e ponteiros do vetor temporário
    NovaInt->UU.U0.pi[0] = tempPtrs[meio + 1];
    for(int k = meio + 1; k < totalChaves; k++){
        NovaInt->UU.U0.ri[NovaInt->UU.U0.ni] = tempChaves[k];
        NovaInt->UU.U0.pi[NovaInt->UU.U0.ni + 1] = tempPtrs[k + 1];
        NovaInt->UU.U0.ni++;
    }
 
    /* a chave do meio sobe para o pai */
    *ChaveRetorno = tempChaves[meio];
    *ApRetorno = NovaInterna;
    *Cresceu = true;
}
 
/* =========================================================
 * InsereBE — interface pública de inserção
 * ========================================================= */
void InsereBE(Registro x, TipoPagina** Ap,
              bool* Cresceu, Registro* RegRetorno, TipoPagina** ApRetorno) {
    /* RegRetorno não é usado na B* (sobe apenas chave int),
     * mantido na assinatura para compatibilidade com o header. */
    int    chaveRetorno = 0;
    TipoPagina* apRet  = NULL;
    Dados  dadosDummy  = {0}; /* sem contagem nesta interface simplificada */
 
    InsBE(x, *Ap, Cresceu, &chaveRetorno, &apRet, &dadosDummy);
 
    if (*Cresceu) {
        /* raiz cresceu: cria nova raiz interna */
        TipoPagina* NovaRaiz = NovaInterna();
        NovaRaiz->UU.U0.ni      = 1;
        NovaRaiz->UU.U0.ri[0]   = chaveRetorno;
        NovaRaiz->UU.U0.pi[0]   = *Ap;      /* filho esquerdo */
        NovaRaiz->UU.U0.pi[1]   = apRet;    /* filho direito  */
        *Ap = NovaRaiz;
        *Cresceu = false;
    }
}
 
/* =========================================================
 * PesquisaBE
 * Desce pelas páginas internas usando apenas as chaves-índice
 * e lê o registro completo ao chegar na página externa.
 * ========================================================= */
void PesquisaBE(Registro* x, TipoPagina** Ap, Dados* dados) {
    TipoPagina* atual = *Ap;
 
    if (atual == NULL) {
        printf("Registro nao encontrado\n");
        return;
    }
 
    /* desce pelas páginas internas */
    while (atual->Pt == Interna) {
        int ni = atual->UU.U0.ni;
        int i  = 0;
        while (i < ni) {
            dados->comparacoes.pesquisa++;
            if (x->chave >= atual->UU.U0.ri[i])
                i++;
            else
                break;
        }
        atual = atual->UU.U0.pi[i];
        if (atual == NULL) {
            printf("Registro nao encontrado\n");
            return;
        }
    }
 
    /* chegou na página externa: busca sequencial */
    for (int i = 0; i < atual->UU.U1.ne; i++) {
        dados->comparacoes.pesquisa++;
        if (x->chave == atual->UU.U1.re[i].chave) {
            *x = atual->UU.U1.re[i];
            printf("Registro encontrado: %d\n", x->chave);
            printf("Dado1: %ld\n",  x->dado1);
            printf("Dado2: %s\n",   x->dado2);
            printf("Dado3: %s\n",   x->dado3);
            return;
        }
    }
    printf("Registro nao encontrado\n");
}
 
/* =========================================================
 * DestruiBE — libera toda a memória da árvore
 * ========================================================= */
void DestruiBE(TipoPagina* Ap) {
    if (Ap == NULL)
        return;
    if (Ap->Pt == Interna) {
        for (int i = 0; i <= Ap->UU.U0.ni; i++)
            DestruiBE(Ap->UU.U0.pi[i]);
    }
    /* páginas externas não têm filhos, apenas libera */
    free(Ap);
}
 
/* =========================================================
 * ArvoreBE — função principal, espelho do ArvoreB()
 * ========================================================= */
void ArvoreBE(FILE* arquivo, int quantidade, char* situacao,
              Registro* resultado, Dados* dados, int TAM_PAGINA) {
 
    (void)arquivo;    /* registros lidos do arquivo fixo abaixo */
    (void)situacao;
    (void)TAM_PAGINA;
 
    FILE* registro = fopen("registros.bin", "rb");
    TipoPagina* arvoreBE = NULL;
 
    Registro reg;
    int    cresceu       = 0;
    Registro regRetorno  = {0};
    TipoPagina* apRetorno = NULL;
 
    for (int i = 0; i < quantidade; i++) {
        dados->transferencias.indexacao++;   /* leitura do arquivo → memória */
        fread(&reg, sizeof(Registro), 1, registro);
 
        /* chama InsBE diretamente para passar dados de comparações */
        int    chaveRet = 0;
        TipoPagina* apRet = NULL;
        InsBE(reg, arvoreBE, &cresceu, &chaveRet, &apRet, dados);
 
        if (cresceu) {
            TipoPagina* NovaRaiz = NovaInterna();
            NovaRaiz->UU.U0.ni     = 1;
            NovaRaiz->UU.U0.ri[0]  = chaveRet;
            NovaRaiz->UU.U0.pi[0]  = arvoreBE;
            NovaRaiz->UU.U0.pi[1]  = apRet;
            arvoreBE = NovaRaiz;
            cresceu  = 0;
        }
    }
 
    PesquisaBE(resultado, &arvoreBE, dados);
    DestruiBE(arvoreBE);
    fclose(registro);
}