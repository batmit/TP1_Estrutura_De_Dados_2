#include "arvoreB.h"

void Inicializa(TipoPagina** raiz) {
    *raiz = NULL;
}


void ArvoreB(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA) {
    
    FILE* registro = fopen("registros.bin", "rb");
    TipoPagina *arvoreB;
    Inicializa(&arvoreB);

    Registro reg;
    
    for(int i = 0; i < quantidade; i++){
        dados->transferencias.indexacao++;//contabiliza a transferência de um registro do arquivo para a memória
        fread(&reg, sizeof(Registro), 1, registro);
        Insere(reg, &arvoreB, situacao, dados);
    }

    Pesquisa(resultado, arvoreB, situacao, dados);
    Destroi(arvoreB);
    fclose(registro);
}


void Pesquisa(Registro *x, TipoPagina* Ap, char* situacao, Dados* dados) {
    long i = 1;
    if(Ap == NULL){
        printf("Registro nao encontrado\n");
        return;
    }
    while(i < Ap->n){
        dados->comparacoes.pesquisa++;//contabiliza a comparação da chave do resultado com a chave do nó atual
        if(x->chave > Ap->r[i - 1].chave)
            i++;
        else
            break;
    }
    if(x->chave == Ap->r[i - 1].chave){
        dados->comparacoes.pesquisa++;//contabiliza a comparação da chave do resultado com a chave do nó atual
        *x = Ap->r[i - 1];
        printf("Registro encontrado: %d\n", x->chave);
        printf("Dado1: %ld\n", x->dado1);
        printf("Dado2: %s\n", x->dado2);
        printf("Dado3: %s\n", x->dado3);
        return;
    }
    dados->comparacoes.pesquisa++;//contabiliza a comparação da chave do resultado com a chave do nó atual
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
    while(NaoAchouPosicao){//busca a posição onde o registro deve ser inserido
        if(reg.chave >= Ap->r[k-1].chave){//se a chave de reg for maior, encontrou a posição onde o registro deve ser inserido
            NaoAchouPosicao = false;
            break;
        }
        Ap->r[k] = Ap->r[k-1];//desloca os registros para a direita para abrir espaço para o novo registro
        Ap->p[k+1] = Ap->p[k];
        k--;
        if(k < 1)//se chegou no início da página, encontrou a posição onde o registro deve ser inserido
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
    if(Ap == NULL){//caso base: chegou em uma página folha
        *cresceu = true;
        (*ApRetorno) = NULL;
        *resultado = reg;
        return;//insere o registro na página folha
    }
    while(i < Ap->n && reg.chave > Ap->r[i - 1].chave){//busca a posição onde o registro deve ser inserido
        i++;
        dados->comparacoes.indexacao++;//contabiliza a comparação da chave do resultado com a chave do nó atual
    }
    if(reg.chave == Ap->r[i - 1].chave){//registro já existe na árvore, não insere
        printf("Registro ja existe\n");
        *cresceu = false;
        return;
    }
    if(reg.chave < Ap->r[i - 1].chave)//se a chave do registro é menor que a chave do nó atual, desce para o filho esquerdo
        i--;
    Ins(reg, Ap->p[i], cresceu, resultado, ApRetorno, dados);//chama recursivamente a função para descer na árvore até chegar em uma página folha
    if(!*cresceu)//se o registro já foi inserido na página folha, não precisa fazer mais nada
        return;
    if(Ap->n < MM){//se a página atual tem espaço para inserir o registro, insere diretamente na página
        InsereNaPagina(Ap, *resultado, *ApRetorno);
        *cresceu = false;//registro inserido, não precisa subir na árvore
        return;
    }

    ApTemp = (TipoPagina*) malloc(sizeof(TipoPagina));//cria uma nova página para dividir a página atual, caso a página atual esteja cheia
    ApTemp->n = 0;
    ApTemp->p[0] = NULL;

    if(i < M + 1){//se o registro deve ser inserido na primeira metade da página, 
                  //insere o último registro da página antiga na nova página e insere o registro na página atual
        InsereNaPagina(ApTemp, Ap->r[MM - 1], Ap->p[MM]);//insere na pagina ApTemp, o registro da posicao MM-1 da pagina Ap, 
                                                         //e o ponteiro da posicao MM da pagina Ap
        Ap->n--;
        InsereNaPagina(Ap, *resultado, *ApRetorno);
    }
    else
        InsereNaPagina(ApTemp, *resultado, *ApRetorno);//se o registro deve ser inserido na segunda metade da página, 
                                                       //insere o registro diretamente na nova página

    for(j = M + 2; j <= MM; j++)
        InsereNaPagina(ApTemp, Ap->r[j - 1], Ap->p[j]);//insere a segunda metade da pagina antiga na nova pagina
    
    Ap->n = M;//pagina antiga agora tem apenas a primeira metade dos registros
    ApTemp->p[0] = Ap->p[M + 1];//primeiro ponteiro da nova pagina aponta para o ponteiro da posicao M+1 da pagina antiga
    *resultado = Ap->r[M];//registro do meio da pagina antiga sobe para a pagina pai
    *ApRetorno = ApTemp;
}

void Insere(Registro reg, TipoPagina** Ap, char* situacao, Dados* dados){
    bool cresceu;
    Registro resultado;
    TipoPagina* ApRetorno;

    Ins(reg, *Ap, &cresceu, &resultado, &ApRetorno, dados);
    if(cresceu){//se cresceu após a inserção, é necessário criar uma nova página para ser a nova raiz da árvore
        TipoPagina* ApTemp = (TipoPagina*) malloc(sizeof(TipoPagina));
        ApTemp->n = 1;
        ApTemp->r[0] = resultado;
        ApTemp->p[0] = *Ap;
        ApTemp->p[1] = ApRetorno;
        *Ap = ApTemp;
    }
}

void Destroi(TipoPagina* Ap){
    if(Ap == NULL)
        return;
    for(int i = 0; i <= Ap->n; i++)
        Destroi(Ap->p[i]);
    free(Ap);
}