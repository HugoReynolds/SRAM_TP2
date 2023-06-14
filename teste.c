#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define PI 3.14159265359

// Definição das estruturas de dados

typedef struct {
    int id; // Identificador da fonte
    int F;  // Frequência de amostragem
    int N;  // Número de amostras
    int M;  // Número de clientes subscritos
} FonteInformacao;

typedef struct {
    int id; // Identificador do cliente
} Cliente;

typedef struct {
    int id;    // Identificador da fonte
    int* data; // Dados da fonte
} PDU1;

typedef struct {
    int id;     // Identificador da fonte
    int* data;  // Dados da fonte
    int length; // Tamanho dos dados
} PDU2;

typedef struct {
    int* data;  // Dados recebidos
    int length; // Tamanho dos dados
} BufferEntrada;

// Variáveis globais

FonteInformacao* fontes;      // Lista de fontes de informação
int num_fontes;               // Número de fontes de informação
Cliente* clientes;            // Lista de clientes
int num_clientes;             // Número de clientes
BufferEntrada buffer_entrada; // Buffer de entrada
pthread_mutex_t mutex;        // Mutex para sincronização de acesso ao buffer

// Função para gerar os dados da fonte de informação

void gerarDados(FonteInformacao* fonte) {
    int i;
    double x;
    fonte -> data = (int*)malloc(sizeof(int) * fonte->N);
    
    for (i = 0; i < fonte->N; i++) {
        x = 1 + (1 + sin(2 * PI * i / fonte->N)) * 30;
        fonte->data[i] = (int)x;
    }
}

// Função para enviar PDU-2 aos clientes subscritos

void enviarPDUs(FonteInformacao* fonte) {
    int i;
    PDU2 pdu;

    pdu.id = fonte->id;
    pdu.data = fonte->data;
    pdu.length = fonte->N;

    for (i = 0; i < num_clientes; i++) {
        // Verifica se o cliente está subscrito ao canal
        if (clienteSubscrito(&clientes[i], fonte->id)) {
            // Envia PDU-2 para o cliente
            enviarPDU2(&clientes[i], &pdu);
        }
    }
}

// Função para processar o pedido "list"

void processarPedidoList() {
    int i;

    printf("Lista de canais disponíveis:\n");
    for (i = 0; i < num_fontes; i++) {
        printf("%d\n", fontes[i].id);
    }
}

// Função para processar o pedido "info"

void processarPedidoInfo(int id) {
    int i;

    for (i = 0; i < num_fontes; i++) {
        if (fontes[i].id == id) {
            printf("Informações sobre o canal %d:\n", id);
            printf("Frequência de amostragem: %d\n", fontes[i].F);
            printf("Número de amostras: %d\n", fontes[i].N);
            printf("Número de clientes subscritos: %d\n", fontes[i].M);
            return;
        }
    }

    printf("Canal %d não encontrado\n", id);
}

// Função para processar o pedido "play"

void processarPedidoPlay(int id, Cliente* cliente) {
    int i;

    for (i = 0; i < num_fontes; i++) {
        if (fontes[i].id == id) {
            // Verifica se o cliente já está subscrito ao canal
            if (clienteSubscrito(cliente, id)) {
                printf("Cliente já subscrito ao canal %d\n", id);
            } else {
                // Adiciona o cliente à lista de subscritores
                fontes[i].M++;
                fontes[i].clientes = (Cliente*)realloc(fontes[i].clientes, sizeof(Cliente) * fontes[i].M);
                fontes[i].clientes[fontes[i].M - 1] = *cliente;
                printf("Cliente subscrito ao canal %d\n", id);
            }
            return;
        }
    }

    printf("Canal %d não encontrado\n", id);
}

// Função para processar o pedido "stop"

void processarPedidoStop(int id, Cliente* cliente) {
    int i, j;

    for (i = 0; i < num_fontes; i++) {
        if (fontes[i].id == id) {
            // Verifica se o cliente está subscrito ao canal
            if (clienteSubscrito(cliente, id)) {
                // Remove o cliente da lista de subscritores
                for (j = 0; j < fontes[i].M; j++) {
                    if (fontes[i].clientes[j].id == cliente->id) {
                        if (j < fontes[i].M - 1) {
                            memmove(&fontes[i].clientes[j], &fontes[i].clientes[j + 1], sizeof(Cliente) * (fontes[i].M - j - 1));
                        }
                        fontes[i].M--;
                        fontes[i].clientes = (Cliente*)realloc(fontes[i].clientes, sizeof(Cliente) * fontes[i].M);
                        printf("Cliente removido do canal %d\n", id);
                        return;
                    }
                }
            } else {
                printf("Cliente não subscrito ao canal %d\n", id);
            }
            return;
        }
    }

    printf("Canal %d não encontrado\n", id);
}

// Função para verificar se um cliente está subscrito a um canal

int clienteSubscrito(Cliente* cliente, int id_canal) {
    int i;

    for (i = 0; i < num_fontes; i++) {
        if (fontes[i].id == id_canal) {
            int j;
            for (j = 0; j < fontes[i].M; j++) {
                if (fontes[i].clientes[j].id == cliente->id) {
                    return 1;
                }
            }
            return 0;
        }
    }

    return 0;
}

// Função para enviar PDU-2 para um cliente

void enviarPDU2(Cliente* cliente, PDU2* pdu) {
    // Simulação do envio do PDU-2 para o cliente
    printf("Enviando PDU-2 para o cliente %d\n", cliente->id);
}

// Função para receber PDU-2 do tipo data

void receberPDU2Data(PDU2* pdu) {
    // Simulação do recebimento do PDU-2 do tipo data
    pthread_mutex_lock(&mutex);

    buffer_entrada.data = pdu->data;
    buffer_entrada.length = pdu->length;

    pthread_mutex_unlock(&mutex);
}

// Função para processar o buffer de entrada e exibir os valores

void processarBufferEntrada() {
    // Simulação do processamento do buffer de entrada
    pthread_mutex_lock(&mutex);

    int i, j;
    for (i = 0; i < buffer_entrada.length; i++) {
        for (j = 0; j < buffer_entrada.data[i]; j++) {
            printf("#");
        }
        printf("\n");
    }

    pthread_mutex_unlock(&mutex);
}

// Função para o thread de processamento do buffer de entrada

void* threadProcessamentoBuffer(void* arg) {
    while (1) {
        processarBufferEntrada();
        usleep(1000000); // Aguarda 1 segundo
    }
    return NULL;
}

// Função principal

int main() {
    // Inicialização das variáveis globais
    fontes = NULL;
    num_fontes = 0;
    clientes = NULL;
    num_clientes = 0;
    buffer_entrada.data = NULL;
    buffer_entrada.length = 0;

    // Criação do thread para o processamento do buffer de entrada
    pthread_t thread_buffer;
    pthread_create(&thread_buffer, NULL, threadProcessamentoBuffer, NULL);

    // Exemplo de utilização do programa

    // Criar algumas fontes de informação
    FonteInformacao fonte1;
    fonte1.id = 1;
    fonte1.F = 10;
    fonte1.N = 100;
    fonte1.M = 0;
    gerarDados(&fonte1);
    num_fontes++;
    fontes = (FonteInformacao*)realloc(fontes, sizeof(FonteInformacao) * num_fontes);
    fontes[num_fontes - 1] = fonte1;

    FonteInformacao fonte2;
    fonte2.id = 2;
    fonte2.F = 5;
    fonte2.N = 50;
    fonte2.M = 0;
    gerarDados(&fonte2);
    num_fontes++;
    fontes = (FonteInformacao*)realloc(fontes, sizeof(FonteInformacao) * num_fontes);
    fontes[num_fontes - 1] = fonte2;

    // Criar alguns clientes
    Cliente cliente1;
    cliente1.id = 1;
    num_clientes++;
    clientes = (Cliente*)realloc(clientes, sizeof(Cliente) * num_clientes);
    clientes[num_clientes - 1] = cliente1;

    Cliente cliente2;
    cliente2.id = 2;
    num_clientes++;
    clientes = (Cliente*)realloc(clientes, sizeof(Cliente) * num_clientes);
    clientes[num_clientes - 1] = cliente2;

    // Enviar PDUs para os clientes subscritos
    enviarPDUs(&fonte1);
    enviarPDUs(&fonte2);

    // Processar alguns pedidos dos clientes
    processarPedidoList();
    processarPedidoInfo(1);
    processarPedidoInfo(3);
    processarPedidoPlay(1, &cliente1);
    processarPedidoPlay(2, &cliente1);
    processarPedidoPlay(1, &cliente2);
    processarPedidoStop(1, &cliente2);
    processarPedidoStop(3, &cliente1);

    // Aguardar o término do thread de processamento do buffer de entrada
    pthread_join(thread_buffer, NULL);

    // Liberação de memória
    int i;
    for (i = 0; i < num_fontes; i++) {
        free(fontes[i].data);
        free(fontes[i].clientes);
    }
    free(fontes);
    free(clientes);
    free(buffer_entrada.data);

    return 0;
}
