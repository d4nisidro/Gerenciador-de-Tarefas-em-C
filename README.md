1. Descrição do problema
Em ambientes acadêmicos e profissionais, é comum acumular tarefas sem um controle claro de ordem e prioridade. Este sistema resolve esse problema oferecendo um gerenciador de tarefas via linha de comando que garante o processamento ordenado das atividades (a mais antiga sempre é concluída primeiro), permite desfazer conclusões feitas por engano e persiste os dados entre execuções, sem depender de banco de dados ou bibliotecas externas.

2. Estruturas de dados utilizadas
Fila Encadeada (FIFO) — tarefas pendentes
Escolhida porque reflete naturalmente a regra de negócio: a primeira tarefa adicionada deve ser a primeira a ser concluída. A implementação usa dois ponteiros (frente e tras), o que garante inserção no fim e remoção no início ambas em O(1), sem precisar percorrer a lista.
Pilha Encadeada (LIFO) — histórico de concluídas
Escolhida para implementar o mecanismo de desfazer (UNDO). A última tarefa concluída é sempre a primeira a ser restaurada, o que é exatamente o comportamento de uma pilha. Inserção e remoção pelo topo também são O(1).
Ambas as estruturas usam nós alocados dinamicamente (malloc/free), sem arrays de tamanho fixo, o que elimina o risco de estouro por limite pré-definido de tarefas.

3. Formato do arquivo de persistência
O arquivo tarefas.txt armazena apenas as tarefas pendentes (o histórico de concluídas não é salvo). Cada linha representa uma tarefa no formato:
ID;DESCRICAO
Exemplo real:
1;Estudar para a prova de Cálculo
2;Entregar relatório do Projeto Extensionista
3;Revisar o código do projeto final

O separador é o ponto e vírgula (;).
O ID é um inteiro positivo gerado sequencialmente pelo programa.
Na leitura, o maior ID encontrado determina o próximo ID a ser gerado, evitando repetições entre execuções.
Se o arquivo não existir (primeira execução), o programa inicia normalmente com a fila vazia e ID inicial = 1.


4. Limitações conhecidas

Descrição limitada a 255 caracteres — textos maiores são truncados silenciosamente.
Sem edição de tarefas — não é possível alterar a descrição de uma tarefa já inserida; é necessário removê-la e adicioná-la novamente.
Histórico não persistido — tarefas concluídas (na pilha) são perdidas ao encerrar o programa; só as pendentes são salvas.
Sem suporte a múltiplos usuários — o arquivo tarefas.txt é compartilhado e não há controle de acesso concorrente.
Interface apenas CLI — não há interface gráfica ou web.
Ponto e vírgula proibido na descrição — usar ; no texto da tarefa corromperia o parsing do arquivo na próxima execução.
