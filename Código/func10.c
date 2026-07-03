/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

/*
 * func10.c
 * -----------------------------------------------------------------------
 * Funcionalidade 10: constroi o grafo de estacoes (via construir_grafo,
 * em grafo.c) e imprime, para cada estacao que possui ao menos uma
 * conexao de saida, o nome da estacao seguido das estacoes de destino,
 * distancia e linhas de metro associadas.
 * -----------------------------------------------------------------------
 */

#include "features.h"
#include "impressao.h"
#include <stdlib.h>
#include "grafo.h"

/*
 * funcionalidade10
 * -------------------
 * Constroi o grafo de estacoes a partir do arquivo de dados (delegando
 * a construcao para construir_grafo) e imprime, para cada estacao com
 * conexoes de saida, a lista de destinos alcancados diretamente
 * (estacao seguinte e integracoes), com distancia e linhas.
 *
 * Parametros:
 *   nome_arq_dados - caminho do arquivo binario de dados das estacoes.
 *
 * Retorno:
 *   Nao ha retorno (void). O resultado e impresso diretamente na saida
 *   padrao, ou uma mensagem de erro em caso de falha no processamento.
 */
void funcionalidade10(const char *nome_arq_dados) {
    // Constroi o grafo completo (abertura do arquivo, validacao,
    // montagem do mapa, criacao de vertices e arestas). A mensagem de
    // erro, caso ocorra, ja e impressa dentro de construir_grafo.
    Graph g;
    if (!construir_grafo(nome_arq_dados, &g)) {
        return;
    }

    // --- Impressao do resultado ---
    // Percorre todos os vertices do grafo e imprime apenas as estacoes
    // que possuem ao menos uma conexao de saida, seguidas de cada
    // destino, distancia e linhas associadas a conexao.
    for (int i = 0; i < g.numVertices; i++) {
        if (g.vertices[i].head == NULL) {
            continue; // estacao sem conexoes de saida nao deve ser impressa
        }

        printf("%s", g.vertices[i].nomeEstacao);

        Edge *curr = g.vertices[i].head;
        while (curr != NULL) {
            // Imprime o destino e a distancia, separados por virgula.
            printf(", %s, %d", curr->nomeProxEstacao, curr->distProxEstacao);

            // Imprime cada linha de metro associada a essa conexao.
            for (int j = 0; j < curr->numLinhas; j++) {
                printf(", %s", curr->nomesLinhas[j]);
            }
            curr = curr->next;
        }
        printf("\n");
    }

    // Libera toda a memoria alocada dinamicamente para o grafo.
    liberar_grafo(&g);
}