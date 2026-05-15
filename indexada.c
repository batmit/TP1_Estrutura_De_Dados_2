#include "indexada.h"


bool buscaBin(Registro* registros, int chave, int l, int r, Registro* resultado, Comparacao* comparacoes);
bool BuscaBinaria(Registro* registros, int quantidade, char* situacao, Registro* resultado, Dados* dados);
bool BuscaSequencial(Registro* registros, int quantidade, Registro* resultado, Dados* dados);
void embaralhar(Registro* v, int tamanho);


bool BuscaSequencialIndexada(FILE* arquivo, int quantidade, char* situacao, Registro* resultado, Dados* dados){

    if(strcmp(situacao, "aleatoria") == 0){
        printf("Busca sequencial indexada nao eh adequada para registros em ordem aleatoria...\n");
        return false;
    }
    if(strcmp(situacao, "crescente") != 0 && strcmp(situacao, "decrescente") != 0){
        printf("Situacao desconhecida: %s\n", situacao);
        return false;
    }

    printf("Busca Sequencial Indexada\n");
    //Cria o índice
    int *chaves = (int*) malloc(sizeof(int) * (quantidade / TAM_PAGINA + 1));//+1 caso quantidade % TAM_PAGINA != 0
    //Guarda uma pagina de registros
    // Trocar o nome de reg para pagina
    Registro reg[TAM_PAGINA]; 
    int pos = 0;

    if((arquivo = fopen("registros.bin", "rb")) == NULL){
        printf("Erro ao abrir o arquivo\n");
        return false;
    }

    int n;
    printf("Lendo o arquivo e criando o indice...\n");
    while((n = fread(reg, sizeof(Registro), TAM_PAGINA, arquivo)) > 0) {
        //'n' diz quantos registros foram lidos
        //Se n > 0, reg[0] sempre terá um dado novo e válido.
        dados->transferencias.indexacao++; //contabiliza as transferências de registros do arquivo para a memória
        chaves[pos++] = reg[0].chave; 
    }

    printf("Indice criado com %d chaves\n", pos);
    printf("Realizando a busca pela chave %d...\n", resultado->chave);

    if(strcmp(situacao, "crescente") == 0 || strcmp(situacao, "decrescente") == 0){//busca apenas funciona para arquivos em ordem crescente ou decrescente
        if(pesquisaSequencial(chaves, pos, situacao, arquivo, resultado, dados)){
            printf("Chave encontrada: %d\n", resultado->chave);
            printf("Dado1: %ld\n", resultado->dado1);
            printf("Dado2: %s\n", resultado->dado2);
            printf("Dado3: %s\n", resultado->dado3);
            fclose(arquivo);
            free(chaves);
            return true;
        }
        else{
            printf("Chave %d nao encontrada\n", resultado->chave);//busca foi possivel mas a chave não foi encontrada
            fclose(arquivo);
            free(chaves);
            return false;
        }
    }
    return false;
}

bool pesquisaSequencial(int *chaves, int pos, char* situacao, FILE* arquivo, Registro* resultado, Dados* dados){
    //recebe a situacao para saber se o arquivo está em ordem crescente ou decrescente
    printf("Realizando pesquisa sequencial no indice...\n");
    Registro pagina[TAM_PAGINA];
    int i = 0, quantItens;
    long deslocamento;

    printf("Comparando a chave %d com as chaves do indice...\n", resultado->chave);
    if(strcmp(situacao, "crescente") == 0){
        while(i < pos && chaves[i] <= resultado->chave){//enquanto a chave for maior que a chave do índice, continua procurando
            dados->comparacoes.indexacao++;//compara a chave do resultado com a chave do índice
            i++;//enquanto a chave for maior que a chave do índice, continua procurando
                //i é a página onde a chave pode estar
        }
    }
    else if(strcmp(situacao, "decrescente") == 0){
        while(i < pos && chaves[i] >= resultado->chave){//enquanto a chave for menor que a chave do índice, continua procurando
            dados->comparacoes.indexacao++;//compara a chave do resultado com a chave do índice
            i++;//enquanto a chave for menor que a chave do índice, continua procurando
                //i é a página onde a chave pode estar
        }
    }

    if(i == 0 && strcmp(situacao, "crescente") == 0){
        printf("A chave %d eh menor que a primeira chave do indice, logo não esta no arquivo\n", resultado->chave);
        return false;//se i for 0, a chave é menor que a primeira chave do índice, logo não está no arquivo
    }
    if(i == 0 && strcmp(situacao, "decrescente") == 0){
        printf("A chave %d eh maior que a primeira chave do indice, logo não esta no arquivo\n", resultado->chave);
        return false;//se i for 0, a chave é maior que a primeira chave do índice, logo não está no arquivo
    }


    printf("A chave %d pode estar na pagina %d do arquivo\n", resultado->chave, i);
    printf("Calculando o numero de itens na pagina %d...\n", i);
    if(i < pos)//se a chave não estiver na última página, quantItems é igual a TAM_PAGINA
        quantItens = TAM_PAGINA;
    else{//se a chave estiver na última página, verificamos se a página está completa ou não
        fseek (arquivo, 0, SEEK_END);
        quantItens = (ftell(arquivo) / sizeof(Registro)) % TAM_PAGINA;//verifica quantos itens tem na última página
        if(quantItens == 0)
            quantItens = TAM_PAGINA;//se a última página estiver completa, quantItens é igual a TAM_PAGINA
    }

    printf("Numero de itens na pagina %d: %d\n", i, quantItens);
    printf("Lendo a pagina %d do arquivo...\n", i);
    deslocamento = (i - 1) * TAM_PAGINA * sizeof(Registro);//deslocamento para a página onde a chave pode estar
    fseek(arquivo, deslocamento, SEEK_SET);//posiciona o ponteiro do arquivo no início da página

    dados->transferencias.pesquisa++;//contabiliza a transferência de registros do arquivo para a memória
    fread(pagina, sizeof(Registro), quantItens, arquivo);//lê a página para o vetor pagina

    printf("Realizando busca na pagina %d...\n", i);
    //return BuscaSequencial(pagina, quantItens, resultado, dados);//faz a busca sequencial na página
    return BuscaBinaria(pagina, quantItens, situacao, resultado, dados);//faz a busca binária na página

}
bool buscaBin(Registro* registros, int chave, int l, int r, Registro* resultado, Comparacao* comparacoes){
    int meio = l + (r - l) / 2;
    if(l > r){
        resultado = NULL;
        return false;
    }
    comparacoes->pesquisa++;
    if(registros[meio].chave == chave){
        *resultado = registros[meio];
        return true;
    }
    else if(registros[meio].chave < chave)
        return buscaBin(registros, chave, meio + 1, r, resultado, comparacoes);
    else
        return buscaBin(registros, chave, l, meio - 1, resultado, comparacoes);

}   

bool buscaBinDecrescente(Registro* registros, int chave, int l, int r, Registro* resultado, Comparacao* comparacoes){
    int meio = l + (r - l) / 2;
    if(l > r){
        resultado = NULL;
        return false;
    }
    comparacoes->pesquisa++;
    if(registros[meio].chave == chave){
        *resultado = registros[meio];
        return true;
    }
    else if(registros[meio].chave > chave)
        return buscaBinDecrescente(registros, chave, meio + 1, r, resultado, comparacoes);
    else
        return buscaBinDecrescente(registros, chave, l, meio - 1, resultado, comparacoes);

}

bool BuscaBinaria(Registro* registros, int quantidade, char* situacao, Registro* resultado, Dados* dados){
    int inicio = 0, fim = quantidade - 1;
    if(strcmp(situacao, "crescente") == 0)
        return buscaBin(registros, resultado->chave, inicio, fim, resultado, &dados->comparacoes);
    else if(strcmp(situacao, "decrescente") == 0)
        return buscaBinDecrescente(registros, resultado->chave, inicio, fim, resultado, &dados->comparacoes);
    else if(strcmp(situacao, "aleatoria") == 0){
        printf("Busca binaria nao eh adequada para registros em ordem aleatoria...\n");
        return false;
    }
    else{
        printf("Situacao desconhecida: %s\n", situacao);
        return false;
    }
    return buscaBin(registros, resultado->chave, inicio, fim, resultado, &dados->comparacoes);
}

bool BuscaSequencial(Registro* registros, int quantidade, Registro* resultado, Dados* dados){
    for(int i = 0; i < quantidade; i++){
        dados->comparacoes.pesquisa++;
        if(registros[i].chave == resultado->chave){
            *resultado = registros[i];
            return true;
        }
    }
    return false;
}

void criaCrescente(int quantidade){
    FILE* arquivo;
    if((arquivo = fopen("registros.bin", "wb")) == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    Registro reg;

    for(int i = 0; i < quantidade; i++){
        reg.chave = i;
        reg.dado1 = i * 10;
        sprintf(reg.dado2, "%d", reg.chave);
        sprintf(reg.dado3, "%d", reg.chave * 10); 
        fwrite(&reg, sizeof(Registro), 1, arquivo);
    }
    fclose(arquivo);
}

void criaDecrescente(int quantidade){
    FILE* arquivo;
    if((arquivo = fopen("registros.bin", "wb")) == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    Registro reg;

    for(int i = quantidade - 1; i >= 0; i--){
        reg.chave = i;
        reg.dado1 = i * 10;
        sprintf(reg.dado2, "%d", reg.chave);
        sprintf(reg.dado3, "%d", reg.chave * 10);
        fwrite(&reg, sizeof(Registro), 1, arquivo);
    }
    fclose(arquivo);
}

void criaAleatorio(int quantidade){
    FILE* arquivo;
    if((arquivo = fopen("registros.bin", "wb")) == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }

    Registro pag[TAM_PAGINA];
    int tam = TAM_PAGINA;
    // Alterar forma que chave eh escolhida para evitar numeros repetidos
    int cont = 0;
    for(int i = 0; i < quantidade / TAM_PAGINA + 1; i++){

        if(i == quantidade / TAM_PAGINA && quantidade % TAM_PAGINA != 0)//se for a última página e a quantidade de registros não for múltipla de TAM_PAGINA, tam é igual ao resto da divisão
            tam = quantidade % TAM_PAGINA;
    
        for(int j = 0; j < tam; j++){

            pag[j].chave = cont; // Gera uma chave aleatória
            pag[j].dado1 = pag[j].chave * 10;
            sprintf(pag[j].dado2, "%d", pag[j].chave); // Converte o valor para string
            sprintf(pag[j].dado3, "%d", pag[j].chave); // Converte o valor para string
            
            cont++;
        }

        embaralhar(pag, tam);

        fwrite(&pag, sizeof(Registro), tam, arquivo);
    }
    fclose(arquivo);
}   

void CriaArquivo(int quantidade, char* situacao){
    if(strcmp(situacao, "crescente") == 0){
        printf("Criando arquivo com registros em ordem crescente...\n");
        criaCrescente(quantidade);
    }
    else if(strcmp(situacao, "decrescente") == 0){
        printf("Criando arquivo com registros em ordem decrescente...\n");
        criaDecrescente(quantidade);
    }
    else if(strcmp(situacao, "aleatoria") == 0){
        printf("Criando arquivo com registros em ordem aleatoria...\n");
        criaAleatorio(quantidade);
    }
    else{
        printf("Situacao desconhecida: %s\n", situacao);
        return;
    }
}

void embaralhar(Registro* v, int tamanho) {
    for(int i = tamanho - 1; i > 0; i--){
        int j = rand() % (i + 1);
        Registro tmp = v[i];
        v[i] = v[j];
        v[j] = tmp;
    }
}

