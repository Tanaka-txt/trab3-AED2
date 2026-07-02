## ⚠️ Restrições Gerais do Projeto
Antes de iniciar as funcionalidades, certifique-se de que sua implementação em **C** garanta as seguintes regras globais:

* **Identificação:** O código deve conter um comentário no início com o NUSP e nome do(s) aluno(s).
* **Manipulação de Arquivos:** * Uso exclusivo da biblioteca `<stdio.h>`.
    * Leitura e gravação estritamente em **modo binário** (nada de modo texto).
    * A escrita dos dados deve ser feita **campo a campo** (não é permitido escrever o registro inteiro de uma vez).
* **Estrutura dos Dados:** Nomes, tamanhos, ordem e existência dos campos são fixos. Não adicione nem altere campos.
* **Documentação:** O código deve ser rigorosamente documentado (funções, procedimentos, variáveis e blocos lógicos).
* **Tratamento de Dados:**
    * Manipule valores nulos conforme a especificação de cada funcionalidade.
    * Não é necessário tratar truncamento de dados.
* **Saídas:** Exiba apenas as mensagens de erro ou saídas exatamente como especificado.

---

## Funcionalidades de Análise de Grafos

### Funcionalidade [10] - Gerar Grafo e Listas de Adjacência
Recupera os registros do arquivo binário e constrói o grafo na memória.
* **Vértices:** Nomes das estações (ordenados alfabeticamente).
* **Arestas:** Conexões direcionadas entre estações (próxima estação e integração).
* **Saída:** Lista de adjacências formatada, onde cada linha inicia com a estação origem seguida de seus destinos e pesos.



### Funcionalidade [11] - Caminho Mais Curto (Dijkstra)
Determina o caminho com a menor distância entre uma estação origem e uma estação destino.
* **Critério:** Algoritmo de Dijkstra.
* **Desempate:** Aplicação rigorosa de ordenação alfabética (vértices e arestas) conforme exigido para garantir a determinidade dos resultados.



### Funcionalidade [12] - Árvore Geradora Mínima (Prim)
Projeta a malha de menor custo possível mantendo a conectividade total entre todas as estações.
* **Critério:** Algoritmo de Prim.
* **Conectividade:** Tratamento da malha como bidirecional (ida e volta).
* **Saída:** Listagem das arestas selecionadas para compor a árvore geradora mínima.



### Funcionalidade [13] - Ciclos Simples
Conta a quantidade de ciclos simples existentes a partir de uma estação de origem.
* **Critério:** Busca em Profundidade (DFS) com *Backtracking*.
* **Definição:** Ciclo onde não há repetição de vértices, com exceção da estação de origem que é o próprio ponto de destino.
* **Saída:** Quantidade total de ciclos encontrados ou `-1` caso a estação informada seja inexistente na rede.

---
