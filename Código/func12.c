#include "features.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

#define INF 1000000000

// --- Estruturas do Grafo ---
typedef struct Edge {
    char *nomeProxEstacao;
    int distProxEstacao;
    int InMST;
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

typedef struct {
    int cod;
    int codProx;
    int codIntegra;
} InfoEntry;

// --- Funções auxiliares para ordenação e busca ---
static int comparar_info(const void *a, const void *b) {
    return ((InfoEntry *)a)->cod - ((InfoEntry *)b)->cod;
}

static int buscar_info_por_codigo(InfoEntry *info, int tam, int codigo, InfoEntry *out) {
    int esq = 0, dir = tam - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        if (info[meio].cod == codigo) { *out = info[meio]; return 1; }
        if (info[meio].cod < codigo) esq = meio + 1;
        else dir = meio - 1;
    }
    return 0;
}

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

// Adiciona aresta bidirecional (grafo não direcionado)
static void adicionar_aresta(Vertex *v, const char *nomeDest, int dist) {
    Edge *curr = v->head;
    Edge *prev = NULL;

    while (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) < 0) {
        prev = curr;
        curr = curr->next;
    }

    if (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) == 0) {
        if (dist < curr->distProxEstacao) {
            curr->distProxEstacao = dist;
        }
        return;
    }

    Edge *new_edge = malloc(sizeof(Edge));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->InMST = 0;
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
            free(curr);
            curr = next;
        }
    }
    free(g->vertices);
}

// DFS para imprimir a MST (apenas arestas com InMST == 1)
static void dfs_imprime_mst(Graph *g, int u, int *visitado_dfs) {
    visitado_dfs[u] = 1;
    Edge *curr = g->vertices[u].head;
    while (curr != NULL) {
        if (curr->InMST) {
            int v = buscar_indice_vertice(g, curr->nomeProxEstacao);
            if (v != -1 && !visitado_dfs[v]) {
                printf("%s, %s, %d\n", g->vertices[u].nomeEstacao,
                       curr->nomeProxEstacao, curr->distProxEstacao);
                dfs_imprime_mst(g, v, visitado_dfs);
            }
        }
        curr = curr->next;
    }
}

// --- Implementação Principal da Funcionalidade 12 (CORRIGIDA) ---
void funcionalidade12(const char *arq_dados, const char *arq_indice, const char *origem) {
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

    int cap_map = 50, tam_map = 0;
    MapEntry *mapa = malloc(cap_map * sizeof(MapEntry));
    InfoEntry *infos = malloc(cap_map * sizeof(InfoEntry));
    int cap_nomes = 50, qtd_nomes = 0;
    char **nomes_unicos = malloc(cap_nomes * sizeof(char *));

    // Primeira passagem: coletar todas as estações e informações
    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        if (ler_registro(fdados, &reg) == 0) break;
        if (reg.status_removido == '1') continue; // ignora removidos

        if (tam_map >= cap_map) {
            cap_map *= 2;
            mapa = realloc(mapa, cap_map * sizeof(MapEntry));
            infos = realloc(infos, cap_map * sizeof(InfoEntry));
        }
        mapa[tam_map].cod = reg.codEstacao;
        mapa[tam_map].nome = strdup(reg.nomeEstacao);
        infos[tam_map].cod = reg.codEstacao;
        infos[tam_map].codProx = reg.codProxEstacao;
        infos[tam_map].codIntegra = reg.codEstIntegra;
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
    qsort(infos, tam_map, sizeof(InfoEntry), comparar_info);
    qsort(nomes_unicos, qtd_nomes, sizeof(char *), comparar_nomes_vertices);

    // Criação do grafo com vértices únicos
    Graph g;
    g.numVertices = qtd_nomes;
    g.vertices = malloc(qtd_nomes * sizeof(Vertex));
    for (int i = 0; i < qtd_nomes; i++) {
        g.vertices[i].nomeEstacao = nomes_unicos[i];
        g.vertices[i].head = NULL;
    }
    free(nomes_unicos);

    // Segunda passagem: construir arestas com VALIDAÇÃO DE DESTINO
    fseek(fdados, 17, SEEK_SET);
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        if (ler_registro(fdados, &reg) == 0) break;
        if (reg.status_removido == '1') continue;

        int idx_origem = buscar_indice_vertice(&g, reg.nomeEstacao);
        if (idx_origem == -1) {
            liberar_strings_registro(&reg);
            continue;
        }

        // --- Aresta via codProxEstacao ---
        if (reg.codProxEstacao != -1) {
            const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
            if (nome_dest != NULL) {
                // VALIDAÇÃO: o destino deve ter continuidade (codProx ou codIntegra existentes)
                InfoEntry info_dest;
                int achou_info = buscar_info_por_codigo(infos, tam_map, reg.codProxEstacao, &info_dest);
                int destino_valido = 0;
                if (achou_info) {
                    // Verifica se o destino tem próxima estação válida (existe no mapa)
                    int prox_existe = (info_dest.codProx != -1) && 
                                      (buscar_nome_por_codigo(mapa, tam_map, info_dest.codProx) != NULL);
                    int integra_existe = (info_dest.codIntegra != -1) && 
                                         (buscar_nome_por_codigo(mapa, tam_map, info_dest.codIntegra) != NULL);
                    destino_valido = prox_existe || integra_existe;
                }
                if (destino_valido) {
                    // Adiciona aresta bidirecional (origem <-> destino)
                    adicionar_aresta(&g.vertices[idx_origem], nome_dest, reg.distProxEstacao);
                    int idx_dest = buscar_indice_vertice(&g, nome_dest);
                    if (idx_dest != -1) {
                        adicionar_aresta(&g.vertices[idx_dest], g.vertices[idx_origem].nomeEstacao, reg.distProxEstacao);
                    }
                }
            }
        }

        // --- Aresta via codEstIntegra ---
        if (reg.codEstIntegra != -1) {
            const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
            if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                // Integração é sempre válida (não há continuidade a verificar, pois é um ponto de conexão)
                adicionar_aresta(&g.vertices[idx_origem], nome_integ, 0);
                int idx_dest = buscar_indice_vertice(&g, nome_integ);
                if (idx_dest != -1) {
                    adicionar_aresta(&g.vertices[idx_dest], g.vertices[idx_origem].nomeEstacao, 0);
                }
            }
        }

        liberar_strings_registro(&reg);
    }

    // Localiza o vértice de origem
    int start_idx = buscar_indice_vertice(&g, origem);
    if (start_idx == -1) {
        printf("Registro inexistente.\n");
        liberar_grafo(&g);
        for (int i = 0; i < tam_map; i++) free(mapa[i].nome);
        free(mapa);
        free(infos);
        fclose(fdados);
        return;
    }

    // --- Algoritmo de Prim para encontrar a MST ---
    int *visited = calloc(g.numVertices, sizeof(int));
    visited[start_idx] = 1;

    for (int count = 0; count < g.numVertices - 1; count++) {
        int best_u = -1, best_v = -1, min_dist = INF;

        for (int u = 0; u < g.numVertices; u++) {
            if (visited[u]) {
                Edge *curr = g.vertices[u].head;
                while (curr != NULL) {
                    int v = buscar_indice_vertice(&g, curr->nomeProxEstacao);
                    if (!visited[v]) {
                        if (curr->distProxEstacao < min_dist) {
                            min_dist = curr->distProxEstacao;
                            best_u = u;
                            best_v = v;
                        } else if (curr->distProxEstacao == min_dist) {
                            // Desempate: menor índice de origem, depois menor índice de destino
                            if (u < best_u) {
                                best_u = u;
                                best_v = v;
                            } else if (u == best_u && v < best_v) {
                                best_v = v;
                            }
                        }
                    }
                    curr = curr->next;
                }
            }
        }

        if (best_u != -1 && best_v != -1) {
            // Marca a aresta u -> v como pertencente à MST
            Edge *e = g.vertices[best_u].head;
            while (e != NULL) {
                if (strcmp(e->nomeProxEstacao, g.vertices[best_v].nomeEstacao) == 0 && e->distProxEstacao == min_dist) {
                    e->InMST = 1;
                    break;
                }
                e = e->next;
            }
            visited[best_v] = 1;
        } else {
            break; // grafo desconexo
        }
    }

    // --- Impressão via DFS na MST a partir da origem ---
    int *visitado_dfs = calloc(g.numVertices, sizeof(int));
    dfs_imprime_mst(&g, start_idx, visitado_dfs);
    free(visitado_dfs);

    // Limpeza
    free(visited);
    for (int i = 0; i < tam_map; i++) free(mapa[i].nome);
    free(mapa);
    free(infos);
    liberar_grafo(&g);
    fclose(fdados);
}