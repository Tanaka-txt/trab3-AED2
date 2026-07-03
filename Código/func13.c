/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "features.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "grafo.h"
#include "grafo_builder.h"

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

    // --- 1. Construção do Grafo (Direcionado, compartilhada via grafo_builder) ---
    MapEntry *mapa;
    int tam_map;
    char **nomes_unicos;
    int qtd_nomes;
    construir_mapa_e_vertices(fdados, &mapa, &tam_map, &nomes_unicos, &qtd_nomes);

    Graph g;
    inicializar_vertices(&g, nomes_unicos, qtd_nomes);

    construir_arestas_direcionado(fdados, &g, mapa, tam_map);

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
