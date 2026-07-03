/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

// Implementação das operações sobre o grafo definido em grafo.h

#include "grafo.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Compara duas strings (nomes de vértices) em ordem alfabética para o qsort
int comparar_nomes_vertices(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}
// Compara dois MapEntries numéricamente pelo código para o qsort
int comparar_mapa(const void *a, const void *b) {
    return ((MapEntry *)a)->cod - ((MapEntry *)b)->cod;
}

// Busca binária sobre o vetor "mapa" para localizar o nome da estação 
const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo) {
    int esq = 0, dir = tam - 1; // Limites inferior e superior da busca

    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2; // Índice central do intervalo atual

        if (mapa[meio].cod == codigo) return mapa[meio].nome; 
        if (mapa[meio].cod < codigo) esq = meio + 1;          
        else dir = meio - 1;                                  
    }
    return NULL; 
}

// Busca binária sobre o vetor de vértices do grafo para localizar o índice do vértice 
int buscar_indice_vertice(Graph *g, const char *nome) {
    int esq = 0, dir = g->numVertices - 1; // Limites inferior e superior da busca

    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2; // Índice central do intervalo atual
        int cmp = strcmp(g->vertices[meio].nomeEstacao, nome);

        if (cmp == 0) return meio;   
        if (cmp < 0) esq = meio + 1; 
        else dir = meio - 1;         
    }
    return -1; }

// Insere (ou atualiza) uma aresta na lista de adjacencias de um vértice
void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha) {
    Edge *curr = v->head; // Aresta atual durante a percorrida da lista
    Edge *prev = NULL;    // Aresta anterior a "curr" 

    // Percorre a lista até achar a posição alfabética correta ou a aresta que já aponta para o destino
    while (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) < 0) {
        prev = curr;
        curr = curr->next;
    }

    // Aresta para o destino já existe -> apenas acrescenta a linha
    if (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) == 0) {
        // Verifica se a linha já está cadastrada 
        for (int i = 0; i < curr->numLinhas; i++) {
            if (strcmp(curr->nomesLinhas[i], linha) == 0) return;
        }

        curr->numLinhas++; // Aumenta o vetor de linhas em uma posição e insere a nova linha
        curr->nomesLinhas = realloc(curr->nomesLinhas, curr->numLinhas * sizeof(char *));
        curr->nomesLinhas[curr->numLinhas - 1] = strdup(linha);

        // Reordena o vetor de linhas em ordem alfabética
        for (int i = 0; i < curr->numLinhas - 1; i++) {
            for (int j = i + 1; j < curr->numLinhas; j++) {
                if (strcmp(curr->nomesLinhas[i], curr->nomesLinhas[j]) > 0) {
                    char *tmp = curr->nomesLinhas[i];
                    curr->nomesLinhas[i] = curr->nomesLinhas[j];
                    curr->nomesLinhas[j] = tmp;
                }
            }
        }
        return;
    }

    // Aresta ainda não existe --> cria uma nova e insere na lista
    Edge *new_edge = malloc(sizeof(Edge));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->numLinhas = 1;
    new_edge->nomesLinhas = malloc(sizeof(char *));
    new_edge->nomesLinhas[0] = strdup(linha);
    new_edge->next = curr; // Encadeia a nova aresta com o restante da lista

    if (prev == NULL) v->head = new_edge;
    else prev->next = new_edge;
}


void liberar_grafo(Graph *g) {
    for (int i = 0; i < g->numVertices; i++) {
        free(g->vertices[i].nomeEstacao); 

        // Percorre e libera toda a lista encadeada de arestas do vértice
        Edge *curr = g->vertices[i].head;
        while (curr != NULL) {
            Edge *next = curr->next; // Guarda referência antes de liberar "curr"
            free(curr->nomeProxEstacao); 
            for (int j = 0; j < curr->numLinhas; j++) {
                free(curr->nomesLinhas[j]);
            }
            free(curr->nomesLinhas);
            free(curr); 
            curr = next;
        }
    }
    free(g->vertices);
}

int construir_grafo(const char *nome_arquivo, Graph *g) {
    // Abertura e validacao do arquivo 
    FILE *fdados = fopen(nome_arquivo, "rb");
    if (fdados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    reg_cabecalho cab; 
    if (!validar_cabecalho(fdados, &cab)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fdados);
        return 0;
    }

    // mapa: traduz código numérico de estação -> nome da estação
    // nomes_unicos: nomes distintos de estações, usados para criar os vértices
    int cap_map = 50, tam_map = 0;
    MapEntry *mapa = malloc(cap_map * sizeof(MapEntry));
    int cap_nomes = 50, qtd_nomes = 0;
    char **nomes_unicos = malloc(cap_nomes * sizeof(char *));

    // Primeira passagem pelo arquivo: preenche o mapa de códigos e coleta o conjunto de nomes únicos de estações, necessários para montar os vértices antes de qualquer aresta
    fseek(fdados, 17, SEEK_SET); 
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;    // Fim do arquivo
        if (ret == 2) continue; // Registro logicamente removido

        // Adiciona a estação atual ao mapa de códigos
        if (tam_map >= cap_map) {
            cap_map *= 2;
            mapa = realloc(mapa, cap_map * sizeof(MapEntry));
        }
        mapa[tam_map].cod = reg.codEstacao;
        mapa[tam_map].nome = strdup(reg.nomeEstacao);
        tam_map++;

        // Verifica se o nome da estação atual já foi coletado
        int existe = 0;
        for (int i = 0; i < qtd_nomes; i++) {
            if (strcmp(nomes_unicos[i], reg.nomeEstacao) == 0) {
                existe = 1;
                break;
            }
        }

        // Se for um nome novo, insere no vetor de nomes únicos
        if (!existe) {
            if (qtd_nomes >= cap_nomes) {
                cap_nomes *= 2;
                nomes_unicos = realloc(nomes_unicos, cap_nomes * sizeof(char *));
            }
            nomes_unicos[qtd_nomes] = strdup(reg.nomeEstacao);
            qtd_nomes++;
        }
        liberar_strings_registro(&reg);
    }

    // Ordenacao das estruturas auxiliares: permitir busca binária no mapa (por código) e no vetor de vértices do grafo (por nome)
    qsort(mapa, tam_map, sizeof(MapEntry), comparar_mapa);
    qsort(nomes_unicos, qtd_nomes, sizeof(char *), comparar_nomes_vertices);

    // Criação dos vértices do grafo: cada nome único de estação vira um vértice, ainda sem aresta
    g->numVertices = qtd_nomes;
    g->vertices = malloc(qtd_nomes * sizeof(Vertex));
    for (int i = 0; i < qtd_nomes; i++) {
        g->vertices[i].nomeEstacao = nomes_unicos[i];
        g->vertices[i].head = NULL;
    }
    free(nomes_unicos); 

    // Segunda passagem pelo arquivo: todos os vértices existem e criam as arestas (conexões) entre as estações
    fseek(fdados, 17, SEEK_SET); 
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        int idx_origem = buscar_indice_vertice(g, reg.nomeEstacao);
        if (idx_origem != -1) {
            // Aresta para a próxima estação da mesma linha
            if (reg.codProxEstacao != -1) {
                const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
                if (nome_dest != NULL) {
                    adicionar_aresta(&g->vertices[idx_origem], nome_dest, reg.distProxEstacao, reg.nomeLinha);
                }
            }
            // Aresta de integração com outra linha
            if (reg.codEstIntegra != -1) {
                const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
                // Insere somente se a estação de integração tiver nome diferente da estação de origem 
                if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                    adicionar_aresta(&g->vertices[idx_origem], nome_integ, 0, "Integração");
                }
            }
        }
        liberar_strings_registro(&reg);
    }

    for (int i = 0; i < tam_map; i++) {
        free(mapa[i].nome);
    }
    free(mapa);
    fclose(fdados);

    return 1; // Grafo construído com sucesso
}