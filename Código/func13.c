#include "features.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

/*
// --- Estruturas de Dados do Grafo (Grafo Direcionado - Igual à Func 10) ---

typedef struct Edge {
    char *nomeProxEstacao;
    int distProxEstacao;
    char **nomesLinhas; 
    int numLinhas;
    struct Edge *next;
} Edge;

typedef struct Vertex {
    char *nomeEstacao;
    Edge *head;
} Vertex;

typedef struct Graph {
    Vertex *vertices;
    int numVertices;
} Graph;

typedef struct {
    int cod;
    char *nome;
} MapEntry;

// --- Funções Auxiliares do Grafo ---

static int comparar_nomes_vertices(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

static int comparar_mapa(const void *a, const void *b) {
    return ((MapEntry *)a)->cod - ((MapEntry *)b)->cod;
}

static const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo) {
    int esq = 0, dir = tam - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        if (mapa[meio].cod == codigo) return mapa[meio].nome;
        if (mapa[meio].cod < codigo) esq = meio + 1;
        else dir = meio - 1;
    }
    return NULL;
}

static int buscar_indice_vertice(Graph *g, const char *nome) {
    int esq = 0, dir = g->numVertices - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        int cmp = strcmp(g->vertices[meio].nomeEstacao, nome);
        if (cmp == 0) return meio;
        if (cmp < 0) esq = meio + 1;
        else dir = meio - 1;
    }
    return -1;
}

static void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha) {
    Edge *curr = v->head;
    Edge *prev = NULL;

    while (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) < 0) {
        prev = curr;
        curr = curr->next;
    }

    if (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) == 0) {
        for (int i = 0; i < curr->numLinhas; i++) {
            if (strcmp(curr->nomesLinhas[i], linha) == 0) return; 
        }
        curr->numLinhas++;
        curr->nomesLinhas = realloc(curr->nomesLinhas, curr->numLinhas * sizeof(char *));
        curr->nomesLinhas[curr->numLinhas - 1] = strdup(linha);
        return;
    }

    Edge *new_edge = malloc(sizeof(Edge));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->numLinhas = 1;
    new_edge->nomesLinhas = malloc(sizeof(char *));
    new_edge->nomesLinhas[0] = strdup(linha);
    new_edge->next = curr;

    if (prev == NULL) v->head = new_edge;
    else prev->next = new_edge;
}

static void liberar_grafo(Graph *g) {
    for (int i = 0; i < g->numVertices; i++) {
        free(g->vertices[i].nomeEstacao);
        Edge *curr = g->vertices[i].head;
        while (curr != NULL) {
            Edge *next = curr->next;
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
*/
// --- DFS com Backtracking para encontrar Ciclos Simples ---

static void dfs_ciclos(Graph *g, int u, int origin, int *visited, int *count) {
    // Marca o vértice atual como visitado para a busca neste caminho específico
    visited[u] = 1;

    Edge *curr = g->vertices[u].head;
    while (curr != NULL) {
        int v = buscar_indice_vertice(g, curr->nomeProxEstacao);
        if (v != -1) {
            // Se o destino é a nossa própria origem, encontramos um ciclo fechado!
            if (v == origin) {
                (*count)++;
            } 
            // Caso contrário, se ainda não visitamos essa estação NESTE caminho, continuamos avançando
            else if (!visited[v]) {
                dfs_ciclos(g, v, origin, visited, count);
            }
        }
        curr = curr->next;
    }

    // Backtracking: Desmarca a visita para permitir que outros caminhos explorem essa estação
    visited[u] = 0;
}

// --- Implementação Principal da Funcionalidade 13 ---

void funcionalidade13(const char *arq_dados, const char *arq_indice, const char *origem) {
    FILE *fdados = fopen(arq_dados, "rb");
    if (fdados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    reg_cabecalho cab;
    if (!validar_cabecalho(fdados, &cab)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fdados);
        return;
    }

    // --- 1. Construção do Grafo (Direcionado) ---
    int cap_map = 50, tam_map = 0;
    MapEntry *mapa = malloc(cap_map * sizeof(MapEntry));
    int cap_nomes = 50, qtd_nomes = 0;
    char **nomes_unicos = malloc(cap_nomes * sizeof(char *));

    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        if (tam_map >= cap_map) {
            cap_map *= 2;
            mapa = realloc(mapa, cap_map * sizeof(MapEntry));
        }
        mapa[tam_map].cod = reg.codEstacao;
        mapa[tam_map].nome = strdup(reg.nomeEstacao);
        tam_map++;

        int existe = 0;
        for (int i = 0; i < qtd_nomes; i++) {
            if (strcmp(nomes_unicos[i], reg.nomeEstacao) == 0) {
                existe = 1; break;
            }
        }
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

    qsort(mapa, tam_map, sizeof(MapEntry), comparar_mapa);
    qsort(nomes_unicos, qtd_nomes, sizeof(char *), comparar_nomes_vertices);

    Graph g;
    g.numVertices = qtd_nomes;
    g.vertices = malloc(qtd_nomes * sizeof(Vertex));
    for (int i = 0; i < qtd_nomes; i++) {
        g.vertices[i].nomeEstacao = nomes_unicos[i]; 
        g.vertices[i].head = NULL;
    }
    free(nomes_unicos); 

    fseek(fdados, 17, SEEK_SET);
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        int idx_origem = buscar_indice_vertice(&g, reg.nomeEstacao);
        if (idx_origem != -1) {
            if (reg.codProxEstacao != -1) {
                const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
                if (nome_dest != NULL) {
                    adicionar_aresta(&g.vertices[idx_origem], nome_dest, reg.distProxEstacao, reg.nomeLinha);
                }
            }
            if (reg.codEstIntegra != -1) {
                const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
                if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                    adicionar_aresta(&g.vertices[idx_origem], nome_integ, 0, "Integração");
                }
            }
        }
        liberar_strings_registro(&reg);
    }

    // --- 2. Busca e Contagem dos Ciclos ---
    int start_idx = buscar_indice_vertice(&g, origem);

    if (start_idx == -1) {
        printf("Registro inexistente.\n");
    } else {
        int count = 0;
        int *visited = calloc(g.numVertices, sizeof(int));

        dfs_ciclos(&g, start_idx, start_idx, visited, &count);

        if (count == 0) {
            printf("Quantidade de ciclos: -1\n");
        } else {
            printf("Quantidade de ciclos: %d\n", count);
        }

        free(visited);
    }

    // --- 3. Limpeza Final de Memória ---
    for (int i = 0; i < tam_map; i++) free(mapa[i].nome);
    free(mapa);
    liberar_grafo(&g);
    fclose(fdados);
}