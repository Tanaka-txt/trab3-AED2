/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "csv_utils.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "registro.h"

int existe_estacao(char *nome, char **lista, int tamanho){
    for(int i = 0; i < tamanho; i++){
        if(strcmp(nome, lista[i]) == 0)
            return 1;
    }
    return 0;
}

int existe_par(int a, int b, Par *lista, int tamanho){
    for(int i = 0; i < tamanho; i++){
        if(lista[i].origem == a && lista[i].destino == b)
            return 1;
    }
    return 0;
}

void le_linha_csv(char *linha, reg_dados *registro, int topo) {
    char *ptr = linha;
    char *temp;

    registro->status_removido = '0'; // status inicial
    registro->prox_queue = topo; // o próximo da fila é o topo atual, que é passado como argumento para a função

    // - CodEstação
    temp = strsep(&ptr, ",");
    if (temp != NULL && strlen(temp) > 0) {
        registro->codEstacao = atoi(temp);
    }

    // - Nome estação
    temp = strsep(&ptr, ",");
    registro->nomeEstacao = malloc(strlen(temp) + 1); 
    temp[strcspn(temp, "\r\n")] = '\0';  // Isso garante que o fgets não bugue com o buffer registrando \n e possíbilitando entradas aleatórias pelo buffer
    strcpy(registro->nomeEstacao, temp); 
    registro->tamNomeEstacao = strlen(temp);

    // - CodLinha
    temp = strsep(&ptr, ",");
    if(temp == NULL || strlen(temp) == 0) {
        registro->codLinha = -1; // se o campo estiver vazio ou for nulo, atribui -1 para indicar que não há próximo
    } else {
        registro->codLinha = atoi(temp);
    }

    // - Nome Linha
    temp = strsep(&ptr, ",");
    registro->nomeLinha = malloc(strlen(temp) + 1); 
    temp[strcspn(temp, "\r\n")] = '\0'; // Isso garante que o fgets não bugue com o buffer registrando \n e possíbilitando entradas aleatórias pelo buffer
    strcpy(registro->nomeLinha, temp); 
    registro->tamNomeLinha = strlen(temp); 

    // - Cod Prox Estac
    temp = strsep(&ptr, ",");
    if(temp == NULL || strlen(temp) == 0) {
        registro->codProxEstacao = -1; // se o campo estiver vazio ou for nulo, atribui -1 para indicar que não há próximo
    } else {
        registro->codProxEstacao = atoi(temp); 
    }

    // - Dist Proxi Estac
    temp = strsep(&ptr, ",");
    if(temp == NULL || strlen(temp) == 0) {
        registro->distProxEstacao = -1; // se o campo estiver vazio ou for nulo, atribui -1 para indicar que não há próximo
    } else {
        registro->distProxEstacao = atoi(temp);
    }

    // - Cod Linha Inte
    temp = strsep(&ptr, ",");
    if (temp == NULL || strlen(temp) == 0) {
        registro->codLinhaIntegra = -1; // se o campo estiver vazio ou for nulo, atribui -1 para indicar que não há próximo
    } else {
        registro->codLinhaIntegra = atoi(temp);
    }

    // - Cod Est Integra
    temp = strsep(&ptr, ",");
    if (temp != NULL) { // fazemos isso porque o campo anterior visto usou fgets ai garantimos a limpeza do buffer
        temp[strcspn(temp, "\r\n")] = '\0';  // Isso garante que o fgets não bugue com o buffer registrando \n e possíbilitando entradas aleatórias pelo buffer
    }
    if (temp == NULL || strlen(temp) == 0) { 
        registro->codEstIntegra = -1; // se o campo estiver vazio ou for nulo, atribui -1 para indicar que não há próximo
    } else {
        registro->codEstIntegra = atoi(temp);
    }
}