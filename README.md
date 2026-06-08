````
# 📋 Gerenciador de Tarefas em C

Sistema de gerenciamento de tarefas via linha de comando, desenvolvido em C puro como projeto acadêmico. Utiliza estruturas de dados dinâmicas (Fila e Pilha encadeadas) e persistência em arquivo texto.

---

## 📌 Descrição do problema

Em ambientes acadêmicos e profissionais, é comum acumular tarefas sem um controle claro de ordem e prioridade. Sem uma ferramenta adequada, tarefas antigas se perdem, a ordem de execução fica confusa e não há como desfazer ações feitas por engano.

Este sistema resolve esse problema oferecendo um gerenciador de tarefas via linha de comando que garante o processamento ordenado das atividades (a mais antiga sempre é concluída primeiro), permite desfazer conclusões feitas por engano e persiste os dados entre execuções, sem depender de banco de dados ou bibliotecas externas — apenas C puro com a biblioteca padrão.

---

## 🛠️ Tecnologias utilizadas

| Tecnologia | Descrição |
|---|---|
| **Linguagem C (padrão C11)** | Toda a lógica do sistema foi escrita em C puro, sem uso de C++ ou bibliotecas externas |
| **Alocação dinâmica de memória** | Uso de `malloc` para reservar memória em tempo de execução e `free` para liberá-la, evitando memory leaks |
| **Estruturas encadeadas (linked lists)** | Fila e Pilha implementadas com nós ligados por ponteiros, sem arrays de tamanho fixo |
| **Ponteiros** | Utilizados para encadear nós, passar variáveis por referência entre funções e acessar estruturas alocadas dinamicamente |
| **Manipulação de arquivos** | Uso de `fopen`, `fclose`, `fprintf` e `fscanf` para leitura e escrita do arquivo de persistência `tarefas.txt` |
| **Entrada e saída padrão** | Uso de `printf`, `scanf` e `fgets` para interação com o usuário via terminal |
| **Biblioteca `<stdio.h>`** | Funções de entrada, saída e manipulação de arquivos |
| **Biblioteca `<stdlib.h>`** | Funções de alocação de memória (`malloc`, `free`) e controle de execução (`exit`) |
| **Biblioteca `<string.h>`** | Funções de manipulação de texto (`strncpy`, `strlen`) |

---

## ⚙️ Funcionalidades

**➕ Adicionar tarefa**
O usuário digita uma descrição e o sistema gera automaticamente um ID sequencial único. A tarefa é inserida no fim da fila de pendentes.

**📋 Listar tarefas pendentes**
Exibe todas as tarefas atualmente na fila, na ordem em que foram adicionadas, com seus respectivos IDs e descrições.

**✅ Concluir próxima tarefa**
Remove a tarefa mais antiga da fila (a primeira a ter sido adicionada) e a registra no histórico de concluídas. Segue rigorosamente a ordem de chegada.

**↩️ Desfazer última conclusão (UNDO)**
Restaura a tarefa concluída mais recentemente, removendo-a do histórico e reinserindo-a no fim da fila de pendentes. Permite corrigir conclusões feitas por engano.

**💾 Salvar e sair**
Salva todas as tarefas pendentes no arquivo `tarefas.txt` e encerra o programa. Na próxima execução, as tarefas são restauradas automaticamente.

---

## 🔄 Como o programa funciona

Ao iniciar, o programa tenta abrir o arquivo `tarefas.txt`. Se ele existir, as tarefas salvas são carregadas de volta para a fila e o contador de IDs é restaurado a partir do maior ID encontrado. Se o arquivo não existir (primeira execução), o programa inicia normalmente com a fila vazia e ID inicial igual a 1.

Em seguida, um menu interativo é exibido em loop. O usuário escolhe uma opção digitando um número. Após cada ação, o menu reaparece automaticamente até que o usuário escolha a opção de sair. Ao sair, as tarefas pendentes são gravadas no arquivo e toda a memória alocada é liberada antes do programa encerrar.

---

## 🗂️ Estruturas de dados utilizadas e por quê

### Fila Encadeada (FIFO) — tarefas pendentes

A fila foi escolhida porque reflete naturalmente a regra de negócio: a primeira tarefa adicionada deve ser a primeira a ser concluída. Ninguém deve "furar a fila". A implementação usa dois ponteiros (`frente` e `tras`), o que garante que tanto a inserção no fim quanto a remoção do início sejam realizadas em tempo O(1), sem percorrer a lista inteira.

### Pilha Encadeada (LIFO) — histórico de concluídas

A pilha foi escolhida para implementar o mecanismo de desfazer (UNDO). A última tarefa concluída deve ser a primeira a ser restaurada — comportamento exatamente igual ao de uma pilha. É o mesmo princípio do Ctrl+Z em editores de texto. A inserção e remoção pelo topo também são O(1).

Ambas as estruturas usam nós alocados dinamicamente com `malloc` e `free`, sem arrays de tamanho fixo, eliminando o risco de estouro por limite pré-definido de tarefas.

---

## 📊 Como o código utiliza a Fila

```
ESTADO INICIAL DA FILA (vazia):
frente → NULL
tras   → NULL

APÓS ADICIONAR "Estudar" (ID 1):
frente → [1|Estudar|NULL] ← tras

APÓS ADICIONAR "Trabalho" (ID 2):
frente → [1|Estudar|→] → [2|Trabalho|NULL] ← tras

APÓS CONCLUIR (remove da frente):
frente → [2|Trabalho|NULL] ← tras
nó [1|Estudar] vai para a Pilha
```

A inserção sempre ocorre pelo `tras` e a remoção sempre ocorre pelo `frente`. O nó removido não é destruído — ele migra para a Pilha.

---

## 📊 Como o código utiliza a Pilha

```
ESTADO INICIAL DA PILHA (vazia):
topo → NULL

APÓS CONCLUIR "Estudar" (ID 1):
topo → [1|Estudar|NULL]

APÓS CONCLUIR "Trabalho" (ID 2):
topo → [2|Trabalho|→] → [1|Estudar|NULL]

APÓS DESFAZER (remove do topo):
topo → [1|Estudar|NULL]
nó [2|Trabalho] volta para o FIM da Fila
```

A inserção e remoção sempre ocorrem pelo `topo`. O nó removido da pilha tem seu conteúdo copiado para um novo nó inserido no fim da Fila, e o nó antigo é liberado com `free`.

---

## 🔧 Principais funções do código

### Funções da Fila

| Função | O que faz |
|---|---|
| `criarFila` | Aloca memória para uma nova Fila vazia e inicializa `frente` e `tras` como NULL |
| `filaVazia` | Retorna 1 se a fila não tiver nenhum elemento, 0 caso contrário |
| `enfileirar` | Cria um novo nó com os dados recebidos e o insere no fim da fila pelo ponteiro `tras` |
| `desenfileirar` | Remove e retorna o nó da frente da fila pelo ponteiro `frente`, atualizando `tras` se necessário |

### Funções da Pilha

| Função | O que faz |
|---|---|
| `criarPilha` | Aloca memória para uma nova Pilha vazia e inicializa `topo` como NULL |
| `pilhaVazia` | Retorna 1 se a pilha não tiver nenhum elemento, 0 caso contrário |
| `empilhar` | Insere um nó existente no topo da pilha sem novo malloc, apenas reencadeando os ponteiros |
| `desempilhar` | Remove e retorna o nó do topo da pilha, avançando `topo` para o próximo nó |

### Funções de Persistência

| Função | O que faz |
|---|---|
| `carregarArquivo` | Abre `tarefas.txt`, lê cada linha no formato `ID;DESCRICAO` e reconstrói a fila, restaurando também o próximo ID |
| `salvarArquivo` | Percorre a fila do início ao fim e grava cada tarefa no arquivo `tarefas.txt`, sobrescrevendo o conteúdo anterior |

### Funções de Interface

| Função | O que faz |
|---|---|
| `exibirMenu` | Imprime o menu numerado de opções na tela |
| `limparBuffer` | Descarta caracteres residuais do stdin após leituras com scanf para evitar pulos de leitura |
| `adicionarTarefa` | Lê a descrição com fgets, gera o ID sequencial e chama enfileirar |
| `listarTarefas` | Percorre a fila do início ao fim exibindo cada tarefa, sem remover nenhum elemento |
| `concluirTarefa` | Chama desenfileirar para remover da fila e empilhar para registrar no histórico |
| `desfazerConclusao` | Chama desempilhar para remover do histórico e enfileirar para reinserir na fila |

### Funções de Limpeza de Memória

| Função | O que faz |
|---|---|
| `liberarFila` | Percorre todos os nós da fila liberando cada um com free, depois libera a estrutura da fila em si |
| `liberarPilha` | Percorre todos os nós da pilha liberando cada um com free, depois libera a estrutura da pilha em si |

---

## 💾 Formato do arquivo de persistência

O arquivo `tarefas.txt` armazena apenas as tarefas pendentes. O histórico de concluídas não é salvo. Cada linha representa uma tarefa no formato:

```
ID;DESCRICAO
```

Exemplo real:

```
1;Estudar para a prova de Estrutura de Dados
2;Entregar relatório do Projeto Extensionista
3;Revisar o código do projeto final
```

- O separador é o ponto e vírgula (`;`)
- O ID é um inteiro positivo gerado sequencialmente pelo programa
- Na leitura, o maior ID encontrado determina o próximo ID a ser gerado, evitando repetições entre execuções
- Se o arquivo não existir, o programa inicia normalmente com fila vazia e ID = 1
- O caractere `;` não deve ser usado na descrição de uma tarefa, pois corromperia o parsing do arquivo

---

## ⚠️ Limitações conhecidas

- **Descrição limitada a 255 caracteres** — textos maiores são truncados silenciosamente
- **Sem edição de tarefas** — não é possível alterar a descrição de uma tarefa já inserida; é necessário removê-la e adicioná-la novamente
- **Histórico não persistido** — tarefas concluídas (na pilha) são perdidas ao encerrar o programa; só as pendentes são salvas
- **Sem suporte a múltiplos usuários** — o arquivo `tarefas.txt` é compartilhado e não há controle de acesso concorrente
- **Interface apenas CLI** — não há interface gráfica ou web
- **Ponto e vírgula proibido na descrição** — usar `;` no texto da tarefa corromperia o parsing do arquivo na próxima execução

---

## 🚀 Como compilar e executar

```bash
gcc -std=c11 -Wall -Wextra -o tarefas gerenciador_tarefas.c
./tarefas
```

- `-std=c11` — garante que o código seja compilado no padrão C11
- `-Wall -Wextra` — ativa todos os avisos do compilador, ajudando a detectar problemas
- `-o tarefas` — define o nome do executável gerado

---

## 📦 Requisitos

- Compilador GCC ou Clang com suporte ao padrão C11
- Sistema operacional Linux, macOS ou Windows com terminal
- Nenhuma biblioteca externa — apenas a biblioteca padrão do C
````
