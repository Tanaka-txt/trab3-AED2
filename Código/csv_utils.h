/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#ifndef CSV_UTILS_H
#define CSV_UTILS_H

#include "registro.h"

// Struct de auxilio para verificar a paridade das estações
typedef struct{
    int origem;
    int destino;
} Par;

int existe_estacao(char *nome, char **lista, int tamanho);

int existe_par(int a, int b, Par *lista, int tamanho);

void le_linha_csv(char *linha, reg_dados *registro, int topo);

#endif