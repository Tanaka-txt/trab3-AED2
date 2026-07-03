/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

// Funcionalidade 11: constrói o grafo de estações e aplica o algoritmo de Dijkstra para encontrar o caminho de menor distância entre uma estação de origem e uma estação de destino. 
// No final, imprime o número de estações percorridas, a distância total é a sequência de estações do caminho encontrado.

#include "features.h"
#include <stdlib.h>
#include "grafo.h"

#define INF 1000000000 // Valor usado para representar "distância infinita" no Dijkstra

void funcionalidade11(const char *arq_dados, const char *arq_indice, const char *origem, const char *destino) {
    Graph g; // Constrói o grafo completo
    if (!construir_grafo(arq_dados, &g)) {
        return;
    }

    // Algoritmo de Dijkstra: identifica os índices numéricos correspondentes a origem e ao destino
    int start_idx = buscar_indice_vertice(&g, origem);
    int end_idx = buscar_indice_vertice(&g, destino);

    // Valida se as estações informadas existem no grafo
    if (start_idx == -1 || end_idx == -1) {
        printf("Registro inexistente.\n");
    } else {
        int *dist = malloc(g.numVertices * sizeof(int));    // dist[i]    - menor distância conhecida da origem até o vértice i
        int *prev = malloc(g.numVertices * sizeof(int));    // prev[i]    - vértice anterior a i no caminho mínimo
        int *visited = malloc(g.numVertices * sizeof(int)); // visited[i] - marca se o vértice i já foi processado

        // Inicialização do Dijkstra: todas as distâncias como infinito, nenhum vértice visitado e nenhum predecessor definido
        for (int i = 0; i < g.numVertices; i++) {
            dist[i] = INF;
            prev[i] = -1;
            visited[i] = 0;
        }

        dist[start_idx] = 0; // A distância da origem para ela mesma é zero

        // Loop principal: processa todos os vértices do grafo
        for (int count = 0; count < g.numVertices; count++) {
            int u = -1;
            int min_dist = INF;

            // Seleciona, entre os vértices ainda não visitados, aquele com menor distância 
            for (int i = 0; i < g.numVertices; i++) {
                if (!visited[i] && dist[i] < min_dist) {
                    min_dist = dist[i];
                    u = i;
                }
            }

            // Encerra o loop se não houver mais vértices alcancaveis (u == -1) ou se o destino já foi processado (u == end_idx).
            if (u == -1 || u == end_idx) break;
            visited[u] = 1;

            Edge *curr = g.vertices[u].head;
            while (curr != NULL) {
                int v = buscar_indice_vertice(&g, curr->nomeProxEstacao);
                int weight = curr->distProxEstacao;

                if (!visited[v] && dist[u] != INF) {
                    // Encontrou um caminho estritamente mais curto até "v".
                    if (dist[u] + weight < dist[v]) {
                        dist[v] = dist[u] + weight;
                        prev[v] = u;
                    }
                    // Empate de distância: para desempatar --> predecessor com menor índice (nome alfabeticamente menor)
                    else if (dist[u] + weight == dist[v]) {
                        if (u < prev[v]) {
                            prev[v] = u;
                        }
                    }
                }
                curr = curr->next;
            }
        }

        if (dist[end_idx] == INF) {
            // Não existe nenhum caminho da origem até o destino
            printf("Registro inexistente.\n");
        } else {
            // path: armazena, na ordem inversa (destino --> origem), os vértices que compõem o caminho mínimo encontrado
            int *path = malloc(g.numVertices * sizeof(int));
            int path_len = 0;
            int curr = end_idx;

            // Reconstrói o caminho percorrendo os predecessores (prev[]) partindo do destino até chegar na origem (prev == -1)
            while (curr != -1) {
                path[path_len++] = curr;
                curr = prev[curr];
            }

            // Subtrai 1 do tamanho do caminho --> a origem não deve ser contabilizada 
            printf("Numero de estacoes que serao percorridas: %d\n", path_len - 1);
            printf("Distancia que sera percorrida: %d\n", dist[end_idx]);

            // O vetor "path" foi montado do destino para a origem
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

    liberar_grafo(&g); // Libera a memória alocada
}