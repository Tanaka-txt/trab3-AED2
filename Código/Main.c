/*
Membros do grupo:
Laysa Almeida de Oliveira - NºUSP 14588002
Júlio César Tanaka Vergamini - NºUSP 15466276
*/

#include "features.h"
#include "fornecidas.h"

int main() {
  int option, RRN, n;
  char arq_csv[256], arq_bin[256], arq_indice[256]; // buffer do tamanho de uma linha

  while(scanf("%d", &option) == 1) {
    getchar(); // limpa o buffer por conta do \n
      switch (option){
        case 1 :
          scanf("%s %s", arq_csv, arq_bin);
          create_regi_bin(arq_csv, arq_bin);
          break;

        case 2 :
          scanf("%s", arq_bin);
          read_bin(arq_bin);
          break;

        case 3 :
          scanf("%s", arq_bin);
          busca_bin(arq_bin);
          break;

        case 4 :
          scanf("%s %d", arq_bin, &RRN);
          busca_por_rrn(arq_bin, RRN);
          break;
        
        case 5:;
          scanf("%s %s", arq_bin, arq_indice);
          funcionalidade5(arq_bin, arq_indice);
          break;
        
          case 6:
          scanf("%s %s %d", arq_bin, arq_indice, &n);
          funcionalidade6(arq_bin, arq_indice, n);
          break;

          case 7:
          scanf("%s %s %d", arq_bin, arq_indice, &n);
          funcionalidade7(arq_bin, arq_indice, n);
          break;

          case 8:
          scanf("%s %s %d", arq_bin, arq_indice, &n);
          funcionalidade8(arq_bin, arq_indice, n);
          break;

          case 9:
          scanf("%s %s %d", arq_bin, arq_indice, &n);
          funcionalidade9(arq_bin, arq_indice, n);
          break;

          case 10:
          scanf("%s", arq_bin);
          funcionalidade10(arq_bin);
          break;

          case 11: {
            char campo1[50], valor1[100], campo2[50], valor2[100];
            scanf("%s %s %s", arq_bin, arq_indice, campo1);
            ScanQuoteString(valor1);
            scanf("%s", campo2);
            ScanQuoteString(valor2);
            funcionalidade11(arq_bin, arq_indice, valor1, valor2);
            break;
        }

        case 12: {
            
            break;
        }

        case 13: {
            char campo1[50], valor1[100];
            scanf("%s %s %s", arq_bin, arq_indice, campo1);
            ScanQuoteString(valor1);
            funcionalidade13(arq_bin, arq_indice, valor1);
            break;
        }

        default:
          printf("Erro\n");
          exit(0);
      }
  }
  return 0;
}