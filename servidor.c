#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#define MAX_BUFFER_SIZE 1024
#define FONT_SOURCE_PORT 8888
#define CLIENT_PORT 12345
#define SERVER_IP "127.0.0.1"
#define MAX_FONTS 100
#define MAX_SUBSCRIPTIONS 100

typedef struct{
    char D[100];  // Identificador da fonte
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

typedef struct {
    char fonte_name[250];
    struct sockaddr_in* clientAddr;
} Subscritor;

typedef struct {
    Subscritor subscritores[MAX_FONTS];
    int count;
} SubscritorList;

SubscritorList* subscritor_list;

 


void printFonteInformacao(FonteInformacao* fonte) {
    printf("D: %s\n", fonte->D);
    printf("i: %d\n", fonte->i);
    printf("Vi: %d\n", fonte->Vi);
    printf("P: %d\n", fonte->P);
    printf("F: %d\n", fonte->F);
    printf("N: %d\n", fonte->N);
    printf("M: %d\n", fonte->M);
    printf("\n");
}

SubscritorList* createSubscritorList() {
    SubscritorList* list = (SubscritorList*)malloc(sizeof(SubscritorList));
    list->count = 0;
    
    for (int i = 0; i < MAX_FONTS; i++) {
        strcpy(list->subscritores[i].fonte_name, "");
        list->subscritores[i].clientAddr = NULL;
    }
    
    return list;
}


void addSubscritor(SubscritorList* list, char* fonte_name, struct sockaddr_in* clientAddr) {
    strcpy(list->subscritores[list->count].fonte_name, fonte_name);
    list->subscritores[list->count].clientAddr = clientAddr;
    list->count++;
}

void addFonteInformacao(FonteInformacaoList* list, FonteInformacao* fonte) {
 for (int i = 0; i < list->count; i++) {
        if (strcmp(list->fonts[i].D, fonte->D) == 0) {
            // A fonte já existe na lista
            //printf("A fonte de informação '%s' já está na lista\n", fonte->D);
            return;
        }
    }

    if (list->count < MAX_FONTS) {
        list->fonts[list->count++] = *fonte;
        //printf("A fonte de informação '%s' foi adicionada à lista\n", fonte->D);
    } else {
        printf("A lista de fontes de informação está cheia\n");
    }
}

void sendFonteInformacaoList(int sockfd, struct sockaddr_in* clientAddr, FonteInformacaoList* list) {
    char buffer[MAX_BUFFER_SIZE];
    int offset = 0;

    // Primeiro, envie o número de fontes de informação
    int result = snprintf(buffer, MAX_BUFFER_SIZE, "%d\n", list->count);
    if (result < 0 || result >= MAX_BUFFER_SIZE) {
        printf("Erro ao criar a lista de fontes de informação\n");
        return;
    }

    ssize_t numBytes = sendto(sockfd, buffer, result, 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
    if (numBytes < 0) {
        perror("Erro no envio da lista de fontes de informação para o cliente");
        exit(1);
    }

    // Em seguida, envie o parâmetro fonte->D de cada fonte de informação
    for (int i = 0; i < list->count; i++) {
        FonteInformacao* fonte = &list->fonts[i];
        result = snprintf(buffer, MAX_BUFFER_SIZE, "%s\n", fonte->D);
        if (result < 0 || result >= MAX_BUFFER_SIZE) {
            printf("Erro ao criar a lista de fontes de informação\n");
            return;
        }

        numBytes = sendto(sockfd, buffer, result, 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
        if (numBytes < 0) {
            perror("Erro no envio da lista de fontes de informação para o cliente");
            exit(1);
        }
    }
}

void sendFonteInfo(char *fonte_id, int sockfd, struct sockaddr_in* clientAddr, FonteInformacaoList* list) {
    char buffer[MAX_BUFFER_SIZE];

    printf("Fonte_Id: %s\n", fonte_id);

    for (int i = 0; i < list->count; i++) {
        FonteInformacao* fonte = &list->fonts[i];

        if (strcmp(fonte_id, fonte->D) == 0) {
            printf("entras aqui?\n");
            // A fonte corresponde à fonte_id solicitada

            int result = snprintf(buffer, MAX_BUFFER_SIZE, "D: %s\nF: %d\nN: %d\nM: %d\n",
                fonte->D, fonte->F, fonte->N, fonte->M);

            if (result < 0 || result >= MAX_BUFFER_SIZE) {
                printf("Erro ao criar o buffer da fonte de informação\n");
                return;
            }

            ssize_t numBytes = sendto(sockfd, buffer, result, 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
            if (numBytes < 0) {
                perror("Erro no envio da fonte de informação para o cliente");
                exit(1);
            }

            return; // Encerra a função após enviar a fonte encontrada
        }
    }

    // A fonte_id solicitada não foi encontrada
    const char* notFoundMessage = "Fonte de informação não encontrada";
    ssize_t numBytesSent = sendto(sockfd, notFoundMessage, strlen(notFoundMessage), 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
    if (numBytesSent < 0) {
        perror("Erro no envio da mensagem 'Fonte de informação não encontrada' para o cliente");
        exit(1);
    }
}

void Play_fonte(char *fonte_id, int sockfd, struct sockaddr_in* clientAddr, FonteInformacaoList* list) {
    char buffer[MAX_BUFFER_SIZE];

    printf("Fonte_Id: %s\n", fonte_id);

    for (int i = 0; i < list->count; i++) {
        FonteInformacao* fonte = &list->fonts[i];

        if (strcmp(fonte_id, fonte->D) == 0) {
            // A fonte corresponde à fonte_id solicitada

            int result = snprintf(buffer, MAX_BUFFER_SIZE, "i: %d\nVi: %d\n", fonte->i, fonte->Vi);

            if (result < 0 || result >= MAX_BUFFER_SIZE) {
                printf("Erro ao criar o buffer da fonte de informação\n");
                return;
            }

            ssize_t numBytes = sendto(sockfd, buffer, result, 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
            if (numBytes < 0) {
                perror("Erro no envio da fonte de informação para o cliente");
                exit(1);
            }

            return; // Encerra a função após enviar a fonte encontrada
        }
    }

    // A fonte_id solicitada não foi encontrada
    const char* notFoundMessage = "Fonte de informação não encontrada";
    ssize_t numBytesSent = sendto(sockfd, notFoundMessage, strlen(notFoundMessage), 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
    if (numBytesSent < 0) {
        perror("Erro no envio da mensagem 'Fonte de informação não encontrada' para o cliente");
        exit(1);
    }
}

void printFonteInformacaoList(FonteInformacaoList* list) {
    printf("Fontes disponíveis:\n");
    for (int i = 0; i < list->count; i++) {
        printf("- %s\n", list->fonts[i].D);
    }
    printf("\n");
}

void showSubscritorList(SubscritorList* list) {
    printf("Subscritores:\n");
    
    for (int i = 0; i < list->count; i++) {
        printf("Subscritor %d:\n", i + 1);
        printf("Fonte: %s\n", list->subscritores[i].fonte_name);
        
        if (list->subscritores[i].clientAddr != NULL) {
            struct sockaddr_in* addr = list->subscritores[i].clientAddr;
            char ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &(addr->sin_addr), ip, INET_ADDRSTRLEN);
            int port = ntohs(addr->sin_port);
            printf("Endereço: %s:%d\n", ip, port);
        } else {
            printf("Endereço: N/A\n");
        }
        
        printf("\n");
    }
}

void* fonteThread(void* arg) {
    char buffer[MAX_BUFFER_SIZE];
    int sockfd;
    struct sockaddr_in fontSourceAddr, clientAddr;
    FonteInformacao fonte_informacao;
  
    FonteInformacaoList* fonte_informacao_list = (FonteInformacaoList*)arg;
    fonte_informacao_list->count = 0;    
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }

    memset(&fontSourceAddr, 0, sizeof(fontSourceAddr));

    // Configuração do endereço da fonte de informação
    fontSourceAddr.sin_family = AF_INET;
    fontSourceAddr.sin_port = htons(FONT_SOURCE_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &fontSourceAddr.sin_addr) <= 0) {
        perror("Erro na configuração do endereço da fonte de informação");
        exit(1);
    }

    if (bind(sockfd, (struct sockaddr *)&fontSourceAddr, sizeof(fontSourceAddr)) < 0) {
        perror("Erro no bind");
        exit(1);
    }


        while (1) {
        memset(buffer, 0, sizeof(buffer));
        socklen_t addrLen = sizeof(fontSourceAddr);
        ssize_t numBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&fontSourceAddr, &addrLen);
        if (numBytes < 0) {
            perror("Erro na recepção da mensagem da fonte de informação");
            exit(1);
        }


        int result = sscanf(buffer, "%d %d %d %d %d %d %[^\n]", &fonte_informacao.M, &fonte_informacao.i,
        &fonte_informacao.Vi, &fonte_informacao.F, &fonte_informacao.N, &fonte_informacao.P, fonte_informacao.D);


        if (result == 7) {

            addFonteInformacao(fonte_informacao_list, &fonte_informacao);

            for (int i = 0; i < subscritor_list->count; i++) {
                Subscritor* subscritor = &subscritor_list->subscritores[i];
                if (strcmp(subscritor->fonte_name, fonte_informacao.D) == 0) {
                    // Criar um novo datagrama UDP com os parâmetros do datagrama recebido
                    char newData[MAX_BUFFER_SIZE];
                    snprintf(newData, sizeof(newData), "%d %d %d %d %d %d %s", fonte_informacao.M, fonte_informacao.i,
                        fonte_informacao.Vi, fonte_informacao.F, fonte_informacao.N, fonte_informacao.P, fonte_informacao.D);

                    // Enviar o novo datagrama para o cliente
                    int clientSockfd = socket(AF_INET, SOCK_DGRAM, 0);
                    if (clientSockfd < 0) {
                        perror("Erro na criação do socket para o cliente");
                        exit(1);
                    }

                    memset(&clientAddr, 0, sizeof(clientAddr));
                    clientAddr.sin_family = AF_INET;
                    clientAddr.sin_port = htons(CLIENT_PORT);
                    

                    ssize_t numSentBytes = sendto(clientSockfd, newData, strlen(newData), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
                    printf("numSentBytes na fonte = %ld\n", numSentBytes);
                    if (numSentBytes < 0) {
                        perror("Erro no envio do datagrama para o cliente");
                        exit(1);
                    }

                    close(clientSockfd);
                }
            }

        } else {
            printf("Erro ao analisar o buffer\n");
        }
    }

    return NULL;

}



void* Cliente_Thread(void* arg) {

    struct sockaddr_in clientAddr, serverAddr;
    char buffer[MAX_BUFFER_SIZE];
    FonteInformacaoList* fonte_informacao_list = (FonteInformacaoList*)arg;
    subscritor_list = createSubscritorList();
    int sockfd;


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }


    int clientSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSockfd < 0) {
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


    // Associa o socket à porta do cliente
    if (bind(clientSockfd, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no bind do cliente");
        exit(1);
    }


    while (1) {
        memset(buffer, 0, sizeof(buffer));
        socklen_t addrLen = sizeof(clientAddr);
        ssize_t numBytes = recvfrom(clientSockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (numBytes < 0) {
            perror("Erro na recepção da mensagem do cliente");
            exit(1);
        }


        if (strcmp(buffer, "list") == 0) {
            printFonteInformacaoList(fonte_informacao_list);

            const char* okMessage = "OK";
            ssize_t numBytesSent = sendto(clientSockfd, okMessage, strlen(okMessage), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
            if (numBytesSent < 0) {
                perror("Erro no envio da mensagem 'OK' para o cliente");
                exit(1);
            }
            sendFonteInformacaoList(sockfd, &clientAddr, fonte_informacao_list);
            
        }

        else if (strncmp(buffer, "info", 4) == 0) {
            
            char nome_fonte[MAX_BUFFER_SIZE];
            sscanf(buffer + 5, "%s", nome_fonte); 

            sendFonteInfo(nome_fonte, sockfd, &clientAddr, fonte_informacao_list);

        }else if (strncmp(buffer, "play", 4) == 0) {
            
            char nome_fonte[MAX_BUFFER_SIZE];
            sscanf(buffer + 5, "%s", nome_fonte);

            addSubscritor(subscritor_list,nome_fonte,&clientAddr);

            showSubscritorList(subscritor_list);

            Play_fonte(nome_fonte, sockfd, &clientAddr, fonte_informacao_list);
        }



        // Lógica para processar a mensagem do cliente
        printf("Mensagem enviada para o cliente: %s\n", buffer);


    }
    free(subscritor_list);

    return NULL;

}


int main() {

    
    int sockfd;
    struct sockaddr_in serverAddr, fontSourceAddr, clientAddr;
    int clientSockfd;
    char buffer[MAX_BUFFER_SIZE];
    FonteInformacao fonte_informacao;
    FonteInformacaoList* fonte_informacao_list = (FonteInformacaoList*)malloc(sizeof(FonteInformacaoList));    
    
   
    


    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }



    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    //serverAddr.sin_port = htons(FONT_SOURCE_PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associação do socket ao endereço do servidor
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no bind");
        exit(1);
    }

    pthread_t fontSourceThread;
    pthread_t clientThread;



    if (pthread_create(&clientThread, NULL, Cliente_Thread, fonte_informacao_list) != 0) {
        perror("Erro na criação da thread da fonte de informação");
        exit(1);
    }     

    if (pthread_create(&fontSourceThread, NULL, fonteThread, fonte_informacao_list) != 0) {
    perror("Erro na criação da thread da fonte de informação");
    exit(1);
    }

    //pthread_join(fontSourceThread, NULL);
    pthread_join(clientThread, NULL);


    
    free(fonte_informacao_list);

    // Fecha os sockets
    close(sockfd);




    return 0;
}
