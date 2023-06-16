#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define MAX_BUFFER_SIZE 1024

typedef struct{
    char D[100];  // Identificador da fonte
    int i;  // index
    int Vi; // Informação
    int P;  // Período 
    int F;  // Frequência
    int N;  // Número de amostras
    int M;  // Máximo??
}FonteInformacao;

int main() {


    int socket_receptor;
    struct sockaddr_in receptor_address, sender_address;
    unsigned int sender_address_length;
    char buffer[MAX_BUFFER_SIZE];

    int receptorPort = 8888; // Porta na qual o receptor vai escutar

    // Criação do socket
    socket_receptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_receptor < 0) {
        perror("Erro ao criar o socket");
        return 1;
    }

    // Configuração do endereço do receptor
    memset(&receptor_address, 0, sizeof(receptor_address));
    receptor_address.sin_family = AF_INET;
    receptor_address.sin_addr.s_addr = htonl(INADDR_ANY);
    receptor_address.sin_port = htons(receptorPort);

    // Associação do socket com o endereço do receptor
    if (bind(socket_receptor, (struct sockaddr*)&receptor_address, sizeof(receptor_address)) < 0) {
        perror("Erro ao associar o socket ao endereço");
        return 1;
    }

    printf("Receptor UDP aguardando mensagens...\n");

    while (1) {
        // Recebimento da mensagem
        memset(buffer, 0, sizeof(buffer));
        sender_address_length = sizeof(sender_address);
        ssize_t received_bytes = recvfrom(socket_receptor, buffer, sizeof(buffer), 0, (struct sockaddr*)&sender_address, &sender_address_length);

     //   write(1,"test\n",5*sizeof(char));
        if (received_bytes < 0) {
            perror("Erro ao receber a mensagem");
            return 1;
        }

        printf("Mensagem recebida: %s\n", buffer);
    }

    // Fechamento do socket
    close(socket_receptor);



   

    return 0;
}
