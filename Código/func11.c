#include "features.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

#define INF 1000000000 // Valor infinito para o algoritmo de Dijkstra
/*
// --- Estruturas de Dados do Grafo (Idênticas à Funcionalidade 10) ---

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
// --- Implementação Principal da Funcionalidade 11 ---

void funcionalidade11(const char *arq_dados, const char *arq_indice, const char *origem, const char *destino) {
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

    // --- 1. Construção do Grafo (Reaproveitado da Funcionalidade 10) ---
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
                if (nome_dest != NULL) adicionar_aresta(&g.vertices[idx_origem], nome_dest, reg.distProxEstacao, reg.nomeLinha);
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

    // --- 2. Algoritmo de Dijkstra ---
    int start_idx = buscar_indice_vertice(&g, origem);
    int end_idx = buscar_indice_vertice(&g, destino);

    if (start_idx == -1 || end_idx == -1) {
        printf("Registro inexistente.\n");
    } else {
        int *dist = malloc(g.numVertices * sizeof(int));
        int *prev = malloc(g.numVertices * sizeof(int));
        int *visited = malloc(g.numVertices * sizeof(int));

        for (int i = 0; i < g.numVertices; i++) {
            dist[i] = INF;
            prev[i] = -1;
            visited[i] = 0;
        }

        dist[start_idx] = 0;

        for (int count = 0; count < g.numVertices; count++) {
            int u = -1;
            int min_dist = INF;

            // Encontra o vértice não visitado com a menor distância.
            // O loop iterando de 0 a g.numVertices garante o desempate pelo menor nome alfabético!
            for (int i = 0; i < g.numVertices; i++) {
                if (!visited[i] && dist[i] < min_dist) {
                    min_dist = dist[i];
                    u = i;
                }
            }

            if (u == -1 || u == end_idx) break; // Chegou ao fim ou não há mais caminhos alcançáveis
            visited[u] = 1;

            Edge *curr = g.vertices[u].head;
            while (curr != NULL) {
                int v = buscar_indice_vertice(&g, curr->nomeProxEstacao);
                int weight = curr->distProxEstacao;

                if (!visited[v] && dist[u] != INF) {
                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        prev[v] = u;
                    } 
                    // Desempate de aresta: se o peso der empate, pegamos o vértice de origem com menor nome (índice)
                    else if (dist[u] + weight == dist[v]) {
                        if (u < prev[v]) {
                            prev[v] = u;
                        }
                    }
                }
                curr = curr->next;
            }
        }

        // --- 3. Recuperação e Impressão do Caminho ---
        if (dist[end_idx] == INF) {
            printf("Registro inexistente.\n");
        } else {
            int *path = malloc(g.numVertices * sizeof(int));
            int path_len = 0;
            int curr = end_idx;

            while (curr != -1) {
                path[path_len++] = curr;
                curr = prev[curr];
            }

            // Subtrai 1 pois o caminho não contabiliza a origem como 'estação percorrida'
            printf("Numero de estacoes que serao percorridas: %d\n", path_len - 1);
            printf("Distancia que sera percorrida: %d\n", dist[end_idx]);

            // O array 'path' armazena o trajeto de trás para frente. Vamos imprimir da origem ao destino.
            for (int i = path_len - 1; i >= 0; i--) {
                printf("%s", g.vertices[path[i]].nomeEstacao);
                if (i > 0) printf(", ");
            }
            printf("\n");
            
            free(path);
        }

        free(dist);
        free(prev);
        free(visited);
    }

    // Limpeza da memória
    for (int i = 0; i < tam_map; i++) free(mapa[i].nome);
    free(mapa);
    liberar_grafo(&g);
    fclose(fdados);
}