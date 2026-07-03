/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "features.h"
#include "registro.h"
#include "leitura.h"
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include "grafo.h"
#include "grafo_mst.h"
#include "grafo_builder.h"

// Uso esse valor alto para garantir que qualquer aresta real seja menor que ele, isso ajuda a achar a aresta mínima durante a construção da MST
#define INF 1000000000 

// Func 12 contrói a MST do grafo de estações e linhas, começando da estação de origem fornecida
void funcionalidade12(const char *arq_dados, const char *arq_indice, const char *origem) {
    // valida o arquivo
    FILE *fdados = fopen(arq_dados, "rb");
    if (fdados == NULL) {
        printf("Falha no processamento do arquivo.\n");
        return;
    }

    reg_cabecalho cab;
    // valida o cabeçalho do arquivo bin, verificação de inconsistência
    if (!validar_cabecalho(fdados, &cab)) {
        printf("Falha no processamento do arquivo.\n");
        fclose(fdados);
        return;
    }

    // Conversão do Código da estação para o nome (Mapa)
    MapEntry *mapa;
    int tam_map;
    char **nomes_unicos;
    int qtd_nomes;
    
    // le o arquivo e constroi o mapa de código para nome
    construir_mapa_e_vertices(fdados, &mapa, &tam_map, &nomes_unicos, &qtd_nomes);

    InfoEntry *infos;
    int tam_info;
    // guarda informações extras, integração e seguinte para validação
    construir_info_entries(fdados, &infos, &tam_info);

    // cria a estrutura do grafo a MST
    GraphMST g;

    // inicialização da lista de adjacência com nomes encontrados
    inicializar_vertices_mst(&g, nomes_unicos, qtd_nomes);

    // pula registros do arquivo direto para os reg
    fseek(fdados, 17, SEEK_SET);
    reg_dados reg;
    
    while (1) {
        memset(&reg, 0, sizeof(reg_dados));

        // Le arquivo e retorna 0, EOF
        if (ler_registro(fdados, &reg) == 0) break;
        // ignora logicamente removidos
        if (reg.status_removido == '1') continue;

        // procura posi, vertice origem do grafo
        int idx_origem = buscar_indice_vertice_mst(&g, reg.nomeEstacao);
        if (idx_origem == -1) {
            liberar_strings_registro(&reg);
            continue;
        }

        // conexão de próxima estação se existir e se o destino for valido
        if (reg.codProxEstacao != -1) {
            // tradução do código pro nome
            const char *nome_dest = buscar_nome_por_codigo(mapa, tam_map, reg.codProxEstacao);
            
            if (nome_dest != NULL) {
                // valida se o destino tem um próximo ou uma integração válida
                InfoEntry info_dest;
                int achou_info = buscar_info_por_codigo(infos, tam_info, reg.codProxEstacao, &info_dest);
                int destino_valido = 0;
                
                if (achou_info) {
                    int prox_existe = (info_dest.codProx != -1) && (buscar_nome_por_codigo(mapa, tam_map, info_dest.codProx) != NULL);
                    int integra_existe = (info_dest.codIntegra != -1) && (buscar_nome_por_codigo(mapa, tam_map, info_dest.codIntegra) != NULL);
                    destino_valido = prox_existe || integra_existe;
                }
                
                if (destino_valido) {
                    // mst não direcional = aresta é adicionada nos dois sentidos
                    adicionar_aresta_mst(&g.vertices[idx_origem], nome_dest, reg.distProxEstacao); // Ida
                    int idx_dest = buscar_indice_vertice_mst(&g, nome_dest);
                    if (idx_dest != -1) {
                        adicionar_aresta_mst(&g.vertices[idx_dest], g.vertices[idx_origem].nomeEstacao, reg.distProxEstacao); // Volta
                    }
                }
            }
        }


        // conexão de outras linhas se existir e se o destino for valido
        if (reg.codEstIntegra != -1) {
            const char *nome_integ = buscar_nome_por_codigo(mapa, tam_map, reg.codEstIntegra);
            
            // Verifica se a integração existe e se não é uma estação integrando com ela mesma.
            if (nome_integ != NULL && strcmp(reg.nomeEstacao, nome_integ) != 0) {
                // A distância é 0 se for integração 
                adicionar_aresta_mst(&g.vertices[idx_origem], nome_integ, 0);
                int idx_dest = buscar_indice_vertice_mst(&g, nome_integ);
                if (idx_dest != -1) {
                    adicionar_aresta_mst(&g.vertices[idx_dest], g.vertices[idx_origem].nomeEstacao, 0);
                }
            }
        }

        // limpa memoria da string dinâmicas lidas no registro atual para evitar vazamentos
        liberar_strings_registro(&reg);
    }

    // ve origem de nó
    int start_idx = buscar_indice_vertice_mst(&g, origem);
    // Se a estação passada pelo usuário não existir no grafo, encerra o programa limpando tudo
    if (start_idx == -1) {
        printf("Registro inexistente.\n");
        liberar_grafo_mst(&g);
        for (int i = 0; i < tam_map; i++) free(mapa[i].nome);
        free(mapa);
        free(infos);
        fclose(fdados);
        return;
    }

    //Rastreia quais vertices já foram colocados na arvore
    int *visited = calloc(g.numVertices, sizeof(int));
    visited[start_idx] = 1; // Começamos a árvore pelo nó de origem

    // Grafo de n vertices precisa de n-1 arestas para conectar tudo
    for (int count = 0; count < g.numVertices - 1; count++) {
        int best_u = -1, best_v = -1, min_dist = INF;

        // Passa por todos os vertices até ver um já colocado (u) Não visitado = (V)
        for (int u = 0; u < g.numVertices; u++) {
            if (visited[u]) {
                EdgeMST *curr = g.vertices[u].head;
                
                // avalia todas as arestas saindo de u
                while (curr != NULL) {
                    int v = buscar_indice_vertice_mst(&g, curr->nomeProxEstacao);
                    
                    // Se o destino ainda não está na árvore é um candidato válido
                    if (!visited[v]) {
                        // Acha arestas menor que a minima
                        if (curr->distProxEstacao < min_dist) {
                            min_dist = curr->distProxEstacao;
                            best_u = u;
                            best_v = v;
                        } 
                        // Desempate = se a distancia for igual, prioriza o vértice com menor índice no array de vértices.
                        else if (curr->distProxEstacao == min_dist) {
                            if (u < best_u) {
                                best_u = u;
                                best_v = v;
                            } else if (u == best_u && v < best_v) {
                                best_v = v;
                            }
                        }
                    }
                    curr = curr->next;
                }
            }
        }

        // Se encontrou uma aresta valida:
        if (best_u != -1 && best_v != -1) {
            // Varre as arestas do vértice vencedor para marcar qual foi escolhida
            EdgeMST *e = g.vertices[best_u].head;
            while (e != NULL) {
                // Ao encontrar a aresta especifica que venceu marca a flag
                if (strcmp(e->nomeProxEstacao, g.vertices[best_v].nomeEstacao) == 0 && e->distProxEstacao == min_dist) {
                    e->InMST = 1;
                    break;
                }
                e = e->next;
            }
            // Marca o vértice destino como visitado para a próxima rodada do laço
            visited[best_v] = 1;
        } else {
            // se for -1 então não há mais arestas para adicionar o grafo é desconexo
            break; 
        }
    }

    // impressão do MST por visitados, unico da busca em profundidade para não confundir com o vetor de visitados da construção da MST
    int *visitado_dfs = calloc(g.numVertices, sizeof(int));
    
    dfs_imprime_mst(&g, start_idx, visitado_dfs);
    free(visitado_dfs);

    // liberação 
    free(visited);
    for (int i = 0; i < tam_map; i++) free(mapa[i].nome);
    free(mapa);
    free(infos);
    liberar_grafo_mst(&g);
    fclose(fdados);
}