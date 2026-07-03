/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#ifndef GRAFO_MST_H
#define GRAFO_MST_H

// Grafo não direcionado usado exclusivamente pela funcionalidade 12
// (construção da Árvore Geradora Mínima). Diferente do grafo de grafo.h,
// as arestas aqui não guardam linhas, apenas a distância e uma flag
// indicando se a aresta pertence à MST.

typedef struct EdgeMST {
    char *nomeProxEstacao;
    int distProxEstacao;
    int InMST;
    struct EdgeMST *next;
} EdgeMST;

typedef struct VertexMST {
    char *nomeEstacao;
    EdgeMST *head;
} VertexMST;

typedef struct GraphMST {
    VertexMST *vertices;
    int numVertices;
} GraphMST;

void inicializar_vertices_mst(GraphMST *g, char **nomes_unicos, int qtd_nomes);
int buscar_indice_vertice_mst(GraphMST *g, const char *nome);

// Adiciona aresta bidirecional (grafo não direcionado)
void adicionar_aresta_mst(VertexMST *v, const char *nomeDest, int dist);

void liberar_grafo_mst(GraphMST *g);

// DFS para imprimir a MST (apenas arestas com InMST == 1)
void dfs_imprime_mst(GraphMST *g, int u, int *visitado_dfs);

#endif
