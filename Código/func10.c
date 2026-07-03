/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "features.h"
#include "impressao.h"
#include <stdlib.h>
#include "grafo.h"

// Funcionalidade 10: constrói o grafo de estações a partir do arquivo de dados e imprime, para cada estação que possui ao menos uma conexão de saída: 
// o nome da estação seguido das estações de destino, distância e linhas de metrô associadas.

void funcionalidade10(const char *nome_arq_dados) {
    // Constrói o grafo completo
    Graph g;
    if (!construir_grafo(nome_arq_dados, &g)) {
        return;
    }

// Percorre todos os vértices do grafo e imprime apenas as estações que possuem ao menos uma conexão de saída, seguidas de cada destino, distância e linhas associadas à conexão
    for (int i = 0; i < g.numVertices; i++) {
        if (g.vertices[i].head == NULL) {
            continue; 
        }

        printf("%s", g.vertices[i].nomeEstacao);

        Edge *curr = g.vertices[i].head;
        while (curr != NULL) {
            
            printf(", %s, %d", curr->nomeProxEstacao, curr->distProxEstacao); // Imprime o destino e a distância, separados por vírgula
            for (int j = 0; j < curr->numLinhas; j++) {
                printf(", %s", curr->nomesLinhas[j]); // Imprime cada linha de metrô associada a essa conexão
            }
            curr = curr->next;
        }
        printf("\n");
    }

    liberar_grafo(&g); // Libera a memória alocada
}