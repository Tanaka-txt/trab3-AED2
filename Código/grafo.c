#include "grafo.h"
#include <stdlib.h>
#include <string.h>

int comparar_nomes_vertices(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int comparar_mapa(const void *a, const void *b) {
    return ((MapEntry *)a)->cod - ((MapEntry *)b)->cod;
}

const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo) {
    int esq = 0, dir = tam - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        if (mapa[meio].cod == codigo) return mapa[meio].nome;
        if (mapa[meio].cod < codigo) esq = meio + 1;
        else dir = meio - 1;
    }
    return NULL;
}

int buscar_indice_vertice(Graph *g, const char *nome) {
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

void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha) {
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

void liberar_grafo(Graph *g) {
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