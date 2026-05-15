#include "arvore.h"

void ArvoreBinariaDePesquisa(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA){
    //Realiza a transcrição do arquivo original para um outro arquivo, onde os registros são organizados em uma árvore binária de pesquisa

    

    criarArvore(situacao, quantidade, TAM_PAGINA);
    if((arquivo = fopen("arvore.bin", "rb")) == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    
    ArvoreBuscaBinaria(arquivo, situacao, resultado, dados, TAM_PAGINA);
    fclose(arquivo);
}

bool ArvoreBuscaBinaria(FILE* arquivo, char* situacao, Registro* resultado, Dados* dados, int TAM_PAGINA){
    //O arquivo contem uma arvore, em que cada item do arquivo é um nó da árvore, 
    //e cada nó tem um registro e dois endereços, neste caso, indices, para os filhos esquerdo e direito, 
    //ex: o item 10 é o primeiro item do arquivo e possui dois filhos, os itens 5 e 15, filho esq e dir, respectivamente. 
    //internamente, o arquivo guarda o registro como item: 10, filho esq: 1 e filho dir: 2, onde os filhos são os indices dos itens do arquivo.
    //A função ArvoreBuscaBinaria é chamada recursivamente para percorrer a árvore, comparando a chave do resultado com a chave do nó atual, 
    //e decidindo se deve ir para o filho esquerdo ou direito, até encontrar a chave ou chegar em um nó folha.
    
    No pagina[TAM_PAGINA];
    No raiz;

    printf("Lendo a raiz da arvore...\n");

    dados->transferencias.indexacao++;//contabiliza a transferência da raiz da árvore, que é o primeiro item do arquivo, ou seja, o item 0
    fread(pagina, sizeof(No), TAM_PAGINA, arquivo);//lê a primeira página do arquivo para o vetor, onde pagina[0] é o primeiro item do arquivo, pagina[1] é o segundo, e assim por diante. 
    
    raiz = pagina[0];
    
    int paginaCarregada = 0; // página 0 já foi lida no fread inicial
    return Busca(arquivo, &raiz, 0, pagina, resultado, dados, &paginaCarregada, TAM_PAGINA);//começa a busca na raiz da árvore, que é o primeiro item do arquivo, ou seja, o item 0

    //
}

bool Busca(FILE* arquivo, No* no, int indice, No* pagina, Registro* resultado, Dados* dados, int *paginaCarregada, int TAM_PAGINA){
    //Busca a chave do resultado no nó do arquivo indicado pelo indice, e retorna o registro encontrado ou NULL se não encontrar
    //A função é chamada recursivamente para percorrer a árvore, comparando a chave do resultado com a chave do nó atual, 
    //e decidindo se deve ir para o filho esquerdo ou direito, até encontrar a chave ou chegar em um nó folha.

    if(indice == -1){//alcançou um nó folha, ou seja, a chave não foi encontrada
        printf("Chave %d não encontrada na árvore!\n", resultado->chave);
        return false;
    }
    //int paginaAtual = indice / TAM_PAGINA;
    //if(indice < paginas[paginaAtual] || indice >= paginas[paginaAtual + 1]){

    int paginaNecessaria = indice / TAM_PAGINA;

    if(paginaNecessaria != *paginaCarregada){
        fseek(arquivo, paginaNecessaria * TAM_PAGINA * sizeof(No), SEEK_SET);
        dados->transferencias.pesquisa++;//contabiliza a transferência da página necessária para acessar o nó desejado 
        fread(pagina, sizeof(No), TAM_PAGINA, arquivo);
        *paginaCarregada = paginaNecessaria;
    }

    *no = pagina[indice % TAM_PAGINA];


    printf("Comparando a chave %d com a chave do nó atual %d...\n", resultado->chave, no->reg.chave);

    dados->comparacoes.pesquisa++;//contabiliza a comparação da chave do resultado com a chave do nó atual
    if(no->reg.chave == resultado->chave){
        *resultado = no->reg;
        printf("Chave %d encontrada na árvore!\n", resultado->chave);
        return true;
    }
    else if(no->reg.chave > resultado->chave)
        return Busca(arquivo, no, no->indiceEsq, pagina, resultado, dados, paginaCarregada, TAM_PAGINA);
    else
        return Busca(arquivo, no, no->indiceDir, pagina, resultado, dados, paginaCarregada, TAM_PAGINA);
}

void criarArvore(char *situacao, int quantidade, int TAM_PAGINA){


    FILE *arquivoArvore = fopen("arvore.bin", "wb+");
    FILE *registro = fopen("registros.bin", "rb");

    Registro pagina[TAM_PAGINA];
    No noCopia;
    int lidos;

    while((lidos = fread(pagina, sizeof(Registro), TAM_PAGINA, registro)) > 0){

        

        for(int i = 0; i < lidos; i++){

            noCopia.reg = pagina[i];
            noCopia.indiceDir = -1;
            noCopia.indiceEsq = -1;
            inserirArvore(arquivoArvore,&noCopia);
            
            

        }



    }

    



    fclose(arquivoArvore);
    fclose(registro);

}


void inserirArvore(FILE *arquivoArvore, No *noCopia){

    No noRegistro;


    
    fseek(arquivoArvore, 0, SEEK_END);

    if(ftell(arquivoArvore) == 0){
        fwrite(noCopia, sizeof(No), 1, arquivoArvore);
        return;
    }

    fseek(arquivoArvore, 0, SEEK_SET);


    while(fread(&noRegistro, sizeof(No), 1, arquivoArvore) > 0){

        if(noRegistro.reg.chave < noCopia->reg.chave){

            if(noRegistro.indiceDir == -1){

                fseek(arquivoArvore, -(long)sizeof(No), SEEK_CUR);

                long posAnterior = ftell(arquivoArvore);

                fseek(arquivoArvore, 0, SEEK_END);
                long arquivoFinal = ftell(arquivoArvore);
                int indice = arquivoFinal/(sizeof(No));
                fwrite(noCopia, sizeof(No), 1, arquivoArvore);

                fseek(arquivoArvore, posAnterior, SEEK_SET);
                noRegistro.indiceDir = indice;

                fwrite(&noRegistro, sizeof(No), 1, arquivoArvore);

                return;


            }else{
                
                fseek(arquivoArvore, sizeof(No)*noRegistro.indiceDir, SEEK_SET);


            }


        }else if(noRegistro.reg.chave > noCopia->reg.chave){

            if(noRegistro.indiceEsq == -1){
                fseek(arquivoArvore, -(long)sizeof(No), SEEK_CUR);

                long posAnterior = ftell(arquivoArvore);

                fseek(arquivoArvore, 0, SEEK_END);
                long arquivoFinal = ftell(arquivoArvore);
                long indice = arquivoFinal/(sizeof(No));
                fwrite(noCopia, sizeof(No), 1, arquivoArvore);

                fseek(arquivoArvore, posAnterior, SEEK_SET);
                noRegistro.indiceEsq = indice;

                fwrite(&noRegistro, sizeof(No), 1, arquivoArvore);
                

                return;
                
            }else{

                fseek(arquivoArvore, sizeof(No)*noRegistro.indiceEsq, SEEK_SET);

            }


        }else{

            printf("Valor duplicado na árvore!\n");
            return;


        }


    }


}



// arvore.h (campos relevantes inferidos do código)
// typedef struct {
//     int chave;
//     // outros campos...
// } Registro;
//
// typedef struct {
//     Registro reg;
//     int indiceEsq;
//     int indiceDir;
// } No;

void imprimirNoArvore(FILE *arquivo, int indice, int nivel) {
    if (indice == -1)
        return;

    No no;

    if (fseek(arquivo, (long)indice * sizeof(No), SEEK_SET) != 0) {
        fprintf(stderr, "Erro ao posicionar no índice %d\n", indice);
        return;
    }

    if (fread(&no, sizeof(No), 1, arquivo) != 1) {
        fprintf(stderr, "Erro ao ler o nó no índice %d\n", indice);
        return;
    }

    // Imprime indentação de acordo com o nível
    for (int i = 0; i < nivel; i++)
        printf("    ");

    printf("Índice: %d | Chave: %d | Esq: %d | Dir: %d\n",
           indice, no.reg.chave, no.indiceEsq, no.indiceDir);

    imprimirNoArvore(arquivo, no.indiceEsq, nivel + 1);
    imprimirNoArvore(arquivo, no.indiceDir, nivel + 1);
}

void imprimirArvore(const char *nomeArquivo) {
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (arquivo == NULL) {
        fprintf(stderr, "Erro ao abrir o arquivo '%s'\n", nomeArquivo);
        return;
    }

    fseek(arquivo, 0, SEEK_END);
    long long tamanho = ftell(arquivo);
    if (tamanho == 0) {
        printf("Árvore vazia.\n");
        fclose(arquivo);
        return;
    }

    int totalNos = (int)(tamanho / sizeof(No));
    printf("=== Árvore Binária de Pesquisa (%d nós) ===\n\n", totalNos);

    imprimirNoArvore(arquivo, 0, 0);

    printf("\n==========================================\n");
    fclose(arquivo);
}