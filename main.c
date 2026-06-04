/*
 * SISTEMA DE GERENCIAMENTO DE TAREFAS
 * Linguagem: C puro (C11) | Estruturas: Fila Encadeada (FIFO) + Pilha Encadeada (LIFO)
 * Persistência: arquivo "tarefas.txt"
 */
 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
 
#define MAX_DESC  256           /* tamanho máximo da descrição */
#define ARQUIVO   "tarefas.txt"
 
/* ── ESTRUTURAS ─────────────────────────────────────────────────────────── */
 
/* Nó genérico: armazena uma tarefa e encadeia para o próximo nó */
typedef struct No {
    int   id;                  /* identificador único e sequencial */
    char  descricao[MAX_DESC]; /* texto da tarefa */
    struct No *proximo;        /* ponteiro para o próximo nó na cadeia */
} No;
 
/* Fila FIFO: dois ponteiros garantem inserção e remoção em O(1) */
typedef struct {
    No *frente; /* primeiro elemento — próximo a sair  */
    No *tras;   /* último elemento  — próximo a entrar */
} Fila;
 
/* Pilha LIFO: um ponteiro de topo garante push/pop em O(1) */
typedef struct {
    No *topo;   /* elemento mais recente — próximo a sair */
} Pilha;
 
 
/* ── FILA ───────────────────────────────────────────────────────────────── */
 
/* Aloca e inicializa uma Fila vazia. Encerra o programa se malloc falhar. */
Fila *criarFila(void) {
    Fila *f = (Fila *)malloc(sizeof(Fila));
    if (!f) { fprintf(stderr, "Erro: sem memória para criar a fila.\n"); exit(EXIT_FAILURE); }
    f->frente = NULL; /* sem elementos: frente e tras apontam para NULL */
    f->tras   = NULL;
    return f;
}
 
/* Retorna 1 se a fila estiver vazia, 0 caso contrário. */
int filaVazia(Fila *f) {
    return (f->frente == NULL);
}
 
/*
 * enfileirar — insere um novo nó no FIM da fila (política FIFO).
 * Passos: aloca nó → preenche dados → se vazia: frente=tras=novo;
 *         senão: tras->proximo=novo e tras=novo.
 */
void enfileirar(Fila *f, int id, const char *descricao) {
    No *novo = (No *)malloc(sizeof(No));
    if (!novo) { fprintf(stderr, "Erro: sem memória ao enfileirar.\n"); exit(EXIT_FAILURE); }
 
    novo->id = id;
    strncpy(novo->descricao, descricao, MAX_DESC - 1);
    novo->descricao[MAX_DESC - 1] = '\0'; /* garante terminador nulo */
    novo->proximo = NULL;                 /* novo nó é o último: sem sucessor */
 
    if (filaVazia(f)) {
        f->frente = novo;  /* fila vazia: único nó é frente E tras */
        f->tras   = novo;
    } else {
        f->tras->proximo = novo; /* antigo último aponta para o novo */
        f->tras = novo;          /* tras agora é o novo último */
    }
}
 
/*
 * desenfileirar — remove e retorna o nó da FRENTE (política FIFO).
 * O chamador é responsável por liberar o nó retornado com free().
 * Retorna NULL se a fila estiver vazia.
 */
No *desenfileirar(Fila *f) {
    if (filaVazia(f)) return NULL;
 
    No *removido  = f->frente;           /* 1. guarda referência ao nó da frente */
    f->frente     = f->frente->proximo;  /* 2. avança frente para o próximo nó */
    if (f->frente == NULL) f->tras = NULL; /* 3. fila ficou vazia: tras também é NULL */
    removido->proximo = NULL;            /* 4. desconecta o nó da cadeia */
    return removido;
}
 
 
/* ── PILHA ──────────────────────────────────────────────────────────────── */
 
/* Aloca e inicializa uma Pilha vazia. Encerra o programa se malloc falhar. */
Pilha *criarPilha(void) {
    Pilha *p = (Pilha *)malloc(sizeof(Pilha));
    if (!p) { fprintf(stderr, "Erro: sem memória para criar a pilha.\n"); exit(EXIT_FAILURE); }
    p->topo = NULL; /* pilha vazia */
    return p;
}
 
/* Retorna 1 se a pilha estiver vazia, 0 caso contrário. */
int pilhaVazia(Pilha *p) {
    return (p->topo == NULL);
}
 
/*
 * empilhar — insere um nó existente no TOPO (política LIFO).
 * Reutiliza o próprio nó já alocado; não chama malloc.
 * Passos: no->proximo = topo atual → topo = no.
 */
void empilhar(Pilha *p, No *no) {
    no->proximo = p->topo; /* novo topo aponta para o antigo topo */
    p->topo = no;          /* atualiza topo para o novo nó */
}
 
/*
 * desempilhar — remove e retorna o nó do TOPO (política LIFO).
 * Retorna NULL se a pilha estiver vazia.
 */
No *desempilhar(Pilha *p) {
    if (pilhaVazia(p)) return NULL;
 
    No *removido = p->topo;          /* 1. guarda referência ao topo */
    p->topo = p->topo->proximo;      /* 2. topo passa a ser o próximo */
    removido->proximo = NULL;        /* 3. desconecta o nó da pilha */
    return removido;
}
 
 
/* ── PERSISTÊNCIA ───────────────────────────────────────────────────────── */
 
/*
 * carregarArquivo — lê "tarefas.txt" e recarrega a fila.
 * Formato de cada linha: ID;DESCRICAO  (ex: 3;Estudar para a prova)
 * Se o arquivo não existir (1ª execução), inicia normalmente com proxId=1.
 */
void carregarArquivo(Fila *f, int *proxId) {
    FILE *arq = fopen(ARQUIVO, "r");
    if (!arq) { *proxId = 1; return; } /* arquivo ausente — situação normal */
 
    int  id, maiorId = 0;
    char descricao[MAX_DESC];
 
    /* %d lê o inteiro, %*c descarta o ';', %255[^\n] lê até o fim da linha */
    while (fscanf(arq, "%d%*c%255[^\n]", &id, descricao) == 2) {
        enfileirar(f, id, descricao);
        if (id > maiorId) maiorId = id; /* rastreia o maior ID já usado */
    }
 
    fclose(arq);
    *proxId = (maiorId + 1 < 1) ? 1 : maiorId + 1; /* proteção contra arquivo corrompido */
}
 
/*
 * salvarArquivo — percorre a fila e grava cada tarefa em "tarefas.txt".
 * Sobrescreve o arquivo anterior. O histórico (pilha) NÃO é salvo.
 */
void salvarArquivo(Fila *f) {
    FILE *arq = fopen(ARQUIVO, "w"); /* "w" sobrescreve o conteúdo anterior */
    if (!arq) { fprintf(stderr, "Aviso: não foi possível salvar '%s'.\n", ARQUIVO); return; }
 
    for (No *atual = f->frente; atual != NULL; atual = atual->proximo)
        fprintf(arq, "%d;%s\n", atual->id, atual->descricao); /* percorre sem remover */
 
    fclose(arq);
    printf("Tarefas salvas em '%s'.\n", ARQUIVO);
}
 
 
/* ── INTERFACE / NEGÓCIO ────────────────────────────────────────────────── */
 
/* Imprime o menu numerado. */
void exibirMenu(void) {
    printf("\n╔══════════════════════════════════════╗\n");
    printf("║   GERENCIADOR DE TAREFAS             ║\n");
    printf("╠══════════════════════════════════════╣\n");
    printf("║  1. Adicionar tarefa                 ║\n");
    printf("║  2. Listar tarefas pendentes         ║\n");
    printf("║  3. Concluir próxima tarefa          ║\n");
    printf("║  4. Desfazer última conclusão (UNDO) ║\n");
    printf("║  5. Salvar e sair                    ║\n");
    printf("╚══════════════════════════════════════╝\n");
    printf("Escolha: ");
}
 
/* Descarta caracteres residuais no stdin até '\n' ou EOF.
   Chamada após scanf para evitar que fgets leia linha vazia. */
void limparBuffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
 
/* Opção 1: lê a descrição com fgets, gera ID sequencial e enfileira no fim. */
void adicionarTarefa(Fila *f, int *proxId) {
    char descricao[MAX_DESC];
    printf("Descrição da tarefa: ");
    if (fgets(descricao, MAX_DESC, stdin) == NULL) { printf("Erro ao ler.\n"); return; }
 
    size_t len = strlen(descricao);
    if (len > 0 && descricao[len - 1] == '\n') descricao[len - 1] = '\0'; /* remove '\n' do fgets */
    if (strlen(descricao) == 0) { printf("Descrição vazia. Tarefa não adicionada.\n"); return; }
 
    enfileirar(f, *proxId, descricao);
    printf("Tarefa #%d adicionada com sucesso!\n", (*proxId)++); /* pós-incremento: exibe e depois incrementa */
}
 
/* Opção 2: percorre a fila exibindo cada nó (apenas leitura, sem remoção). */
void listarTarefas(Fila *f) {
    if (filaVazia(f)) { printf("Nenhuma tarefa pendente.\n"); return; }
 
    printf("\n── Tarefas Pendentes ──────────────────\n");
    int pos = 1;
    for (No *atual = f->frente; atual != NULL; atual = atual->proximo) /* percorre via ponteiro proximo */
        printf("  %d. [ID %d] %s\n", pos++, atual->id, atual->descricao);
    printf("───────────────────────────────────────\n");
}
 
/* Opção 3: desenfileira da frente (FIFO) e empilha no histórico (LIFO). O nó migra de estrutura. */
void concluirTarefa(Fila *f, Pilha *historico) {
    if (filaVazia(f)) { printf("Não há tarefas pendentes para concluir.\n"); return; }
 
    No *concluida = desenfileirar(f);              /* remove da frente — O(1) */
    printf("Tarefa concluída: [ID %d] %s\n", concluida->id, concluida->descricao);
    empilhar(historico, concluida);                /* empilha no histórico — O(1) */
}
 
/*
 * Opção 4 (UNDO): desempilha do topo (LIFO) e reinsere no fim da fila.
 * enfileirar() faz malloc interno e copia os dados, então o nó antigo
 * deve ser liberado com free() para evitar memory leak.
 */
void desfazerConclusao(Fila *f, Pilha *historico) {
    if (pilhaVazia(historico)) { printf("Nenhuma tarefa concluída para desfazer.\n"); return; }
 
    No *restaurada = desempilhar(historico);       /* remove do topo — O(1) */
    printf("Conclusão desfeita: [ID %d] %s voltou para a fila.\n", restaurada->id, restaurada->descricao);
    enfileirar(f, restaurada->id, restaurada->descricao); /* copia dados para novo nó no fim da fila */
    free(restaurada);                              /* libera nó antigo (conteúdo já copiado acima) */
}
 
 
/* ── LIMPEZA DE MEMÓRIA ─────────────────────────────────────────────────── */
 
/* Libera todos os nós da fila e a estrutura em si. Evita memory leak ao encerrar. */
void liberarFila(Fila *f) {
    No *atual = f->frente;
    while (atual != NULL) {
        No *prox = atual->proximo; /* salva próximo ANTES de liberar o atual */
        free(atual);
        atual = prox;
    }
    free(f);
}
 
/* Libera todos os nós da pilha e a estrutura em si. */
void liberarPilha(Pilha *p) {
    No *atual = p->topo;
    while (atual != NULL) {
        No *prox = atual->proximo; /* salva próximo ANTES de liberar o atual */
        free(atual);
        atual = prox;
    }
    free(p);
}
 
 
/* ── FUNÇÃO PRINCIPAL ───────────────────────────────────────────────────── */
int main(void) {
    Fila  *fila      = criarFila();   /* fila de tarefas pendentes */
    Pilha *historico = criarPilha();  /* pilha de tarefas concluídas (UNDO) */
    int    proxId    = 1;             /* próximo ID sequencial disponível */
 
    carregarArquivo(fila, &proxId);   /* restaura estado da execução anterior */
 
    int opcao;
    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1) opcao = -1; /* leitura inválida: força opção inválida */
        limparBuffer();                            /* descarta '\n' e lixo residual do stdin */
 
        switch (opcao) {
            case 1: adicionarTarefa(fila, &proxId);       break;
            case 2: listarTarefas(fila);                   break;
            case 3: concluirTarefa(fila, historico);       break;
            case 4: desfazerConclusao(fila, historico);    break;
            case 5: salvarArquivo(fila); printf("Até logo!\n"); break;
            default: printf("Opção inválida. Tente novamente.\n"); break;
        }
    } while (opcao != 5);
 
    liberarFila(fila);      /* libera todos os nós pendentes + estrutura */
    liberarPilha(historico); /* libera todos os nós do histórico + estrutura */
 
    return EXIT_SUCCESS;
}