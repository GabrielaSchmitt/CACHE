#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "cache.c"

DADOS * leitura_arquivo(char * nome_arquivo);
CACHE * cria_cache(int linhas_cache, int associatividade);
void imprime_cache(CACHE * cache, DADOS * dados);

int main(int argc, char **argv) {
    DADOS *dados = leitura_arquivo(argv[1]);
    CACHE *cache = cria_cache(dados->cabecalho->linhas_cache, dados->cabecalho->associatividade);
    cache = executa_requisicoes(cache, dados);
    imprime_cache(cache, dados);
    free(dados);
    free(cache);
    return EXIT_SUCCESS;
}

void imprime_cache(CACHE * cache, DADOS * dados) {
    printf("%u %u %u FIFO WB WA %u %u %u\n",
        dados->cabecalho->tam_cache,
        dados->cabecalho->associatividade,
        dados->cabecalho->tam_bloco,
        dados->cabecalho->tempo_acesso_cache,
        dados->cabecalho->leitura_dram,
        dados->cabecalho->escrita_dram
    );
    fflush(stdout);
    printf("%u\n%u %u\n%u\n", dados->tempo_total, dados->hit, dados->miss, dados->cabecalho->linhas_cache);
    fflush(stdout);
    int j = 0;
    for (int i = 0; i < dados->cabecalho->linhas_cache; i++) {
        for (j = 0; j < dados->cabecalho->associatividade - 1; j++) {
            if (cache[i].bloco[j].validade != 0) {
                printf("%u ", cache[i].bloco[j].dado);
                fflush(stdout);
            } else {
                printf("NULL ");
                fflush(stdout);
            }
        }
        if (cache[i].bloco[j].validade != 0) {
            printf("%u\n", cache[i].bloco[j].dado);
            fflush(stdout);
        } else {
            printf("NULL\n");
            fflush(stdout);
        }
    }
}

CACHE * cria_cache(int linhas_cache, int associatividade) {
    CACHE * cache = (CACHE *) malloc(sizeof(CACHE) * linhas_cache);

    for (int i = 0; i < linhas_cache; i++) {
        cache[i].indice = i;
        cache[i].bloco = (BLOCO *) malloc(sizeof(BLOCO) * associatividade);
        for (int j = 0; j < associatividade; j++) {
            cache[i].bloco[j].ciclo = 0;
            cache[i].bloco[j].tag = 0;
            cache[i].bloco[j].validade = 0;
            cache[i].bloco[j].dado = 0;
        }
    }
    return cache;
}

DADOS * leitura_arquivo(char * nome_arquivo) {
    DADOS *dados = (DADOS *) malloc(sizeof(DADOS));
    CABECALHO * cabecalho = (CABECALHO *) malloc(sizeof(CABECALHO));
    cabecalho->associatividade = 16;

    FILE * arquivo;
    arquivo = fopen(nome_arquivo, "r");
    fscanf(arquivo,
        "%u %u %u %u %u\n",
        &cabecalho->tam_cache,
        &cabecalho->tam_bloco,
        &cabecalho->tempo_acesso_cache,
        &cabecalho->leitura_dram,
        &cabecalho->escrita_dram
    );
    fscanf(arquivo, "%u", &cabecalho->num_requisicoes);
    cabecalho->linhas_cache = cabecalho->tam_cache / cabecalho->tam_bloco / cabecalho->associatividade;
    cabecalho->tam_offset = log2(cabecalho->tam_bloco);
    cabecalho->tam_indice = log2(cabecalho->linhas_cache);

    REQUISICAO *requisicoes = (REQUISICAO *) malloc(sizeof(REQUISICAO) * cabecalho->num_requisicoes);
    for (int i = 0; i < cabecalho->num_requisicoes; i++) {
        fscanf(arquivo,
            "%u %c %u",
            &requisicoes[i].ciclo,
            &requisicoes[i].tipo,
            &requisicoes[i].endereco
        );
    }
    fclose(arquivo);

    for (int i = 0; i < cabecalho->num_requisicoes; i++) {
        requisicoes[i].indice = (requisicoes[i].endereco >> cabecalho->tam_offset) & (cabecalho->linhas_cache - 1);
        requisicoes[i].tag = requisicoes[i].endereco >> (cabecalho->tam_offset + cabecalho->tam_indice);
    }
    dados->cabecalho = cabecalho;
    dados->requisicoes = requisicoes;
    dados->miss = 0;
    dados->hit = 0;
    dados->tempo_total = 0;
    return dados;
}