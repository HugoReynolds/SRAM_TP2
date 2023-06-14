#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 12345
#define BUFFER_SIZE 1024

struct FonteInformacao{
    int D;  // Identificador da fonte
    int i;  // ??
    int Vi; // Informação
    int P;  // Período 
    int F;  // Frequência
    int N;  // Número de amostras
    int M;  // Máximo??
};

int main() {
    int sockfd;
    struct sockaddr_in serverAddr, clientAddr;
    int msgSize;
    unsigned int cliAddrLen;
    char buffer[BUFFER_SIZE];

    // Criação do socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    memset(&clientAddr, 0, sizeof(clientAddr));

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Vincula o socket com o endereço do servidor
    if (bind(sockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no bind");
        exit(1);
    }

    struct FonteInformacao * temp = malloc(sizeof( struct FonteInformacao));

    printf("Servidor iniciado. Aguardando mensagens...\n");

    while (1) {
        printf("while");
        socklen_t addrLen = sizeof(clientAddr);

        // Recebe mensagem do cliente
        //int numBytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        //if (numBytes < 0) {
        //    perror("Erro na recepção da mensagem");
        //    exit(1);
        //}

        //buffer[numBytes] = '\0';
        //printf("Mensagem recebida: %s\n", buffer);
        printf ("if");
        if ((msgSize = recvfrom(sockfd, temp, sizeof(temp), 0, (struct sockaddr *) &clientAddr, &cliAddrLen)) < 0){
            perror("Erro na receção");
            exit(1);
        }
       

        printf("Handling client %s\n", inet_ntoa(clientAddr.sin_addr));
        printf("Incoming Length: %u\n", cliAddrLen);
        printf("Received: %d\n", temp -> Vi);

        // Envia uma resposta para o cliente
        //if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&clientAddr, addrLen) < 0) {
        //    perror("Erro no envio da mensagem");
        //    exit(1);
        //}

        printf("Resposta enviada.\n");
    }

    return 0;
}
