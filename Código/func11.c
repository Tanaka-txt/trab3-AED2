/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

/*
 * func11.c
 * -----------------------------------------------------------------------
 * Funcionalidade 11: constroi o grafo de estacoes (via construir_grafo,
 * em grafo.c) e aplica o algoritmo de Dijkstra para encontrar o caminho
 * de menor distancia entre uma estacao de origem e uma estacao de
 * destino. Ao final, imprime o numero de estacoes percorridas, a
 * distancia total e a sequencia de estacoes do caminho encontrado.
 * -----------------------------------------------------------------------
 */

#include "features.h"
#include <stdlib.h>
#include "grafo.h"

#define INF 1000000000 // Valor usado para representar "distancia infinita" (nao alcancado) no Dijkstra

/*
 * funcionalidade11
 * -------------------
 * Constroi o grafo de estacoes a partir do arquivo de dados (delegando
 * a construcao para construir_grafo) e calcula, via algoritmo de
 * Dijkstra, o caminho de menor distancia entre a estacao de origem e a
 * estacao de destino informadas.
 *
 * Parametros:
 *   arq_dados  - caminho do arquivo binario de dados das estacoes.
 *   arq_indice - caminho do arquivo de indice (nao utilizado diretamente
 *                nesta funcionalidade, mantido por compatibilidade de
 *                assinatura com o restante do programa).
 *   origem     - nome da estacao de origem do trajeto.
 *   destino    - nome da estacao de destino do trajeto.
 *
 * Retorno:
 *   Nao ha retorno (void). O resultado (numero de estacoes, distancia
 *   total e caminho) e impresso diretamente na saida padrao, ou uma
 *   mensagem de erro caso origem/destino nao existam ou nao haja
 *   caminho possivel entre eles.
 */
void funcionalidade11(const char *arq_dados, const char *arq_indice, const char *origem, const char *destino) {
    // Constroi o grafo completo (abertura do arquivo, validacao,
    // montagem do mapa, criacao de vertices e arestas). A mensagem de
    // erro, caso ocorra, ja e impressa dentro de construir_grafo.
    Graph g;
    if (!construir_grafo(arq_dados, &g)) {
        return;
    }

    // --- Algoritmo de Dijkstra ---
    // Identifica os indices numericos correspondentes a origem e ao destino.
    int start_idx = buscar_indice_vertice(&g, origem);
    int end_idx = buscar_indice_vertice(&g, destino);

    // Valida se as estacoes informadas existem no grafo.
    if (start_idx == -1 || end_idx == -1) {
        printf("Registro inexistente.\n");
    } else {
        // dist[i]    - menor distancia conhecida da origem ate o vertice i.
        // prev[i]    - vertice anterior a i no caminho minimo (para reconstruir o trajeto).
        // visited[i] - marca se o vertice i ja foi definitivamente processado.
        int *dist = malloc(g.numVertices * sizeof(int));
        int *prev = malloc(g.numVertices * sizeof(int));
        int *visited = malloc(g.numVertices * sizeof(int));

        // Inicializacao padrao do Dijkstra: todas as distancias como
        // infinito, nenhum vertice visitado e nenhum predecessor definido.
        for (int i = 0; i < g.numVertices; i++) {
            dist[i] = INF;
            prev[i] = -1;
            visited[i] = 0;
        }

        dist[start_idx] = 0; // a distancia da origem para ela mesma e zero

        // Loop principal: processa todos os vertices do grafo, um por vez.
        for (int count = 0; count < g.numVertices; count++) {
            int u = -1;
            int min_dist = INF;

            // Seleciona, entre os vertices ainda nao visitados, aquele com
            // menor distancia atual conhecida. Percorrer sempre de 0 a V
            // garante o desempate alfabetico (indices menores tem prioridade).
            for (int i = 0; i < g.numVertices; i++) {
                if (!visited[i] && dist[i] < min_dist) {
                    min_dist = dist[i];
                    u = i;
                }
            }

            // Encerra o loop se nao houver mais vertices alcancaveis (u == -1)
            // ou se o destino ja foi definitivamente processado (u == end_idx).
            if (u == -1 || u == end_idx) break;
            visited[u] = 1;

            // Relaxamento das arestas que saem do vertice "u": tenta
            // melhorar a distancia de cada vizinho "v" passando por "u".
            Edge *curr = g.vertices[u].head;
            while (curr != NULL) {
                int v = buscar_indice_vertice(&g, curr->nomeProxEstacao);
                int weight = curr->distProxEstacao;

                if (!visited[v] && dist[u] != INF) {
                    // Encontrou um caminho estritamente mais curto ate "v".
                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        prev[v] = u;
                    }
                    // Empate de distancia: por desempate, preferimos o
                    // predecessor com menor indice (nome alfabeticamente menor).
                    else if (dist[u] + weight == dist[v]) {
                        if (u < prev[v]) {
                            prev[v] = u;
                        }
                    }
                }
                curr = curr->next;
            }
        }

        // --- Recuperacao e impressao do caminho encontrado ---
        if (dist[end_idx] == INF) {
            // Nao existe nenhum caminho da origem ate o destino.
            printf("Registro inexistente.\n");
        } else {
            // path: armazena, na ordem inversa (destino -> origem), os
            // vertices que compoem o caminho minimo encontrado.
            int *path = malloc(g.numVertices * sizeof(int));
            int path_len = 0;
            int curr = end_idx;

            // Reconstroi o caminho percorrendo os predecessores (prev[])
            // a partir do destino ate chegar na origem (prev == -1).
            while (curr != -1) {
                path[path_len++] = curr;
                curr = prev[curr];
            }

            // Subtrai 1 do tamanho do caminho, pois a origem nao deve ser
            // contabilizada como uma "estacao percorrida".
            printf("Numero de estacoes que serao percorridas: %d\n", path_len - 1);
            printf("Distancia que sera percorrida: %d\n", dist[end_idx]);

            // O vetor "path" foi montado do destino para a origem; portanto
            // percorremos de tras para frente para imprimir da origem ao destino.
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

    // Libera toda a memoria alocada dinamicamente para o grafo.
    liberar_grafo(&g);
}