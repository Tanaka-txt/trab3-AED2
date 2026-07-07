/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "grafo_builder.h"
#include "leitura.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>

void construir_mapa_e_vertices(FILE *fdados, MapEntry **mapa, int *tam_map, char ***nomes_unicos, int *qtd_nomes) {
    int cap_map = 50;
    *tam_map = 0;
    *mapa = malloc(cap_map * sizeof(MapEntry));

    int cap_nomes = 50;
    *qtd_nomes = 0;
    *nomes_unicos = malloc(cap_nomes * sizeof(char *));

    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue; // Registro removido

        if (*tam_map >= cap_map) {
            cap_map *= 2;
            *mapa = realloc(*mapa, cap_map * sizeof(MapEntry));
        }
        (*mapa)[*tam_map].cod = reg.codEstacao;
        (*mapa)[*tam_map].nome = strdup(reg.nomeEstacao);
        (*tam_map)++;

        int existe = 0;
        for (int i = 0; i < *qtd_nomes; i++) {
            if (strcmp((*nomes_unicos)[i], reg.nomeEstacao) == 0) {
                existe = 1;
                break;
            }
        }
        if (!existe) {
            if (*qtd_nomes >= cap_nomes) {
                cap_nomes *= 2;
                *nomes_unicos = realloc(*nomes_unicos, cap_nomes * sizeof(char *));
            }
            (*nomes_unicos)[*qtd_nomes] = strdup(reg.nomeEstacao);
            (*qtd_nomes)++;
        }
        liberar_strings_registro(&reg);
    }

    qsort(*mapa, *tam_map, sizeof(MapEntry), comparar_mapa);
    qsort(*nomes_unicos, *qtd_nomes, sizeof(char *), comparar_nomes_vertices);
}

void inicializar_vertices(Graph *g, char **nomes_unicos, int qtd_nomes) {
    g->numVertices = qtd_nomes;
    g->vertices = malloc(qtd_nomes * sizeof(Vertex));
    for (int i = 0; i < qtd_nomes; i++) {
        g->vertices[i].nomeEstacao = nomes_unicos[i];
        g->vertices[i].head = NULL;
    }
    free(nomes_unicos);
}

void construir_arestas_direcionado(FILE *fdados, Graph *g, MapEntry *mapa, int tam_map) {
    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        int idx_origem = buscar_indice_vertice(g, reg.nomeEstacao);
        if (idx_origem != -1) {
            // Requisito: Aresta para a próxima estação (codProxEstacao)
            if (reg.codProxEstacao != -1) {
                const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
                if (nome_dest != NULL) {
                    adicionar_aresta(&g->vertices[idx_origem], nome_dest, reg.distProxEstacao, reg.nomeLinha);
                }
            }

            // Requisito: Aresta de Integração (codEstIntegra)
            if (reg.codEstIntegra != -1) {
                const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
                if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                    adicionar_aresta(&g->vertices[idx_origem], nome_integ, 0, "Integração");
                }
            }
        }
        liberar_strings_registro(&reg);
    }
}

int comparar_info(const void *a, const void *b) {
    return ((InfoEntry *)a)->cod - ((InfoEntry *)b)->cod;
}

int buscar_info_por_codigo(InfoEntry *info, int tam, int codigo, InfoEntry *out) {
    int esq = 0, dir = tam - 1;
    while (esq <= dir) {
        int meio = esq + (dir - esq) / 2;
        if (info[meio].cod == codigo) { *out = info[meio]; return 1; }
        if (info[meio].cod < codigo) esq = meio + 1;
        else dir = meio - 1;
    }
    return 0;
}

void construir_info_entries(FILE *fdados, InfoEntry **infos, int *tam_info) {
    int cap_info = 50;
    *tam_info = 0;
    *infos = malloc(cap_info * sizeof(InfoEntry));

    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));
        int ret = ler_registro(fdados, &reg);
        if (ret == 0) break;
        if (ret == 2) continue;

        if (*tam_info >= cap_info) {
            cap_info *= 2;
            *infos = realloc(*infos, cap_info * sizeof(InfoEntry));
        }
        (*infos)[*tam_info].cod = reg.codEstacao;
        (*infos)[*tam_info].codProx = reg.codProxEstacao;
        (*infos)[*tam_info].codIntegra = reg.codEstIntegra;
        (*tam_info)++;

        liberar_strings_registro(&reg);
    }

    qsort(*infos, *tam_info, sizeof(InfoEntry), comparar_info);
}

/*
 * Função unificada para construir o grafo direcionado completo a partir do arquivo de dados.
 * Abre o arquivo, valida o cabeçalho, monta o mapa, os vértices e as arestas.
 * Retorna 1 em sucesso, 0 em falha (já imprime a mensagem de erro).
 * O grafo 'g' deve ser passado por referência e será alocado internamente.
 */
int construir_grafo(const char *nome_arquivo, Graph *g) {
    FILE *fdados = fopen(nome_arquivo, "rb");
    if (fdados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return 0;
    }

    reg_cabecalho cab;
    if (!validar_cabecalho(fdados, &cab)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fdados);
        return 0;
    }

    MapEntry *mapa;
    int tam_map;
    char **nomes_unicos;
    int qtd_nomes;

    construir_mapa_e_vertices(fdados, &mapa, &tam_map, &nomes_unicos, &qtd_nomes);
    inicializar_vertices(g, nomes_unicos, qtd_nomes);  // libera nomes_unicos internamente
    construir_arestas_direcionado(fdados, g, mapa, tam_map);

    for (int i = 0; i < tam_map; i++) {
        free(mapa[i].nome);
    }
    free(mapa);
    fclose(fdados);
    return 1;
}