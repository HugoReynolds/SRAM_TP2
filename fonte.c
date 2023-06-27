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
    int M;  // Máximo
} FonteInformacao;


// A função fonteThread é a função de entrada que será executada por cada thread criada
// Ela recebe um ponteiro arg que aponta para a estrutura de dados FonteInformacao correspondente a essa thread.
void* fonteThread(void* arg) {

    FonteInformacao* fonte = (FonteInformacao*)arg;

    unsigned short Fa = fonte->F * fonte->N;
    unsigned int Vi = 0;
    char buffer[MAX_BUFFER_SIZE];
    int periodo = 1/fonte->F;
    fonte->P = 0;
    char serverIP[MAX_LINE_LENGTH] = "127.0.0.1";
    int serverPort = 8888;


    // cria um socket UDP (SOCK_DGRAM) usando a função socket()
    int socket_fonte = socket(AF_INET, SOCK_DGRAM, 0);    
    if (socket_fonte < 0) {
        perror("Erro ao criar o socket");
        return NULL;
    }

    // A estrutura server_address é criada e preenchida com as informações do servidor de destino (endereço IP e porta).
    struct sockaddr_in server_address;
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr(serverIP);
    server_address.sin_port = htons(serverPort);


    //loop for, itera-se de 0 até Fa, realizando o envio dos dados para o servidor.
    for (int i = 0; i <= Fa; i++) {

        // verifica-se se fonte->P é menor ou igual a fonte->M. Se for verdadeiro, significa que a condição fonte->P <= fonte->M é atendida.
        if(fonte->P <= fonte->M){

            Vi = 1 + (1 + sin(2 * PI * i / fonte->N)) * 30; // Calculo do valor de Vi 

            // O conteúdo do buffer é formatado usando a função sprintf(), 
            // preenchendo-o com os valores de fonte->M, i, Vi, fonte->F, fonte->N, fonte->P e fonte->D.
            sprintf(buffer, "%d %d %d %d %d %d %s",fonte->M, i, Vi, fonte->F, fonte->N, fonte->P, fonte->D);

            // Os dados contidos no buffer são enviados para o servidor usando a função sendto(). 
            //O socket utilizado é socket_fonte, e os dados são enviados para o endereço especificado na estrutura server_address.
            sendto(socket_fonte, buffer, strlen(buffer), 0, (struct sockaddr*)&server_address, sizeof(server_address));

            printf("%s\n", buffer);
            
            // A função sleep() é chamada para pausar a execução da thread por um período de tempo especificado em periodo.
            sleep(periodo);

        // Quando atingir o valor maximo entra nesta condicão
        }else {

            fonte->P=1;
            Vi = 1 + (1 + sin(2 * PI * i / fonte->N)) * 30;

            sprintf(buffer, "%d %d %d %d %d %d %s",fonte->M, i, Vi, fonte->F, fonte->N, fonte->P, fonte->D);
            
            sendto(socket_fonte, buffer, strlen(buffer), 0, (struct sockaddr*)&server_address, sizeof(server_address));

            // Envie os dados para o servidor aqui usando o socket_fonte

            printf("%s\n", buffer);

            sleep(periodo);
        }
        // incrementa o Periodp
        fonte->P++;
    }

    return NULL;
}



int main() {

    // Variaveis para a comunicação entre a fonte e o servidor
    int socket_fonte;
    struct sockaddr_in sm_address; 
    char serverIP[MAX_LINE_LENGTH];
    int serverPort;

    // abertura do ficheiro "fonteconfig.txt" em modo de leitura
    FILE *file_config = fopen("fonteconfig.txt", "r");

    if (file_config == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        return 1;
    }

    int num_fontes = 0; // variavel para armazenar o numero de fontes
    char line[MAX_LINE_LENGTH];

    // na confiração que escolhemos sempre que lê "nova_fonte" significa que existe uma nova fonte
    while (fgets(line, sizeof(line), file_config) != NULL) {
        if (strstr(line, "nova_fonte") != NULL) {
            num_fontes++;
        }
    }

    // alocamos memória dinamicamente para um array de estruturas FonteInformacao
    FonteInformacao* fontes = (FonteInformacao*)malloc(num_fontes * sizeof(FonteInformacao));

    if (fontes == NULL) {
        printf("Erro ao alocar memória.\n");
        fclose(file_config);
        return 1;
    }

    // reposicionar o ponteiro do arquivo apontado por file_config de volta para o início do arquivo "fonteconfig.txt"
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

    fgetc(file_config);   // Consumir o caractere de nova linha após cada configuração de fonte
    }

    fclose(file_config);

    // Cria um array de threads para as fontes
    pthread_t threads[num_fontes];

    // Para cada fonte, a função pthread_create() é chamada para criar uma nova thread
    for (int i = 0; i < num_fontes; i++) {
        pthread_create(&threads[i], NULL, fonteThread, &fontes[i]);
    }

    // Aguarde todas as threads terminarem
    for (int i = 0; i < num_fontes; i++) {
        pthread_join(threads[i], NULL);
    }


    free(fontes);  // Libere a memória alocada para as fontes
    close(socket_fonte); 
    

    return 0;
}
