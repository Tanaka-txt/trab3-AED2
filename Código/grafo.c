/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "grafo.h"
#include <stdlib.h>
#include <string.h>

// Compara duas strings (nomes de vértices) em ordem alfabética para o qsort
int comparar_nomes_vertices(const void *a, const void *b) { 
    return strcmp(*(const char **)a, *(const char **)b);
}
// Compara dois MapEntries numéricamente pelo código para o qsort
int comparar_mapa(const void *a, const void *b) { 
    return ((MapEntry *)a)->cod - ((MapEntry *)b)->cod;
}

const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo) {
    int esq = 0, dir = tam - 1; // limites inferior e superior da busca
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2; // indice central do intervalo atual
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

// Insere uma nova aresta na lista de adjacências de um vértice, mantendo a ordem alfabética do destino
void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha) { 
    Edge *curr = v->head;
    Edge *prev = NULL;

    while (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) < 0) { // Percorre a lista até encontrar a posição correta (ordem crescente do nome de destino)
        prev = curr;
        curr = curr->next;
    }

    if (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) == 0) { // Se a aresta para o destino já existe, apenas adicionamos a nova linha à aresta existente
        for (int i = 0; i < curr->numLinhas; i++) {
            if (strcmp(curr->nomesLinhas[i], linha) == 0) return;
        }
        curr->numLinhas++;
        curr->nomesLinhas = realloc(curr->nomesLinhas, curr->numLinhas * sizeof(char *));
        curr->nomesLinhas[curr->numLinhas - 1] = strdup(linha);

        for (int i = 0; i < curr->numLinhas - 1; i++) { // Reordena o vetor de linhas em ordem alfabética
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

    // Criação de uma nova aresta se a conexão ainda não existir
    Edge *new_edge = malloc(sizeof(Edge));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->numLinhas = 1;
    new_edge->nomesLinhas = malloc(sizeof(char *));
    new_edge->nomesLinhas[0] = strdup(linha);
    new_edge->next = curr; // Encadeia a nova aresta com o resto da lista

    // Ajusta o ponteiro da lista (início ou meio)
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






/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

/*
 * grafo.c
 * -----------------------------------------------------------------------
 * Implementacao das operacoes sobre o grafo definido em grafo.h:
 * comparacao para ordenacao (qsort), busca binaria, insercao de arestas
 * e liberacao de memoria. Este modulo e reutilizado pelas funcionalidades
 * 10, 11 e 13, evitando duplicacao da logica de construcao do grafo.
 * -----------------------------------------------------------------------
 */

#include "grafo.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * comparar_nomes_vertices
 * -------------------------
 * Compara duas strings (nomes de estacoes) em ordem alfabetica.
 * Usada como funcao de comparacao pelo qsort ao ordenar o vetor de
 * nomes unicos de estacoes antes de montar o vetor de vertices.
 */
int comparar_nomes_vertices(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

/*
 * comparar_mapa
 * ---------------
 * Compara dois MapEntry numericamente pelo campo "cod".
 * Usada como funcao de comparacao pelo qsort ao ordenar o mapa de
 * codigo->nome, que precisa estar ordenado para permitir busca binaria.
 */
int comparar_mapa(const void *a, const void *b) {
    return ((MapEntry *)a)->cod - ((MapEntry *)b)->cod;
}

/*
 * buscar_nome_por_codigo
 * -------------------------
 * Busca binaria sobre o vetor "mapa" (ordenado por codigo) para
 * localizar o nome da estacao associado ao codigo informado.
 * Pre-condicao: o vetor "mapa" deve estar ordenado por codigo
 * (ver comparar_mapa) antes de chamar esta funcao.
 */
const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo) {
    int esq = 0, dir = tam - 1; // limites inferior e superior da busca

    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2; // indice central do intervalo atual

        if (mapa[meio].cod == codigo) return mapa[meio].nome; // encontrado
        if (mapa[meio].cod < codigo) esq = meio + 1;          // busca na metade direita
        else dir = meio - 1;                                  // busca na metade esquerda
    }
    return NULL; // codigo nao encontrado no mapa
}

/*
 * buscar_indice_vertice
 * ------------------------
 * Busca binaria sobre o vetor de vertices do grafo (ordenado
 * alfabeticamente pelo nome da estacao) para localizar o indice do
 * vertice cujo nome corresponde ao parametro "nome".
 * Pre-condicao: g->vertices deve estar ordenado alfabeticamente
 * (ver comparar_nomes_vertices) antes de chamar esta funcao.
 */
int buscar_indice_vertice(Graph *g, const char *nome) {
    int esq = 0, dir = g->numVertices - 1; // limites inferior e superior da busca

    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2; // indice central do intervalo atual
        int cmp = strcmp(g->vertices[meio].nomeEstacao, nome);

        if (cmp == 0) return meio;   // vertice encontrado
        if (cmp < 0) esq = meio + 1; // busca na metade direita
        else dir = meio - 1;         // busca na metade esquerda
    }
    return -1; // estacao nao encontrada no grafo
}

/*
 * adicionar_aresta
 * -------------------
 * Insere (ou atualiza) uma aresta na lista de adjacencias de um
 * vertice, mantendo a lista sempre ordenada alfabeticamente pelo nome
 * da estacao de destino.
 *
 * Logica dividida em tres blocos:
 *   1) Percorre a lista encadeada ate encontrar a posicao correta
 *      (ordem alfabetica) ou a propria aresta de destino, caso exista.
 *   2) Se a aresta para o destino ja existir, apenas adiciona a nova
 *      linha de metro aquela aresta (evitando duplicar linhas ja
 *      registradas) e reordena o vetor de linhas alfabeticamente.
 *   3) Se a aresta nao existir, cria uma nova e a insere na posicao
 *      correta da lista encadeada.
 */
void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha) {
    Edge *curr = v->head; // aresta atual durante a percorrida da lista
    Edge *prev = NULL;    // aresta anterior a "curr" (para religar a lista)

    // Bloco 1: percorre a lista ate achar a posicao alfabetica correta
    // ou a aresta que ja aponta para o mesmo destino.
    while (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) < 0) {
        prev = curr;
        curr = curr->next;
    }

    // Bloco 2: aresta para o destino ja existe -> apenas acrescenta a linha.
    if (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) == 0) {
        // Verifica se a linha ja esta cadastrada nessa aresta, para nao duplicar.
        for (int i = 0; i < curr->numLinhas; i++) {
            if (strcmp(curr->nomesLinhas[i], linha) == 0) return;
        }

        // Aumenta o vetor de linhas em uma posicao e insere a nova linha.
        curr->numLinhas++;
        curr->nomesLinhas = realloc(curr->nomesLinhas, curr->numLinhas * sizeof(char *));
        curr->nomesLinhas[curr->numLinhas - 1] = strdup(linha);

        // Reordena o vetor de linhas em ordem alfabetica (Selection Sort simplificado:
        // fixa uma posicao "i" e troca pelo menor valor encontrado a frente dela).
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

    // Bloco 3: a aresta ainda nao existe -> cria uma nova e insere na lista.
    Edge *new_edge = malloc(sizeof(Edge));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->numLinhas = 1;
    new_edge->nomesLinhas = malloc(sizeof(char *));
    new_edge->nomesLinhas[0] = strdup(linha);
    new_edge->next = curr; // encadeia a nova aresta com o restante da lista

    // Ajusta o ponteiro de encadeamento: inicio da lista ou no meio dela.
    if (prev == NULL) v->head = new_edge;
    else prev->next = new_edge;
}

/*
 * liberar_grafo
 * ----------------
 * Percorre todos os vertices do grafo e libera a memoria alocada
 * dinamicamente: nome de cada vertice, cada aresta de sua lista de
 * adjacencias (incluindo o nome do destino e o vetor de linhas
 * associado), e por fim o vetor de vertices do grafo.
 */
void liberar_grafo(Graph *g) {
    for (int i = 0; i < g->numVertices; i++) {
        free(g->vertices[i].nomeEstacao); // libera o nome da estacao

        // Percorre e libera toda a lista encadeada de arestas do vertice.
        Edge *curr = g->vertices[i].head;
        while (curr != NULL) {
            Edge *next = curr->next; // guarda referencia antes de liberar "curr"

            free(curr->nomeProxEstacao); // libera nome da estacao de destino

            // Libera cada string do vetor de linhas e depois o vetor em si.
            for (int j = 0; j < curr->numLinhas; j++) {
                free(curr->nomesLinhas[j]);
            }
            free(curr->nomesLinhas);

            free(curr); // libera a propria aresta
            curr = next;
        }
    }
    free(g->vertices); // libera o vetor de vertices do grafo
}

/*
 * construir_grafo
 * ------------------
 * Rotina compartilhada de construcao do grafo a partir do arquivo
 * binario de dados das estacoes. Reune, em um unico lugar, a logica
 * que antes estava duplicada nas funcionalidades 10, 11 e 13.
 *
 * Dividida em blocos funcionais:
 *   1) Abertura e validacao do arquivo.
 *   2) Primeira passagem: montagem do mapa codigo->nome e coleta dos
 *      nomes unicos de estacoes.
 *   3) Ordenacao das estruturas auxiliares (necessaria para busca binaria).
 *   4) Criacao dos vertices do grafo.
 *   5) Segunda passagem: criacao das arestas (proxima estacao e integracoes).
 *   6) Liberacao do mapa auxiliar (nao e mais necessario apos esta etapa)
 *      e fechamento do arquivo.
 */
int construir_grafo(const char *nome_arquivo, Graph *g) {
    // --- Bloco 1: Abertura e validacao do arquivo ---
    FILE *fdados = fopen(nome_arquivo, "rb");
    if (fdados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    reg_cabecalho cab; // cabecalho lido apenas para fins de validacao
    if (!validar_cabecalho(fdados, &cab)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fdados);
        return 0;
    }

    // --- Estruturas auxiliares ---
    // mapa: traduz codigo numerico de estacao -> nome da estacao.
    // nomes_unicos: nomes distintos de estacoes, usados para criar os vertices.
    int cap_map = 50, tam_map = 0;
    MapEntry *mapa = malloc(cap_map * sizeof(MapEntry));
    int cap_nomes = 50, qtd_nomes = 0;
    char **nomes_unicos = malloc(cap_nomes * sizeof(char *));

    // --- Bloco 2: Primeira passagem pelo arquivo ---
    // Preenche o mapa de codigos e coleta o conjunto de nomes unicos de
    // estacoes, necessarios para montar os vertices antes de qualquer aresta.
    fseek(fdados, 17, SEEK_SET); // pula o cabecalho (17 bytes) e vai para o 1o registro
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;    // fim do arquivo
        if (ret == 2) continue; // registro logicamente removido, ignora

        // Adiciona a estacao atual ao mapa de codigos, realocando se necessario.
        if (tam_map >= cap_map) {
            cap_map *= 2;
            mapa = realloc(mapa, cap_map * sizeof(MapEntry));
        }
        mapa[tam_map].cod = reg.codEstacao;
        mapa[tam_map].nome = strdup(reg.nomeEstacao);
        tam_map++;

        // Verifica se o nome da estacao atual ja foi coletado.
        int existe = 0;
        for (int i = 0; i < qtd_nomes; i++) {
            if (strcmp(nomes_unicos[i], reg.nomeEstacao) == 0) {
                existe = 1;
                break;
            }
        }

        // Se for um nome novo, insere no vetor de nomes unicos.
        if (!existe) {
            if (qtd_nomes >= cap_nomes) {
                cap_nomes *= 2;
                nomes_unicos = realloc(nomes_unicos, cap_nomes * sizeof(char *));
            }
            nomes_unicos[qtd_nomes] = strdup(reg.nomeEstacao);
            qtd_nomes++;
        }
        liberar_strings_registro(&reg); // libera strings alocadas na leitura do registro
    }

    // --- Bloco 3: Ordenacao das estruturas auxiliares ---
    // Necessaria para permitir busca binaria no mapa (por codigo) e no
    // vetor de vertices do grafo (por nome), nas etapas seguintes.
    qsort(mapa, tam_map, sizeof(MapEntry), comparar_mapa);
    qsort(nomes_unicos, qtd_nomes, sizeof(char *), comparar_nomes_vertices);

    // --- Bloco 4: Criacao dos vertices do grafo ---
    // Cada nome unico de estacao vira um vertice, ainda sem nenhuma aresta.
    g->numVertices = qtd_nomes;
    g->vertices = malloc(qtd_nomes * sizeof(Vertex));
    for (int i = 0; i < qtd_nomes; i++) {
        g->vertices[i].nomeEstacao = nomes_unicos[i];
        g->vertices[i].head = NULL;
    }
    free(nomes_unicos); // o vetor em si nao e mais necessario (os nomes foram
                         // repassados como propriedade dos vertices do grafo)

    // --- Bloco 5: Segunda passagem pelo arquivo ---
    // Agora que todos os vertices existem, cria as arestas (conexoes)
    // entre as estacoes, usando o mapa codigo->nome.
    fseek(fdados, 17, SEEK_SET); // volta ao inicio dos registros
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        int idx_origem = buscar_indice_vertice(g, reg.nomeEstacao);
        if (idx_origem != -1) {

            // Requisito: aresta para a proxima estacao da mesma linha
            // (codProxEstacao == -1 significa que nao ha proxima estacao).
            if (reg.codProxEstacao != -1) {
                const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
                if (nome_dest != NULL) {
                    adicionar_aresta(&g->vertices[idx_origem], nome_dest, reg.distProxEstacao, reg.nomeLinha);
                }
            }

            // Requisito: aresta de integracao com outra linha
            // (codEstIntegra == -1 significa que a estacao nao faz integracao).
            if (reg.codEstIntegra != -1) {
                const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
                // Insere somente se a estacao de integracao tiver nome
                // diferente da estacao de origem (evita auto-laco).
                if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                    adicionar_aresta(&g->vertices[idx_origem], nome_integ, 0, "Integração");
                }
            }
        }
        liberar_strings_registro(&reg);
    }

    // --- Bloco 6: Liberacao do mapa auxiliar e fechamento do arquivo ---
    // O mapa codigo->nome so e necessario durante a construcao das
    // arestas; a partir daqui o grafo ja e autossuficiente.
    for (int i = 0; i < tam_map; i++) {
        free(mapa[i].nome);
    }
    free(mapa);
    fclose(fdados);

    return 1; // grafo construido com sucesso
}