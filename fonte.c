#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>

#define SERVER_IP "127.0.0.1"
#define SERVER_PORT 8888
#define CONFIG_FILE "fonteconfig.txt"
#define MAX_LINE_LENGTH 100
#define BUFFER_SIZE 1024

#define PI 3.14159265359

// Definição da estrutura de dados

typedef struct {
    int D;  // Identificador da fonte
    int i;  // ?
    int P;  // Período 
    int F;  // Frequência
    int N;  // Número de amostras
    int M;  // Máximo??
    int Vi; // Informação
} FonteInformacao;

//void parseConfigFile(const char* filename, char* serverIP, int* serverPort, FonteInformacao fonte) {
//    FILE* file = fopen(filename, "r");
//    if (file == NULL) {
//        printf("Erro ao abrir o arquivo de configuração.\n");
//        exit(EXIT_FAILURE);
//    }

    // Leitura do SERVER_IP
//    fgets(serverIP, MAX_LINE_LENGTH, file);
//    serverIP[strcspn(serverIP, "\n")] = '\0';
    //printf("%s", &serverIP);

    // Leitura do SERVER_PORT
//    char serverPortStr[MAX_LINE_LENGTH];
//    fgets(serverPortStr, MAX_LINE_LENGTH, file);
//    *serverPort = atoi(serverPortStr);
    //printf("%d", &serverPort);
    // Leitura dos valores da FonteInformacao

//    char line[MAX_LINE_LENGTH];
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.D = atoi(line);
//
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.i = atoi(line);
//
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.Vi = atoi(line);
//
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.P = atoi(line);
//
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.F = atoi(line);
//
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.N = atoi(line);
//
//    fgets(line, MAX_LINE_LENGTH, file);
//    fonte.M = atoi(line);
//
//    fclose(file);
//}


int main() {
    char serverIP[MAX_LINE_LENGTH];
    int serverPort;
    FonteInformacao fonte;

    //parseConfigFile(CONFIG_FILE, serverIP, &serverPort, fonte);

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
    serverAddr.sin_port = htons(SERVER_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &serverAddr.sin_addr) <= 0) {
        perror("Erro na configuração do endereço do servidor");
        exit(1);
    }

    memset(&fonte, 0, sizeof(fonte));
    fonte.D = 1;
    fonte.F = 2;
    fonte.N = 7;
    fonte.M = 50;
    fonte.Vi = 0;
    int ativo = 1;
    int Fa = fonte.N * fonte.F;
    double x; 
    
    while(ativo == 1){
        for (int i = 1; i <= Fa; i++) {
        x = (1 + (1 + sin(2 * PI * i / fonte.N)) * 30);
        fonte.i = i;
        fonte.Vi = (int)x;
        
        //int tempint = 0;

        /* Send the string to the server */
        //tempint = sendto(sockfd, (FonteInformacao*)&fonte, (1024+sizeof(fonte)), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)); 

        //if (tempint == -1 ) {

        //       printf("Sent struct size: %d\n", tempint);
        //       DieWithError("sendto() sent a different number of bytes than expected\n");
        //}

        if (sendto(sockfd, (FonteInformacao*)&fonte, (1024+sizeof(fonte)), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
            perror("Erro no envio da mensagem");
            exit(1);
        }
        
        for(int j = 0; j < fonte.Vi; j++){
            printf("_");
        }
        printf("\n");
        }   
    }

    printf("Digite uma mensagem: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    // Envia a mensagem para o servidor
    //if (sendto(sockfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    //    perror("Erro no envio da mensagem");
    //    exit(1);
    // }

    printf("Mensagem enviada.\n");

    // Fecha o socket
    close(sockfd);

    return 0;
}
