/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#ifndef GRAFO_BUILDER_H
#define GRAFO_BUILDER_H

#include <stdio.h>
#include "grafo.h"
#include "registro.h"

// Struct auxiliar usada apenas pela func12 para validar continuidade de estações
typedef struct {
    int cod;
    int codProx;
    int codIntegra;
} InfoEntry;

// Primeira passagem: percorre o arquivo de dados e monta o mapa cod->nome
// (ordenado por código) e a lista de nomes únicos de estações (ordenada
// alfabeticamente). Usado por func10, func11, func12 e func13.
void construir_mapa_e_vertices(FILE *fdados, MapEntry **mapa, int *tam_map,
                                char ***nomes_unicos, int *qtd_nomes);

// Cria o array de vértices do grafo a partir dos nomes únicos já ordenados.
// Consome (assume posse de) o array nomes_unicos, liberando-o ao final.
void inicializar_vertices(Graph *g, char **nomes_unicos, int qtd_nomes);

// Segunda passagem: percorre novamente o arquivo de dados construindo as
// arestas de um grafo direcionado com linhas (codProxEstacao e
// codEstIntegra). Usado por func10, func11 e func13.
void construir_arestas_direcionado(FILE *fdados, Graph *g, MapEntry *mapa, int tam_map);

// Passagem auxiliar (usada apenas pela func12): coleta, para cada estação
// não removida, seu código, o código da próxima estação e o código de
// integração. Retorna o array já ordenado por código.
void construir_info_entries(FILE *fdados, InfoEntry **infos, int *tam_info);

int comparar_info(const void *a, const void *b);
int buscar_info_por_codigo(InfoEntry *info, int tam, int codigo, InfoEntry *out);

int construir_grafo(const char *nome_arquivo, Graph *g);

#endif
