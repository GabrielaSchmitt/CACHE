from sys import argv
import numpy as np
import itertools

def decimal_para_binario(decimal):
    binario = ''
    while decimal > 0:
        binario+= str(decimal%2)
        decimal//= 2
    return binario[::-1]

class FIFO:
    def __init__(self):
        arquivo = argv[1]
        with open(arquivo, 'r') as file:
            values = file.read().splitlines()

        first_line = values[0].split(' ')
        # Associatividade
        self.associatividade = 16
        # Tamanho da cache em bytes
        self.s = int(first_line[0])
        # Tamanho do bloco em bytes
        self.b = int(first_line[1])
        # Tempo de acesso a cache
        self.tc = int(first_line[2])
        # Tempo de leitura DRAM
        self.trm = int(first_line[3])
        # Tempo de escrita DRAM
        self.twm = int(first_line[4])
        # Número de requisições
        self.n = int(values[1])
        # Quantidade de linhas da cache
        self.linhas_cache = int(self.s / self.b / self.associatividade)
        # Tamanho do offset
        self.tamanho_offset = int(np.log2(self.b))
        # Tamanho do índice da cache
        self.tamanho_indice = int(np.log2(self.linhas_cache))

        self.requisicoes = {
            'ciclo': [],
            'tipo': [],
            'indice': [],
            'tag': [],
            'endereco': []
        }
        for i in range(2, self.n + 2):
            requisicao = values[i].split(' ')
            self.requisicoes['ciclo'].append(int(requisicao[0]))
            self.requisicoes['tipo'].append(requisicao[1])
            self.requisicoes['endereco'].append(requisicao[2])
            bin = decimal_para_binario(int(requisicao[2]))
            if self.tamanho_offset > 0:
                bin = bin[:-self.tamanho_offset]
            if self.tamanho_indice > 0:
                self.requisicoes['indice'].append(bin[-self.tamanho_indice:])
                self.requisicoes['tag'].append(bin[:-self.tamanho_indice])
            else:
                self.requisicoes['indice'].append('0')
                self.requisicoes['tag'].append(bin)

        self.cache = {
            'indices': [],
            'linhas': []
        }
        if self.tamanho_indice > 0:
            for i in list(map(list, itertools.product([0, 1], repeat=self.tamanho_indice))):
                idx = ''
                for j in i:
                    idx += str(j)
                self.cache['indices'].append(idx)
        else:
            self.cache['indices'].append('0')

        for i in range(self.linhas_cache):
            linhas = {
                'ciclo': [],
                'tag': [],
                'validade': [],
                'dado': []
            }
            for j in range(self.associatividade):
                linhas['ciclo'].append(0)
                linhas['tag'].append(None)
                linhas['validade'].append(False)
                linhas['dado'].append(None)
            self.cache['linhas'].append(linhas)

        self.misses = 0
        self.hits = 0
        self.tempo = 0

        count = 0
        for i in self.requisicoes['indice']:
            idx = self.cache['indices'].index(i)
            if self.requisicoes['tipo'][count] == 'R':
                count_associatividade = 0
                isHit = False
                for j in self.cache['linhas'][idx]['validade']:
                    if j == True and self.cache['linhas'][idx]['tag'][count_associatividade] == self.requisicoes['tag'][count]:
                        self.hits += 1
                        self.tempo += self.tc
                        isHit = True
                        break
                    count_associatividade += 1
                if isHit == False:
                    self.misses += 1
                    self.tempo += self.tc + self.trm
                    idx_fifo = self.fifo(idx)
                    self.cache['linhas'][idx]['ciclo'][idx_fifo] = self.requisicoes['ciclo'][count]
                    self.cache['linhas'][idx]['tag'][idx_fifo] = self.requisicoes['tag'][count]
                    self.cache['linhas'][idx]['validade'][idx_fifo] = True
                    self.cache['linhas'][idx]['dado'][idx_fifo] = self.requisicoes['endereco'][count]
            else:
                count_associatividade = 0
                isHit = False
                for j in self.cache['linhas'][idx]['validade']:
                    if j == True and self.cache['linhas'][idx]['tag'][count_associatividade] == self.requisicoes['tag'][count]:
                        self.hits += 1
                        self.tempo += self.tc
                        isHit = True
                        break
                    count_associatividade += 1
                if isHit == False:
                    self.misses += 1
                    self.tempo += self.tc + self.trm + self.tc
                    idx_fifo = self.fifo(idx)
                    self.cache['linhas'][idx]['ciclo'][idx_fifo] = self.requisicoes['ciclo'][count]
                    self.cache['linhas'][idx]['tag'][idx_fifo] = self.requisicoes['tag'][count]
                    self.cache['linhas'][idx]['validade'][idx_fifo] = True
                    self.cache['linhas'][idx]['dado'][idx_fifo] = self.requisicoes['endereco'][count]
            count += 1

        for i in self.cache['linhas']:
            for j in i['validade']:
                if j == True:
                    self.tempo += self.twm

        print(self.s, self.associatividade, self.b, "FIFO", "WB", "WA", self.tc, self.trm, self.twm)
        print(self.tempo)
        print(self.hits, self.misses)
        print(self.linhas_cache)
        for i in self.cache['linhas']:
            for j in range(self.associatividade):
                if j < self.associatividade - 1:
                    print(i['dado'][j], end=" ")
                else:
                    print(i['dado'][j])

    def fifo(self, linha):
        menor = self.cache['linhas'][linha]['ciclo'][0]
        idx = 0
        for i in range(self.associatividade):
            if self.cache['linhas'][linha]['ciclo'][i] < menor:
                menor = self.cache['linhas'][linha]['ciclo'][i]
                idx = i
        return idx

test = FIFO()