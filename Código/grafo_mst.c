/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "grafo_mst.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void inicializar_vertices_mst(GraphMST *g, char **nomes_unicos, int qtd_nomes) {
    g->numVertices = qtd_nomes;
    g->vertices = malloc(qtd_nomes * sizeof(VertexMST));
    for (int i = 0; i < qtd_nomes; i++) {
        g->vertices[i].nomeEstacao = nomes_unicos[i];
        g->vertices[i].head = NULL;
    }
    free(nomes_unicos);
}

int buscar_indice_vertice_mst(GraphMST *g, const char *nome) {
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

void adicionar_aresta_mst(VertexMST *v, const char *nomeDest, int dist) {
    EdgeMST *curr = v->head;
    EdgeMST *prev = NULL;

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

    EdgeMST *new_edge = malloc(sizeof(EdgeMST));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->InMST = 0;
    new_edge->next = curr;

    if (prev == NULL) v->head = new_edge;
    else prev->next = new_edge;
}

void liberar_grafo_mst(GraphMST *g) {
    for (int i = 0; i < g->numVertices; i++) {
        free(g->vertices[i].nomeEstacao);
        EdgeMST *curr = g->vertices[i].head;
        while (curr != NULL) {
            EdgeMST *next = curr->next;
            free(curr->nomeProxEstacao);
            free(curr);
            curr = next;
        }
    }
    free(g->vertices);
}

void dfs_imprime_mst(GraphMST *g, int u, int *visitado_dfs) {
    visitado_dfs[u] = 1;
    EdgeMST *curr = g->vertices[u].head;
    while (curr != NULL) {
        if (curr->InMST) {
            int v = buscar_indice_vertice_mst(g, curr->nomeProxEstacao);
            if (v != -1 && !visitado_dfs[v]) {
                printf("%s, %s, %d\n", g->vertices[u].nomeEstacao,
                       curr->nomeProxEstacao, curr->distProxEstacao);
                dfs_imprime_mst(g, v, visitado_dfs);
            }
        }
        curr = curr->next;
    }
}
