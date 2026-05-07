#include "arvore.h"

void ArvoreBinariaDePesquisa(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados){
    //Realiza a transcrição do arquivo original para um outro arquivo, onde os registros são organizados em uma árvore binária de pesquisa

    if(fopen("registros.bin", "rb") == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    //Guarda as chaves do índice
    printf("Criando indice...\n");
    int *paginas = (int*) malloc(sizeof(int) * (quantidade / TAM_PAGINA + 1));//+1 caso quantidade % TAM_PAGINA != 0
    for(int i = 0; i < quantidade / TAM_PAGINA + 1; i++){
        paginas[i] = i * TAM_PAGINA;//paginas[i] guarda o indice do primeiro item da página i no arquivo. ex: paginas[0] = 0, paginas[1] = 30, paginas[2] = 60, e assim por diante.
    }
    //paginas[i] guarda o indice do primeiro item da página i no arquivo. ex: paginas[0] = 0, paginas[1] = 30, paginas[2] = 60, e assim por diante.
    //Guarda uma pagina de registros

    ArvoreBuscaBinaria(arquivo, situacao, paginas, resultado, dados);

    free(paginas);
}

bool ArvoreBuscaBinaria(FILE* arquivo, char* situacao, int* paginas, Registro* resultado, Dados* dados){
    //O arquivo contem uma arvore, em que cada item do arquivo é um nó da árvore, 
    //e cada nó tem um registro e dois endereços, neste caso, indices, para os filhos esquerdo e direito, 
    //ex: o item 10 é o primeiro item do arquivo e possui dois filhos, os itens 5 e 15, filho esq e dir, respectivamente. 
    //internamente, o arquivo guarda o registro como item: 10, filho esq: 1 e filho dir: 2, onde os filhos são os indices dos itens do arquivo.
    //A função ArvoreBuscaBinaria é chamada recursivamente para percorrer a árvore, comparando a chave do resultado com a chave do nó atual, 
    //e decidindo se deve ir para o filho esquerdo ou direito, até encontrar a chave ou chegar em um nó folha.
    
    No pagina[TAM_PAGINA];
    No* raiz = NULL;

    printf("Lendo a raiz da arvore...\n");
    fread(pagina, sizeof(No), TAM_PAGINA, arquivo);//lê a primeira página do arquivo para o vetor, onde pagina[0] é o primeiro item do arquivo, pagina[1] é o segundo, e assim por diante. 
    dados->transferencias.indexacao++;//contabiliza a

    return Busca(arquivo, raiz, 0, paginas, pagina, resultado, dados);//começa a busca na raiz da árvore, que é o primeiro item do arquivo, ou seja, o item 0

    //
}

bool Busca(FILE* arquivo, No* no, int indice, int* paginas, No* pagina, Registro* resultado, Dados* dados){
    //Busca a chave do resultado no nó do arquivo indicado pelo indice, e retorna o registro encontrado ou NULL se não encontrar
    //A função é chamada recursivamente para percorrer a árvore, comparando a chave do resultado com a chave do nó atual, 
    //e decidindo se deve ir para o filho esquerdo ou direito, até encontrar a chave ou chegar em um nó folha.

    if(indice == -1)//alcançou um nó folha, ou seja, a chave não foi encontrada
        return false;

    if(indice < paginas[indice] || indice >= paginas[indice + 1]){//Se o indice do nó atual nao estiver na pagina atual, realiza a leitura da nova pagina
        int paginaAtual = indice / TAM_PAGINA;//paginaAtual é o indice da página onde esta o nó indicado pelo indice
        fseek(arquivo, paginaAtual * TAM_PAGINA * sizeof(No), SEEK_SET);//posiciona o ponteiro do arquivo no início da página onde esta o nó indicado pelo indice
        fread(pagina, sizeof(No), TAM_PAGINA, arquivo);//lê o nó indicado pelo indice para a memória
        dados->transferencias.pesquisa++;//contabiliza a transferência de registros do arquivo para a memória
    }

    *no = pagina[indice % TAM_PAGINA];//indice % TAM_PAGINA é o indice do nó na página lida para a memória, ou seja, o indice do nó na página atual

    printf("Comparando a chave %d com a chave do nó atual %d...\n", resultado->chave, no->reg.chave);
    dados->comparacoes.pesquisa++;//contabiliza a comparação da chave do resultado com a chave do nó atual
    if(no->reg.chave == resultado->chave){
        *resultado = no->reg;
        return true;
    }
    else if(no->reg.chave > resultado->chave)
        return Busca(arquivo, no, no->indiceEsq, paginas, pagina, resultado, dados);
    else
        return Busca(arquivo, no, no->indiceDir, paginas, pagina, resultado, dados);
}