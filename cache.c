#include "cache.h"

CACHE * executa_requisicoes(CACHE * cache, DADOS * dados) {
    for (int i = 0; i < dados->cabecalho->num_requisicoes; i++) {
        for (int j = 0; j < dados->cabecalho->linhas_cache; j++) {
            if (dados->requisicoes[i].indice == cache[j].indice) {
                hit = 0;
                for (int k = 0; k < dados->cabecalho->associatividade; k++) {
                    if (cache[j].bloco[k].validade == 1 && cache[j].bloco[k].tag == dados->requisicoes[i].tag) {
                        hit = 1;
                        dados->hit++;
                        if (dados->requisicoes[i].tipo == 'R') {
                            dados->tempo_total += dados->cabecalho->tempo_acesso_cache;
                        } else {
                            dados->tempo_total += dados->cabecalho->tempo_acesso_cache;
                        }
                        break;
                    }
                }
                if (hit == 0) {
                    pFIFO = fifo(j, dados->cabecalho->associatividade, cache);
                    dados->miss++;
                    cache[j].bloco[pFIFO].validade = 1;
                    cache[j].bloco[pFIFO].tag = dados->requisicoes[i].tag;
                    cache[j].bloco[pFIFO].ciclo = dados->requisicoes[i].ciclo;
                    cache[j].bloco[pFIFO].dado = dados->requisicoes[i].endereco;
                    if (dados->requisicoes[i].tipo == 'R') {
                        dados->tempo_total += dados->cabecalho->tempo_acesso_cache + dados->cabecalho->leitura_dram;
                    } else {
                        dados->tempo_total += dados->cabecalho->tempo_acesso_cache + dados->cabecalho->leitura_dram + dados->cabecalho->tempo_acesso_cache;
                    }
                }
            }
        }
    }
    for (int i = 0; i < dados->cabecalho->linhas_cache; i++) {
        for (int j = 0; j < dados->cabecalho->associatividade; j++) {
            if (cache[i].bloco[j].validade == 1) {
                dados->tempo_total += dados->cabecalho->escrita_dram;
            }
        }
    }
    return cache;
}

int fifo(int indice, int associatividade, CACHE * cache) {
    int menor_ciclo = cache[indice].bloco[0].ciclo;
    int posicao = 0;
    for (int i = 1; i < associatividade; i++) {
        if (cache[indice].bloco[i].ciclo < menor_ciclo) {
            menor_ciclo = cache[indice].bloco[i].ciclo;
            posicao = i;
        }
    }
    return posicao;
}