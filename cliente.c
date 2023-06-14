// Cliente UDP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 12345
#define BUFFER_SIZE 1024

int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];

    // Criação do socket UDP
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Erro na configuração do endereço do servidor");
        exit(1);
    }

    printf("Digite uma mensagem: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Envia a mensagem para o servidor
    if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no envio da mensagem");
        exit(1);
    }

    // Recebe a resposta do servidor
    int numBytes = recvfrom(sockfd, buffer, BUFFER_SIZE, 0, NULL, NULL);
    if (numBytes < 0) {
        perror("Erro na recepção da mensagem");
        exit(1);
    }

    buffer[numBytes] = '\0';

    printf("Resposta do servidor: %s\n", buffer);

    // Fecha o socket
    close(sockfd);

    return 0;
}
