typedef struct cabecalho {
    unsigned int associatividade;
    unsigned int tam_cache;
    unsigned int tam_bloco;
    unsigned int tempo_acesso_cache;
    unsigned int leitura_dram;
    unsigned int escrita_dram;
    unsigned int num_requisicoes;
    unsigned int linhas_cache;
    unsigned int tam_offset;
    unsigned int tam_indice;
} CABECALHO;

typedef struct requisicao {
    unsigned int ciclo;
    unsigned int indice;
    unsigned int tag;
    unsigned int endereco;
    char tipo;
} REQUISICAO;

typedef struct dados {
    CABECALHO * cabecalho;
    REQUISICAO * requisicoes;
    unsigned int miss;
    unsigned int hit;
    unsigned int tempo_total;
} DADOS;

typedef struct bloco {
    unsigned int ciclo;
    unsigned int tag;
    unsigned int validade;
    unsigned int dado;
} BLOCO;

typedef struct cache {
    unsigned int  indice;
    BLOCO * bloco;
} CACHE;

int hit, pFIFO;
CACHE * executa_requisicoes(CACHE * cache, DADOS * dados);
int fifo(int indice, int associatividade, CACHE * cache);