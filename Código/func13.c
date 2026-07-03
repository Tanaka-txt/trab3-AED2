/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

/*
 * func13.c
 * -----------------------------------------------------------------------
 * Funcionalidade 13: constroi o grafo de estacoes (via construir_grafo,
 * em grafo.c) e conta, a partir de uma estacao de origem informada,
 * quantos ciclos simples existem partindo dela e retornando a ela
 * (um ciclo simples nao repete nenhum vertice, exceto o primeiro e o
 * ultimo, que sao a propria estacao de origem).
 * -----------------------------------------------------------------------
 */

#include "features.h"
#include <stdlib.h>
#include "grafo.h"

/*
 * dfs_ciclos
 * -------------
 * Realiza uma busca em profundidade (DFS) com backtracking a partir do
 * vertice "u", procurando caminhos que retornem ao vertice de origem
 * "origin" sem repetir nenhum vertice pelo caminho (ciclo simples).
 * Cada vez que um caminho fecha de volta em "origin", o contador
 * "count" e incrementado.
 *
 * Parametros:
 *   g       - ponteiro para o grafo onde a busca sera realizada.
 *   u       - vertice atual sendo explorado nesta chamada recursiva.
 *   origin  - vertice de origem/destino do ciclo (fixo durante toda a busca).
 *   visited - vetor de marcacao dos vertices ja visitados no caminho
 *             atual (usado para nao repetir vertices dentro do mesmo ciclo).
 *   count   - ponteiro para o contador de ciclos simples encontrados,
 *             incrementado a cada ciclo fechado.
 *
 * Retorno:
 *   Nao ha retorno (void). O resultado e acumulado em "*count".
 */
static void dfs_ciclos(Graph *g, int u, int origin, int *visited, int *count) {
    // Marca o vertice atual como visitado para a busca neste caminho especifico.
    visited[u] = 1;

    // Percorre todas as arestas (conexoes) que saem do vertice "u".
    Edge *curr = g->vertices[u].head;
    while (curr != NULL) {
        int v = buscar_indice_vertice(g, curr->nomeProxEstacao);
        if (v != -1) {
            // Se o destino e a propria origem, um ciclo simples foi fechado.
            if (v == origin) {
                (*count)++;
            }
            // Caso contrario, se o vertice ainda nao foi visitado neste
            // caminho, continua a busca recursivamente a partir dele.
            else if (!visited[v]) {
                dfs_ciclos(g, v, origin, visited, count);
            }
        }
        curr = curr->next;
    }

    // Backtracking: desmarca o vertice para permitir que outros caminhos
    // (originados de outras arestas visitadas anteriormente) explorem
    // esta mesma estacao novamente.
    visited[u] = 0;
}

/*
 * funcionalidade13
 * -------------------
 * Constroi o grafo de estacoes a partir do arquivo de dados (delegando
 * a construcao para construir_grafo) e conta quantos ciclos simples
 * existem a partir da estacao de origem informada.
 *
 * Parametros:
 *   arq_dados  - caminho do arquivo binario de dados das estacoes.
 *   arq_indice - caminho do arquivo de indice (nao utilizado diretamente
 *                nesta funcionalidade, mantido por compatibilidade de
 *                assinatura com o restante do programa).
 *   origem     - nome da estacao de origem a partir da qual os ciclos
 *                simples serao contados.
 *
 * Retorno:
 *   Nao ha retorno (void). Imprime a quantidade de ciclos simples
 *   encontrados, ou "Registro inexistente." caso a estacao de origem
 *   nao exista no grafo. Caso nao seja possivel voltar a origem, a
 *   quantidade de ciclos impressa e -1.
 */
void funcionalidade13(const char *arq_dados, const char *arq_indice, const char *origem) {
    // Constroi o grafo completo (abertura do arquivo, validacao,
    // montagem do mapa, criacao de vertices e arestas). A mensagem de
    // erro, caso ocorra, ja e impressa dentro de construir_grafo.
    Graph g;
    if (!construir_grafo(arq_dados, &g)) {
        return;
    }

    // --- Busca e contagem dos ciclos simples ---
    int start_idx = buscar_indice_vertice(&g, origem);

    if (start_idx == -1) {
        // Estacao de origem informada nao existe no grafo.
        printf("Registro inexistente.\n");
    } else {
        int count = 0; // quantidade de ciclos simples encontrados
        // visited: marca, para o caminho atual da DFS, quais vertices ja
        // foram visitados (evita repetir estacoes dentro do mesmo ciclo).
        int *visited = calloc(g.numVertices, sizeof(int));

        dfs_ciclos(&g, start_idx, start_idx, visited, &count);

        // Caso nenhum ciclo tenha sido encontrado, a especificacao pede
        // que seja impresso -1 em vez de 0.
        if (count == 0) {
            printf("Quantidade de ciclos: -1\n");
        } else {
            printf("Quantidade de ciclos: %d\n", count);
        }

        free(visited);
    }

    // Libera toda a memoria alocada dinamicamente para o grafo.
    liberar_grafo(&g);
}