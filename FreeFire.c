#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

// Código da Ilha – Edição Free Fire
// Nível: Mestre
// Este programa simula o gerenciamento avançado de uma mochila com componentes coletados durante a fuga de uma ilha.
// Ele introduz ordenação com critérios e busca binária para otimizar a gestão dos recursos.

// ------------------------------------------------
// Definições de Estruturas e Constantes
// ------------------------------------------------

#define MAX_SLOTS 10

// Enum CriterioOrdenacao:
// Define os critérios possíveis para a ordenação dos itens (nome, tipo ou prioridade).
typedef enum {
    CRITERIO_NOME,
    CRITERIO_TIPO,
    CRITERIO_PRIORIDADE
} CriterioOrdenacao;

// Struct Item:
// Representa um componente com nome, tipo, quantidade e prioridade (1 a 5).
// A prioridade indica a importância do item na montagem do plano de fuga.
typedef struct {
    char nome[30];
    char tipo[20];
    int quantidade;
    int prioridade; // 1 (mais alta) a 5 (mais baixa)
} Item;

// ------------------------------------------------
// Variáveis Globais (Vetor mochila e Controles)
// ------------------------------------------------

// Vetor mochila:
// Armazena até 10 itens coletados.
Item mochila[MAX_SLOTS];

// Variáveis de controle:
int numItens = 0; // quantidade atual
long long comparacoes = 0; // análise de desempenho (última ordenação)
bool ordenadaPorNome = false; // para controle da busca binária

// ------------------------------------------------
// Protótipos das Funções
// ------------------------------------------------

void limparBuffer();
void limparTela();
void exibirMenu();
void listarItens();

void inserirItem();
void removerItem();

void insertionSort(CriterioOrdenacao criterio);
void menuDeOrdenacao();

int buscarItemSequencial(const char *nome); // Auxiliar para remoção
void buscaBinariaPorNome();

// ------------------------------------------------
// Implementação das Funções
// ------------------------------------------------

// limparTela():
// Simula a limpeza da tela imprimindo várias linhas em branco.
void limparTela() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

// limparBuffer():
// Limpa o buffer de entrada do teclado.
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// exibirMenu():
// Apresenta o menu principal ao jogador, com destaque para status da ordenação.
void exibirMenu() {
    printf("\n================================================================\n");
    printf("MOCHILA DE LOOT - NÍVEL MESTRE (Status: %s)\n", ordenadaPorNome ? "Ordenada por Nome" : "Desordenada");
    printf("----------------------------------------------------------------\n");
    printf("1. Adicionar um item\n");
    printf("2. Remover um item\n");
    printf("3. Listar todos os itens\n");
    printf("4. Ordenar os itens por critério (Nome/Tipo/Prioridade)\n");
    printf("5. Realizar busca binária por nome\n");
    printf("0. Sair\n");
    printf("Escolha: ");
}

// inserirItem():
// Adiciona um novo componente à mochila se houver espaço.
// Solicita nome, tipo, quantidade e prioridade.
// Após inserir, marca a mochila como "não ordenada por nome".
void inserirItem() {
    if (numItens >= MAX_SLOTS) {
        printf("\n[ALERTA] Mochila cheia! (%d/%d slots)\n", numItens, MAX_SLOTS);
        return;
    }

    Item novo;
    printf("\n--- Adicionar Item ---\n");

    printf("Nome: ");
    limparBuffer();
    if (fgets(novo.nome, sizeof(novo.nome), stdin) == NULL) return;
    novo.nome[strcspn(novo.nome, "\n")] = 0;

    printf("Tipo (ex: Arma, Cura): ");
    if (scanf("%19s", novo.tipo) != 1) { limparBuffer(); return; }

    printf("Quantidade: ");
    if (scanf("%d", &novo.quantidade) != 1 || novo.quantidade <= 0) {
        printf("[ERRO] Quantidade inválida.\n"); limparBuffer(); return;
    }

    printf("Prioridade (1 = Alta, 5 = Baixa): ");
    if (scanf("%d", &novo.prioridade) != 1 || novo.prioridade < 1 || novo.prioridade > 5) {
        printf("[ERRO] Prioridade deve ser entre 1 e 5. Item não adicionado.\n"); limparBuffer(); return;
    }

    mochila[numItens] = novo;
    numItens++;
    
    // Marca a mochila como "não ordenada por nome" após inserção
    ordenadaPorNome = false; 
    comparacoes = 0;

    printf("\n[SUCESSO] '%s' adicionado.\n", novo.nome);
    listarItens();
}

// buscarItemSequencial():
// Reutilizado para encontrar o índice do item a ser removido (a lista pode estar desordenada).
int buscarItemSequencial(const char *nome) {
    for (int i = 0; i < numItens; i++) {
        if (strcmp(mochila[i].nome, nome) == 0) {
            return i;
        }
    }
    return -1;
}

// removerItem():
// Permite remover um componente da mochila pelo nome.
// Se encontrado, reorganiza o vetor para preencher a lacuna.
void removerItem() {
    char nomeParaRemover[30];
    int indice;

    if (numItens == 0) {
        printf("\nA mochila está vazia. Nada para remover.\n");
        return;
    }

    printf("\n--- Remover Item ---\n");
    printf("Digite o NOME exato do item para remoção: ");
    limparBuffer();
    if (fgets(nomeParaRemover, sizeof(nomeParaRemover), stdin) == NULL) return;
    nomeParaRemover[strcspn(nomeParaRemover, "\n")] = 0;

    indice = buscarItemSequencial(nomeParaRemover);

    if (indice == -1) {
        printf("\n[ALERTA] Item '%s' não encontrado.\n", nomeParaRemover);
        return;
    }

    // Reorganiza o vetor (deslocamento)
    for (int i = indice; i < numItens - 1; i++) {
        mochila[i] = mochila[i + 1];
    }

    numItens--;
    // A remoção desorganiza o vetor
    ordenadaPorNome = false;
    comparacoes = 0;

    printf("\n[SUCESSO] Item '%s' removido.\n", nomeParaRemover);
    listarItens();
}

// listarItens():
// Exibe uma tabela formatada com todos os componentes presentes na mochila.
void listarItens() {
    printf("\n--- Conteúdo da Mochila ---\n");
    printf("Status de Ordenação: %s\n", ordenadaPorNome ? "Por Nome" : "Desordenada");
    printf("Comparações da Última Ordenação: %lld\n", comparacoes);
    printf("------------------------------------------------------------------\n");

    if (numItens == 0) {
        printf("A mochila está vazia.\n");
        printf("------------------------------------------------------------------\n");
        return;
    }

    printf("Pos | Nome                      | Tipo            | Qtd | Prioridade\n");
    printf("------------------------------------------------------------------\n");
    for (int i = 0; i < numItens; i++) {
        Item item = mochila[i];
        printf("%02d  | %-25s | %-15s | %-3d | %d\n",
               i + 1, item.nome, item.tipo, item.quantidade, item.prioridade);
    }
    printf("------------------------------------------------------------------\n");
}

// insertionSort():
// Implementação do algoritmo de ordenação por inserção.
// Funciona com diferentes critérios de ordenação e conta comparações.
void insertionSort(CriterioOrdenacao criterio) {
    long long count = 0; 
    int i, j;
    Item chave;

    if (numItens <= 1) {
        printf("\n[ALERTA] Apenas %d item(s). Ordenação desnecessária.\n", numItens);
        comparacoes = 0;
        return;
    }

    for (i = 1; i < numItens; i++) {
        chave = mochila[i];
        j = i - 1;

        while (j >= 0) {
            bool deveTrocar = false;
            count++; // Contador de comparações

            switch (criterio) {
                case CRITERIO_NOME:
                    if (strcmp(mochila[j].nome, chave.nome) > 0) {
                        deveTrocar = true;
                    }
                    break;
                case CRITERIO_TIPO:
                    if (strcmp(mochila[j].tipo, chave.tipo) > 0) {
                        deveTrocar = true;
                    }
                    break;
                case CRITERIO_PRIORIDADE:
                    // 1 (mais alta) vem antes de 5 (mais baixa)
                    if (mochila[j].prioridade > chave.prioridade) {
                        deveTrocar = true;
                    }
                    break;
            }

            if (deveTrocar) {
                mochila[j + 1] = mochila[j];
                j = j - 1;
            } else {
                break;
            }
        }
        mochila[j + 1] = chave;
    }
    
    // Atualiza as variáveis de controle
    comparacoes = count;
    ordenadaPorNome = (criterio == CRITERIO_NOME);

    printf("\n[SUCESSO] Mochila ordenada por %s!\n", 
           criterio == CRITERIO_NOME ? "Nome" : (criterio == CRITERIO_TIPO ? "Tipo" : "Prioridade"));
    listarItens();
}

// menuDeOrdenacao():
// Permite ao jogador escolher como deseja ordenar os itens.
// Utiliza a função insertionSort() com o critério selecionado.
// Exibe a quantidade de comparações feitas (análise de desempenho).
void menuDeOrdenacao() {
    int escolha;
    printf("\n--- Critério de Ordenação ---\n");
    printf("1. Nome do Item (Prepara para Busca Binária)\n");
    printf("2. Tipo do Item\n");
    printf("3. Prioridade (1 a 5)\n");
    printf("Escolha: ");

    if (scanf("%d", &escolha) != 1) {
        printf("[ERRO] Opção inválida.\n");
        limparBuffer();
        return;
    }

    switch (escolha) {
        case 1: insertionSort(CRITERIO_NOME); break;
        case 2: insertionSort(CRITERIO_TIPO); break;
        case 3: insertionSort(CRITERIO_PRIORIDADE); break;
        default: printf("[ALERTA] Critério desconhecido.\n");
    }
}

// buscaBinariaPorNome():
// Realiza busca binária por nome, desde que a mochila esteja ordenada por nome.
// Se encontrar, exibe os dados do item buscado.
// Caso contrário, informa que não encontrou o item.
void buscaBinariaPorNome() {
    char nomeParaBuscar[30];
    int indice = -1;

    if (numItens == 0) {
        printf("\nA mochila está vazia.\n");
        return;
    }

    printf("\n--- Busca Binária por Nome ---\n");

    // Validação de Pré-requisito
    if (!ordenadaPorNome) {
        printf("[ERRO CRÍTICO] A busca binária exige que a mochila esteja ordenada por NOME.\n");
        printf("Por favor, use a opção 4 para ordenar primeiro.\n");
        return;
    }

    printf("Qual item você está procurando? ");
    limparBuffer();
    if (fgets(nomeParaBuscar, sizeof(nomeParaBuscar), stdin) == NULL) return;
    nomeParaBuscar[strcspn(nomeParaBuscar, "\n")] = 0;

    // Lógica da Busca Binária
    int inicio = 0;
    int fim = numItens - 1;

    while (inicio <= fim) {
        int meio = inicio + (fim - inicio) / 2;
        int resultadoComparacao = strcmp(mochila[meio].nome, nomeParaBuscar);

        if (resultadoComparacao == 0) {
            indice = meio; // Item encontrado!
            break;
        } else if (resultadoComparacao < 0) {
            inicio = meio + 1; // Item está na metade superior
        } else {
            fim = meio - 1; // Item está na metade inferior
        }
    }

    // Saída
    if (indice != -1) {
        Item itemEncontrado = mochila[indice];
        printf("\n[ITEM ENCONTRADO! - POSIÇÃO %d]\n", indice + 1);
        printf("Nome: %s\n", itemEncontrado.nome);
        printf("Tipo: %s\n", itemEncontrado.tipo);
        printf("Quantidade: %d\n", itemEncontrado.quantidade);
        printf("Prioridade: %d\n", itemEncontrado.prioridade);
    } else {
        printf("\n[NÃO ENCONTRADO] O item '%s' não está na mochila.\n", nomeParaBuscar);
    }
}


// ------------------------------------------------
// Função Principal
// ------------------------------------------------

int main() {
    int escolha;
    
    // Configuração inicial (opcional: limpar tela)
    limparTela();
    printf("**************************************************\n");
    printf("* Código da Ilha – Edição Free Fire (Nível Mestre) *\n");
    printf("**************************************************\n");

    // Loop principal
    do {
        exibirMenu();
        
        // Leitura da opção
        if (scanf("%d", &escolha) != 1) {
            printf("\n[ERRO] Opção inválida. Digite um número.\n");
            escolha = -1;
            limparBuffer();
        }

        // A estrutura switch trata cada opção chamando a função correspondente.
        switch (escolha) {
            case 1: inserirItem(); break;
            case 2: removerItem(); break;
            case 3: listarItens(); break;
            case 4: menuDeOrdenacao(); break;
            case 5: buscaBinariaPorNome(); break;
            case 0: printf("\nMissão concluída! Fechando o sistema.\n"); break;
            default: printf("\nOpção desconhecida. Tente novamente.\n");
        }
    } while (escolha != 0);

    return 0;
}