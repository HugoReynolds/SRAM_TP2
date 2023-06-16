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
#define MAX_LINE_LENGTH 1000
#define MAX_BUFFER_SIZE 1024

#define PI 3.14159265359

// Definição da estrutura de dados

typedef struct {
    char D[100];  // Identificador da fonte
    int i;  // index
    int Vi; // Informação
    int P;  // Período 
    int F;  // Frequência
    int N;  // Número de amostras
    int M;  // Máximo??
    
} FonteInformacao;



void* fonteThread(void* arg) {
    FonteInformacao* fonte = (FonteInformacao*)arg;
    unsigned short Fa = fonte->F * fonte->N;
    unsigned int Vi = 0;
    char buffer[MAX_BUFFER_SIZE];
    fonte->P = 1/fonte->F;
    char serverIP[MAX_LINE_LENGTH] = "127.0.0.1";
    int serverPort = 8888;


    
    int socket_fonte = socket(AF_INET, SOCK_DGRAM, 0);    
    if (socket_fonte < 0) {
        perror("Erro ao criar o socket");
        return NULL;
    }

    struct sockaddr_in server_address;
    printf("Connecting %d\n", serverPort);
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(serverIP);
    server_address.sin_port = htons(serverPort);

    for (int i = 1; i <= Fa; i++) {
        Vi = 1 + (1 + sin(2 * PI * i / fonte->N)) * 30;
        printf("Vi: %d\n", Vi);

        sprintf(buffer, "%d %d %d %d %d %s", i, Vi, fonte->F, fonte->N, fonte->P, fonte->D);
        sendto(socket_fonte, buffer, strlen(buffer), 0, (struct sockaddr*)&server_address, sizeof(server_address));

        // Envie os dados para o servidor aqui usando o socket_fonte

        printf("Enviado: %s\n", buffer);

        sleep(fonte->P);
    }

    return NULL;
}



int main() {

    int socket_fonte;
    struct sockaddr_in sm_address;
    char serverIP[MAX_LINE_LENGTH];
    int serverPort;

    FonteInformacao fonte;
    unsigned short Fa = 0;
    unsigned int Vi = 0;




    FILE *file_config = fopen("fonteconfig.txt", "r");

    if (file_config == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    int num_fontes = 0;
    char line[MAX_LINE_LENGTH];

    while (fgets(line, sizeof(line), file_config) != NULL) {
        if (strstr(line, "nova_fonte") != NULL) {
            
            num_fontes++;
        }
    }

    printf("Número de fontes lido: %d\n", num_fontes);


    FonteInformacao* fontes = (FonteInformacao*)malloc(num_fontes * sizeof(FonteInformacao));

    if (fontes == NULL) {
        printf("Erro ao alocar memória.\n");
        fclose(file_config);
        return 1;
    }

    rewind(file_config);



    // Ler as configurações para cada fonte
    for (int i = 0; i < num_fontes; i++) {
    // Verificar se a linha corresponde a "fonteinfo"
    while (fgets(line, sizeof(line), file_config) != NULL) {
        if (strstr(line, "nova_fonte") != NULL) {
            break;
        }
    }

    // Ler o identificador da fonte
    fscanf(file_config, "%s", fontes[i].D);

    // Ler as outras configurações da fonte
    fscanf(file_config, "%d", &fontes[i].F);
    fscanf(file_config, "%d", &fontes[i].N);
    fscanf(file_config, "%d", &fontes[i].M);
    fscanf(file_config, "%s", serverIP);
    fscanf(file_config, "%d", &serverPort);

    //printf("Nome da Fonte: %s\n", fontes[i].D);
    //printf("N: %d\n", fontes[i].M);
    //printf("Server IP: %s\n", serverIP);
    //printf("Server_Port: %d\n", serverPort);

    // Resto do código...

    fgetc(file_config);   // Consumir o caractere de nova linha após cada configuração de fonte
}

    fclose(file_config);

    /*
    fscanf(file_config, "%s", fonte.D);
    fscanf(file_config, "%d", &fonte.F);
    fscanf(file_config, "%d", &fonte.N);
    fscanf(file_config, "%d", &fonte.M);
    fscanf(file_config, "%s", serverIP);
    fscanf(file_config, "%d", &serverPort);

    printf("Server IP: %s\n", serverIP);

    fclose(file_config);
    */



    socket_fonte = socket(AF_INET, SOCK_DGRAM, 0);    
    if (socket_fonte < 0) {
        perror("Erro ao criar o socket");
        return 1;
    }

    printf("Connecting %d\n", serverPort);
    memset(&sm_address, 0, sizeof(sm_address));
    sm_address.sin_family = AF_INET;
    sm_address.sin_addr.s_addr = inet_addr(serverIP);
    sm_address.sin_port = htons(serverPort);


    

    

    // Cria um array de threads para as fontes
    pthread_t threads[num_fontes];

    // Crie e execute as threads para cada fonte
    for (int i = 0; i < num_fontes; i++) {
        pthread_create(&threads[i], NULL, fonteThread, &fontes[i]);
    }

    // Aguarde todas as threads terminarem
    for (int i = 0; i < num_fontes; i++) {
        pthread_join(threads[i], NULL);
    }

    // Resto do código...

    free(fontes);  // Libere a memória alocada para as fontes

    

    /*

    int i = 0;

    Fa = fonte.F * fonte.N;
    fonte.P = 1 / fonte.F;
    char buffer[MAX_BUFFER_SIZE];

    for(i = 1; i <= Fa; i++){
        Vi = 1 + (1 + sin(2 * PI * i/fonte.N)) * 30;
        printf("Vi: %d\n",Vi);
        
        sprintf(buffer, "%d %d %d %d %d %s", i, Vi, fonte.F, fonte.N, fonte.P, fonte.D);

        sendto(socket_fonte, buffer, strlen(buffer), 0, (struct sockaddr*)&sm_address, sizeof(sm_address));
        printf("Enviado: %s\n", buffer);

        sleep(fonte.P);
    }

    */

     close(socket_fonte);
    





    return 0;
}
