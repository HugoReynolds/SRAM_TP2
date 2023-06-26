#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>


#define SERVER_IP "127.0.0.1"
#define CLIENT_PORT 12345
#define BUFFER_SIZE 100000
#define MAX_FONTS 100

typedef struct{
    char D[100];  // Identificador da fonte // Máximo??
    int i;  // index
    int Vi; // Informação
    int P;  // Período 
    int F;  // Frequência
    int N;  // Número de amostras
    int M;  // Máximo??
}FonteInformacao;

typedef struct {
    FonteInformacao fonts[MAX_FONTS];
    int count;
} FonteInformacaoList;





void send_Mensage_to_server(char *message, int soctfd,struct sockaddr_in serverAddr){
    char buffer[BUFFER_SIZE];
    //printf("message: %s\n", message);
    memset(buffer, 0, sizeof(buffer));
    strncpy(buffer, message, sizeof(buffer));
    //printf("buffer: %s\n", buffer);
    ssize_t numBytes = sendto(soctfd, buffer, strlen(buffer), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (numBytes < 0) {
        perror("Erro no envio da mensagem para o servidor\n");
        exit(1);
    }
    
}


void option2_list(FonteInformacaoList* list, int soctfd, struct sockaddr_in serverAddr){

    char buffer[BUFFER_SIZE];

    send_Mensage_to_server("list", soctfd, serverAddr );
        
    // Receber a lista de fontes de informação do servidor
    memset(buffer, 0, sizeof(buffer));

    ssize_t numBytes = recvfrom(soctfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (numBytes < 0) {
        perror("Erro na recepção da lista de fontes de informação");
        exit(1);
    }

    // Converter a lista recebida para a estrutura FonteInformacaoList
    int n_fontes = sscanf(buffer, "%d", &list->count);
    
    if (n_fontes < 0) {
        perror("Erro ao analisar a lista de fontes de informação");
        exit(1);
    }
    
    for (int i = 0; i < sizeof(list->count); i++) {
        memset(buffer, 0, sizeof(buffer));
        numBytes = recvfrom(soctfd, buffer, sizeof(buffer), 0, NULL, NULL);
        if (numBytes < 0) {
            perror("Erro na recepção da lista de fontes de informação");
            exit(1);
        }

        int fontes_id = sscanf(buffer, "%[^\n]", list->fonts[i].D);
        printf("%s", buffer);

        if (fontes_id != 1) {
            printf("Erro ao analisar a lista de fontes de informação\n");
            exit(1);
        }

    }
    close(soctfd);

}

void option2_info(FonteInformacaoList* list, char* message, int soctfd, struct sockaddr_in serverAddr) {
    const char* info = "info";

    printf("Info da fonte: %s\n", message);
    char buffer[BUFFER_SIZE];

    int offset = snprintf(buffer, sizeof(buffer), "%s %s", info, message);
    if (offset < 0 || offset >= BUFFER_SIZE) {
        printf("Erro ao concatenar as strings\n");
        exit(1);
    }

    printf("Buffer no fim: %s\n", buffer);

    send_Mensage_to_server(buffer, soctfd, serverAddr);

    // Receber a lista de fontes de informação do servidor
    memset(buffer, 0, sizeof(buffer));

    ssize_t numBytes = recvfrom(soctfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (numBytes < 0) {
        perror("Erro na recepção da lista de fontes de informação");
        exit(1);
    }

    printf("Valores de D, F, N e M recebidos: %s\n", buffer);

    close(soctfd);
}

void option2_play(FonteInformacaoList* list, char* message, int soctfd, struct sockaddr_in serverAddr) {
    const char* info = "play";
    char buffer[BUFFER_SIZE];
     // Tempo de espera em microssegundos (0,1 segundo)

    int offset = snprintf(buffer, sizeof(buffer), "%s %s", info, message);
    if (offset < 0 || offset >= BUFFER_SIZE) {
        printf("Erro ao concatenar as strings\n");
        exit(1);
    }

    send_Mensage_to_server(buffer, soctfd, serverAddr);

    // Receber a lista de fontes de informação do servidor
     memset(buffer, 0, sizeof(buffer));

    ssize_t numBytes = recvfrom(soctfd, buffer, sizeof(buffer), 0, NULL, NULL);
    if (numBytes < 0) {
        perror("Erro na recepção dos valores de i e Vi do servidor");
        exit(1);
    }

    printf("Valores de i e Vi recebidos: %s\n", buffer);
   

    close(soctfd);
}



int main() {
    int sockfd;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    FonteInformacaoList fonte_informacao_list;
    FonteInformacao fonte_info;
    int option, option2;
    char font_op1;
    char font_op2;
    char font_op3;
    struct timeval timeout;
    timeout.tv_sec = 0;
    timeout.tv_usec = 100000; 

    

    
    
    while(1){
        printf("MENU:\n");
        printf("1. Start\n");
        printf("2. End\n");
        printf("Escolha uma opção: ");
        scanf("%d", &option);

        

        


        switch(option){
            case 1: {
                do {
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
                    
                    

                    
                    printf("\nSUBMENU CLIENTE:\n");
                    printf("1. List\n");
                    printf("2. Info\n");
                    printf("3. Play\n");
                    printf("4. Stop\n");
                    printf("5. Leave\n");
                    printf("Escolha uma opção: ");
                    scanf("%d", &option2);

                    switch(option2){
                        case 1: {

                            //Lista das Fontes Dísponiveis
                            printf("Fontes Dísponiveis: ");
                            option2_list(&fonte_informacao_list, sockfd, serverAddr);

                            
                            break;
                        }
                        case 2: {
                            //Dados de fonte/fontes dísponiveis
                            printf("Especifique a fonte:\n");
                            scanf("%s", &font_op1);
                            //printf("%s", font_op);
                            option2_info(&fonte_informacao_list,&font_op1, sockfd, serverAddr);
                            break;
                        }
                        case 3: {
                            printf("Digite o nome da fonte:\n");
                            scanf("%s", &font_op2);
                            //printf("%s", font_op);
                            option2_play(&fonte_informacao_list,&font_op2, sockfd, serverAddr);
                            
                            break;
                        }
                        case 4: {
                            
                            //Parar a Transmissão de Dados da Fonte X
                            printf("Fim de Transmissão\n");
                            break;
                        }
                        case 5: {
                            //Sair do Ciclo
                            printf("Fim de Execução\n");
                            break;
                        }
                        default:
		                    printf("wrong Input\n");
                            continue;                        
                    }

                    
                }while (option2 != 5);

                break;
            }
            case 2: {
                printf("Fim do Programa\n");
                exit(1);
            }
            default:
		        printf("wrong Input\n");  
        }
    }



    

    // Fechar o socket
    close(sockfd);

    return 0;
}
