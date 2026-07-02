#ifndef GRAFO_H
#define GRAFO_H

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

int comparar_nomes_vertices(const void *a, const void *b);
int comparar_mapa(const void *a, const void *b);
const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo);
int buscar_indice_vertice(Graph *g, const char *nome);
void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha);
void liberar_grafo(Graph *g);

#endif