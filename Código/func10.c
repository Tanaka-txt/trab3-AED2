#include "features.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include "impressao.h"
#include <stdlib.h>
#include <string.h>
#include "grafo.h"

/*
// --- Estruturas de Dados do Grafo ---

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

// --- Funções Auxiliares do Grafo ---

int comparar_nomes_vertices(const void *a, const void *b) {
    return strcmp(*(const char **)a, *(const char **)b);
}

int comparar_mapa(const void *a, const void *b) {
    return ((MapEntry *)a)->cod - ((MapEntry *)b)->cod;
}

const char* buscar_nome_por_codigo(MapEntry *mapa, int tam, int codigo) {
    int esq = 0, dir = tam - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
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

void adicionar_aresta(Vertex *v, const char *nomeDest, int dist, const char *linha) {
    Edge *curr = v->head;
    Edge *prev = NULL;

    // Busca a posição correta (ordenação alfabética pelo nome da próxima estação)
    while (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) < 0) {
        prev = curr;
        curr = curr->next;
    }

    // Se a aresta para esse destino já existe, insere apenas a linha (se não for duplicada)
    if (curr != NULL && strcmp(curr->nomeProxEstacao, nomeDest) == 0) {
        for (int i = 0; i < curr->numLinhas; i++) {
            if (strcmp(curr->nomesLinhas[i], linha) == 0) return; 
        }
        curr->numLinhas++;
        curr->nomesLinhas = realloc(curr->nomesLinhas, curr->numLinhas * sizeof(char *));
        curr->nomesLinhas[curr->numLinhas - 1] = strdup(linha);

        // Ordena as linhas alfabeticamente
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

    // Cria e insere nova aresta
    Edge *new_edge = malloc(sizeof(Edge));
    new_edge->nomeProxEstacao = strdup(nomeDest);
    new_edge->distProxEstacao = dist;
    new_edge->numLinhas = 1;
    new_edge->nomesLinhas = malloc(sizeof(char *));
    new_edge->nomesLinhas[0] = strdup(linha);
    new_edge->next = curr;

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
*/
// --- Implementação Principal da Funcionalidade 10 ---

void funcionalidade10(const char *nome_arq_dados) {
    FILE *fdados = fopen(nome_arq_dados, "rb");
    if (fdados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    reg_cabecalho cab;
    if (!validar_cabecalho(fdados, &cab)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fdados);
        return;
    }

    int cap_map = 50, tam_map = 0;
    MapEntry *mapa = malloc(cap_map * sizeof(MapEntry));

    int cap_nomes = 50, qtd_nomes = 0;
    char **nomes_unicos = malloc(cap_nomes * sizeof(char *));

    // Primeira passagem: Preencher o mapa de códigos e coletar vértices únicos
    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue; // Registro removido

        if (tam_map >= cap_map) {
            cap_map *= 2;
            mapa = realloc(mapa, cap_map * sizeof(MapEntry));
        }
        mapa[tam_map].cod = reg.codEstacao;
        mapa[tam_map].nome = strdup(reg.nomeEstacao);
        tam_map++;

        int existe = 0;
        for (int i = 0; i < qtd_nomes; i++) {
            if (strcmp(nomes_unicos[i], reg.nomeEstacao) == 0) {
                existe = 1;
                break;
            }
        }
        if (!existe) {
            if (qtd_nomes >= cap_nomes) {
                cap_nomes *= 2;
                nomes_unicos = realloc(nomes_unicos, cap_nomes * sizeof(char *));
            }
            nomes_unicos[qtd_nomes] = strdup(reg.nomeEstacao);
            qtd_nomes++;
        }
        liberar_strings_registro(&reg);
    }

    // Ordenação das estruturas auxiliares
    qsort(mapa, tam_map, sizeof(MapEntry), comparar_mapa);
    qsort(nomes_unicos, qtd_nomes, sizeof(char *), comparar_nomes_vertices);

    // Inicialização do Grafo
    Graph g;
    g.numVertices = qtd_nomes;
    g.vertices = malloc(qtd_nomes * sizeof(Vertex));
    for (int i = 0; i < qtd_nomes; i++) {
        g.vertices[i].nomeEstacao = nomes_unicos[i]; 
        g.vertices[i].head = NULL;
    }
    free(nomes_unicos); 

    // Segunda passagem: Construir as arestas (Listas de Adjacência)
    fseek(fdados, 17, SEEK_SET);
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        int idx_origem = buscar_indice_vertice(&g, reg.nomeEstacao);
        if (idx_origem != -1) {
            
            // Requisito: Aresta para a próxima estação (codProxEstacao)
            if (reg.codProxEstacao != -1) {
                const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
                if (nome_dest != NULL) {
                    adicionar_aresta(&g.vertices[idx_origem], nome_dest, reg.distProxEstacao, reg.nomeLinha);
                }
            }
            
            // Requisito: Aresta de Integração (codEstIntegra)
            if (reg.codEstIntegra != -1) {
                const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
                // Inserir apenas se as estações tiverem nomes diferentes
                if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                    adicionar_aresta(&g.vertices[idx_origem], nome_integ, 0, "Integração");
                }
            }
        }
        liberar_strings_registro(&reg);
    }

    // --- CORREÇÃO: Exibição do Grafo no formato esperado ---
    for (int i = 0; i < g.numVertices; i++) {
        // Se a estação não tem arestas de saída, ela não deve ser impressa
        if (g.vertices[i].head == NULL) {
            continue;
        }

        // Imprime a origem
        printf("%s", g.vertices[i].nomeEstacao);
        
        Edge *curr = g.vertices[i].head;
        while (curr != NULL) {
            // Imprime o destino e a distância com a vírgula separadora
            printf(", %s, %d", curr->nomeProxEstacao, curr->distProxEstacao);
            
            // Imprime as linhas associadas também com vírgulas
            for (int j = 0; j < curr->numLinhas; j++) {
                printf(", %s", curr->nomesLinhas[j]);
            }
            curr = curr->next;
        }
        printf("\n");
    }

    // Limpeza de toda a memória alocada dinamicamente
    for (int i = 0; i < tam_map; i++) {
        free(mapa[i].nome);
    }
    free(mapa);
    liberar_grafo(&g);
    fclose(fdados);
}