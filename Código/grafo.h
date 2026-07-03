/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#ifndef GRAFO_H
#define GRAFO_H

// Estrutura que representa uma aresta na lista de adjacências
typedef struct Edge {
    char *nomeProxEstacao;
    int distProxEstacao;
    char **nomesLinhas;
    int numLinhas;
    struct Edge *next; // Ponteiro para a próxima aresta da lista
} Edge;

// Estrutura que representa um vértice (estação) no grafo
typedef struct Vertex {
    char *nomeEstacao;
    Edge *head; // Ponteiro para o início da lista encadeada de arestas (adjacências)
} Vertex;

// Estrutura principal do Grafo
typedef struct Graph {
    Vertex *vertices; // Vetor dinâmico contendo todos os vértices do grafo
    int numVertices;
} Graph;

// Estrutura auxiliar para mapear códigos numéricos para nomes de estações
typedef struct {
    int cod;
    char *nome;
} MapEntry;

// Funções de comparação para ordenação via qsort
int comparar_nomes_vertices(const void *a, const void *b);
int comparar_mapa(const void *a, const void *b);

// Funções de busca (Busca Binária)
const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo);
int buscar_indice_vertice(Graph *g, const char *nome);

// Inserção e gerenciamento de memória
void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha);
void liberar_grafo(Graph *g);

// Construção do Grafo
int construir_grafo(const char *nome_arquivo, Graph *g);

#endif