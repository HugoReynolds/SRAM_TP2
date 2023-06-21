#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SERVER_IP "127.0.0.1"
#define CLIENT_PORT 12345
#define BUFFER_SIZE 1024
#define MAX_FONTS 100

typedef struct{
    char D[100];  // Identificador da fonte // Máximo??
}FonteInformacao;

typedef struct {
    FonteInformacao fonts[MAX_FONTS];
    int count;
} FonteInformacaoList;

void printFonteInformacaoList(FonteInformacaoList* list) {
    printf("Fontes disponíveis:\n");
    for (int i = 0; i < list->count; i++) {
        printf("i: %d ", i);
        printf("- %s\n", list->fonts[i].D);
    }
}



int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    FonteInformacaoList fonte_informacao_list;

    // Criação do socket UDP para o cliente
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro na criação do socket do cliente");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(CLIENT_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Erro na configuração do endereço do servidor");
        exit(1);
    }

    // Envio de uma mensagem vazia para o servidor para solicitar a lista de fontes de informação
    ssize_t numBytes = sendto(sockfd, "", 0, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (numBytes < 0) {
        perror("Erro no envio da mensagem para o servidor");
        exit(1);
    }

    // Recebimento da lista de fontes de informação
    memset(buffer, 0, sizeof(buffer));
    socklen_t addrLen = sizeof(serverAddr);
    numBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddr, &addrLen);

    if (numBytes < 0) {
        perror("Erro na recepção da lista de fontes de informação");
        exit(1);
    }

    // Conversão da lista de fontes de informação para a estrutura FonteInformacaoList
    int n_fontes = sscanf(buffer, "%d", &fonte_informacao_list.count);

    if (n_fontes < 0) {
        perror("Erro ao analisar a lista de fontes de informação");
        exit(1);
    }

    printf("Numero de fontes %d\n", fonte_informacao_list.count);

    for (int i = 0; i < fonte_informacao_list.count; i++) {
        memset(buffer, 0, sizeof(buffer));
        numBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddr, &addrLen);
        if (numBytes < 0) {
            perror("Erro na recepção da lista de fontes de informação");
            exit(1);
        }

    int fontes_id = sscanf(buffer, "%[^\n]", fonte_informacao_list.fonts[i].D);

    if (fontes_id != 1) {
        printf("Erro ao analisar a lista de fontes de informação\n");
        exit(1);
    }
}

    // Imprimir a lista de fontes de informação
    printFonteInformacaoList(&fonte_informacao_list);

    // Continuar recebendo informações adicionais do servidor
    while (1) {
    memset(buffer, 0, sizeof(buffer));
    numBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr *)&serverAddr, &addrLen);
    if (numBytes < 0) {
        perror("Erro na recepção de informações adicionais do servidor");
        exit(1);
    } else if (numBytes == 0) {
        // Não há mais dados a serem recebidos
        break;
    }

    // Processar as informações adicionais recebidas
    // Aqui você pode adicionar o código necessário para processar as informações recebidas
    //printf("Informações adicionais recebidas: %s\n", buffer);

     // Atualizar a lista de fontes disponíveis com a nova informação recebida
    int novaFonte = 1;
    for (int i = 0; i < fonte_informacao_list.count; i++) {
        if (strcmp(fonte_informacao_list.fonts[i].D, buffer) == 0) {
            novaFonte = 0;
            break;
        }
    }

    // Adicionar a nova informação à lista, apenas se for uma fonte nova
    if (novaFonte) {
        if (fonte_informacao_list.count < MAX_FONTS) {
            strncpy(fonte_informacao_list.fonts[fonte_informacao_list.count].D, buffer, sizeof(fonte_informacao_list.fonts[fonte_informacao_list.count].D));
            fonte_informacao_list.count++;

            // Imprimir a lista de fontes de informação atualizada
            printFonteInformacaoList(&fonte_informacao_list);
        } else {
            printf("Número máximo de fontes atingido. Não é possível adicionar mais fontes.\n");
        }
    }   
    }


    // Fechar o socket
    close(sockfd);

    return 0;
}
