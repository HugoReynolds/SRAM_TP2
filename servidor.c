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

// Estrurura de dados de uma fonte de Informação
typedef struct{
    char D[100];  // Identificador da fonte
    int i;  // index
    int Vi; // Informação
    int P;  // Período 
    int F;  // Frequência
    int N;  // Número de amostras
    int M;  // Máximo??
}FonteInformacao;

// Estrutura de dados de uma lista de Fontes de Informação
typedef struct {
    FonteInformacao fonts[MAX_FONTS];
    int count;
} FonteInformacaoList;

//Estrutura de dados de um Subscritor
typedef struct {
    char fonte_name[250];
    struct sockaddr_in* clientAddr;
} Subscritor;

//Estrutura de dados a lista de Subscritores
typedef struct {
    Subscritor subscritores[MAX_FONTS];
    int count;
} SubscritorList;


SubscritorList* subscritor_list; // declaração da variavel global lista de Subscritores

 
//Esta função cria e inicializa uma nova instância da estrutura SubscritorList
SubscritorList* createSubscritorList() {

    //aloca memória para a estrutura SubscritorList
    SubscritorList* list = (SubscritorList*)malloc(sizeof(SubscritorList));
    list->count = 0;
    
    // percorre o array de subscritores subscritores e inicializa cada elemento, 
    // definindo o nome da fonte como uma string vazia ("") e o endereço do cliente como NULL.
    for (int i = 0; i < MAX_FONTS; i++) {
        strcpy(list->subscritores[i].fonte_name, "");
        list->subscritores[i].clientAddr = NULL;
    }
    
    // retorna o ponteiro para a nova instância da estrutura SubscritorList
    return list;
}

// Essa função é responsável por adicionar um novo subscritor à lista de Subscritores
void addSubscritor(SubscritorList* list, char* fonte_name, struct sockaddr_in* clientAddr) {

    // Copia o nome da fonte (fonte_name) para o membro fonte_name do próximo subscritor na lista.
    strcpy(list->subscritores[list->count].fonte_name, fonte_name);

    // Atribui o endereço do cliente (clientAddr) ao membro clientAddr do próximo subscritor na lista.
    list->subscritores[list->count].clientAddr = clientAddr;

    // Incrementa o contador count para indicar que um novo subscritor foi adicionado à lista.
    list->count++;
}

// Função é responsável por adicionar uma nova fonte de informação à lista, desde que essa fonte não exista na lista.
void addFonteInformacao(FonteInformacaoList* list, FonteInformacao* fonte) {

    // Percorre a lista de fontes de informação existentes 
    for (int i = 0; i < list->count; i++) {

        // Verifica se o nome da fonte (D) da fonte que se deseja adicionar é igual ao nome de alguma fonte já presente na lista. 
        if (strcmp(list->fonts[i].D, fonte->D) == 0) {
            //Se uma fonte com o mesmo nome já existir na lista, a função retorna sem adicionar a fonte novamente.
            return;
        }
    }

    // Se a lista não estiver cheia (ou seja, o número de fontes na lista for menor que MAX_FONTS),
    // a função adiciona a nova fonte à lista, copiando os dados da fonte para a próxima posição disponível na lista.
    if (list->count < MAX_FONTS) {
        list->fonts[list->count++] = *fonte;
    } else {
        printf("A lista de fontes de informação está cheia\n");
    }
}

// // A função sendFonteInformacaoList é responsável por enviar a lista de fontes de informação para um cliente por meio de um socket UDP.
void sendFonteInformacaoList(int sockfd, struct sockaddr_in* clientAddr, FonteInformacaoList* list) {
    
    char buffer[MAX_BUFFER_SIZE];
    int offset = 0;

    // Primeiro, envie o número de fontes de informação
    int result = snprintf(buffer, MAX_BUFFER_SIZE, "%d\n", list->count);
    if (result < 0 || result >= MAX_BUFFER_SIZE) {
        printf("Erro ao criar a lista de fontes de informação\n");
        return;
    }

    // O número de bytes é então enviado para o cliente 
    ssize_t numBytes = sendto(sockfd, buffer, result, 0, (struct sockaddr*)clientAddr, sizeof(*clientAddr));
    if (numBytes < 0) {
        perror("Erro no envio da lista de fontes de informação para o cliente");
        exit(1);
    }

    // Envia o parâmetro fonte->D de cada fonte de informação
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

// A função sendFonteInfo é responsável por enviar as informações de uma fonte de informação específica para um cliente por meio de um socket UDP.
void sendFonteInfo(char *fonte_id, int sockfd, struct sockaddr_in* clientAddr, FonteInformacaoList* list) {
    char buffer[MAX_BUFFER_SIZE];

    //  iteramos sobre cada fonte de informação na lista.
    for (int i = 0; i < list->count; i++) {

        FonteInformacao* fonte = &list->fonts[i];

        // Para cada fonte, ela verifica se o ID da fonte (fonte->D) corresponde ao fonte_id solicitado.
        if (strcmp(fonte_id, fonte->D) == 0) {

            // Se houver uma correspondência, a função cria uma string formatada no buffer contendo as informações da fonte encontrada (nome, F, N e M).
            int result = snprintf(buffer, MAX_BUFFER_SIZE, "D: %s\nF: %d\nN: %d\nM: %d\n",
                fonte->D, fonte->F, fonte->N, fonte->M);

            if (result < 0 || result >= MAX_BUFFER_SIZE) {
                printf("Erro ao criar o buffer da fonte de informação\n");
                return;
            }

            // o conteúdo do buffer é enviado para o cliente 
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

// A função Play_Option é responsável por enviar os parâmetros "i" e "Vi" de uma fonte de informação específica para um cliente por meio de um socket UDP.
void Play_Option(char *fonte_id, int sockfd, struct sockaddr_in* clientAddr, FonteInformacaoList* list) {
    char buffer[MAX_BUFFER_SIZE];

    // iteramos sobre cada fonte de informação na lista.
    for (int i = 0; i < list->count; i++) {

        FonteInformacao* fonte = &list->fonts[i];

        if (strcmp(fonte_id, fonte->D) == 0) {
            // A fonte corresponde à fonte_id solicitada
            // Se houver uma correspondência, a função cria uma string formatada no buffer contendo os parâmetros "i" e "Vi" da fonte encontrada.
            int result = snprintf(buffer, MAX_BUFFER_SIZE, "i: %d\nVi: %d\n", fonte->i, fonte->Vi);

            if (result < 0 || result >= MAX_BUFFER_SIZE) {
                printf("Erro ao criar o buffer da fonte de informação\n");
                return;
            }

            // o conteúdo do buffer é enviado para o cliente
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

// A função printFonteInformacaoList é responsável por imprimir no console a lista de fontes de informação disponíveis.
void printFonteInformacaoList(FonteInformacaoList* list) {
    printf("Fontes disponíveis:\n");
    for (int i = 0; i < list->count; i++) {
        printf("- %s\n", list->fonts[i].D);
    }
    printf("\n");
}

// A função showSubscritorList é responsável por exibir no terminal a lista de subscritores.
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

// A função fonteThread é uma função de thread responsável por receber e processar mensagens da fonte de informação.
void* fonteThread(void* arg) {

    char buffer[MAX_BUFFER_SIZE];
    int sockfd;
    struct sockaddr_in fontSourceAddr, clientAddr;
    FonteInformacao fonte_informacao;
  
    FonteInformacaoList* fonte_informacao_list = (FonteInformacaoList*)arg;
    fonte_informacao_list->count = 0;   

    // Configuração o endereço da fonte de informação usando o IP do servidor e a porta definida.
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }

    memset(&fontSourceAddr, 0, sizeof(fontSourceAddr));

    fontSourceAddr.sin_family = AF_INET;
    fontSourceAddr.sin_port = htons(FONT_SOURCE_PORT);
    if (inet_pton(AF_INET, SERVER_IP, &fontSourceAddr.sin_addr) <= 0) {
        perror("Erro na configuração do endereço da fonte de informação");
        exit(1);
    }

    // Realiza o bind do socket com o endereço da fonte de informação.
    if (bind(sockfd, (struct sockaddr *)&fontSourceAddr, sizeof(fontSourceAddr)) < 0) {
        perror("Erro no bind");
        exit(1);
    }

    //loop infinito para receber mensagens da fonte de informação
    while (1) {

        // Recebe uma mensagem na variável buffer usando a função recvfrom.
        memset(buffer, 0, sizeof(buffer));
        socklen_t addrLen = sizeof(fontSourceAddr);

        // Recebe uma mensagem na variável buffer usando a função recvfrom.
        ssize_t numBytes = recvfrom(sockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&fontSourceAddr, &addrLen);
        if (numBytes < 0) {
            perror("Erro na recepção da mensagem da fonte de informação");
            exit(1);
        }

        // Faz a análise do buffer recebido usando a função sscanf para extrair os parâmetros da fonte de informação.
        int result = sscanf(buffer, "%d %d %d %d %d %d %[^\n]", &fonte_informacao.M, &fonte_informacao.i,
        &fonte_informacao.Vi, &fonte_informacao.F, &fonte_informacao.N, &fonte_informacao.P, fonte_informacao.D);

        // Se a análise for bem-sucedida e todos os parâmetros forem lidos corretamente, 
        // adiciona a fonte de informação à lista usando a função addFonteInformacao.
        if (result == 7) {

            addFonteInformacao(fonte_informacao_list, &fonte_informacao);

            // itera sobre a lista de subscritores e verifica se há algum subscritor registrado para a fonte de informação atual.
            for (int i = 0; i < subscritor_list->count; i++) {
                Subscritor* subscritor = &subscritor_list->subscritores[i];
                
                // Se houver um subscritor registrado para a fonte, cria um novo datagrama UDP com os parâmetros da fonte de informação recebida.
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
                    
                    // Envia o datagrama para o cliente usando a função sendto. 
                    ssize_t numSentBytes = sendto(clientSockfd, newData, strlen(newData), 0, (struct sockaddr*)&clientAddr, sizeof(clientAddr));
                    printf("numSentBytes na fonte = %ld\n", numSentBytes);
                    if (numSentBytes < 0) {
                        perror("Erro no envio do datagrama para o cliente");
                        exit(1);
                    }
                    close(clientSockfd);
                }
            }
            // Repete esse processo de receber mensagens e enviar dados para subscritores indefinidamente.

        } else {
            printf("Erro ao analisar o buffer\n");
        }
    }

    return NULL;

}

// A função Cliente_Thread é uma função de thread responsável por receber e processar mensagens do cliente.
void* Cliente_Thread(void* arg) {

    struct sockaddr_in clientAddr, serverAddr;
    char buffer[MAX_BUFFER_SIZE];
    FonteInformacaoList* fonte_informacao_list = (FonteInformacaoList*)arg;
    subscritor_list = createSubscritorList();
    int sockfd;

    // Cria um socket sockfd para comunicação com a fonte de informação.
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }

    // Cria um socket clientSockfd para comunicação com o cliente.
    int clientSockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (clientSockfd < 0) {
        perror("Erro na criação do socket do cliente");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));

    // Configura o endereço do servidor usando o IP do servidor e a porta definida.
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

    // loop infinito para receber mensagens do cliente
    while (1) {


        memset(buffer, 0, sizeof(buffer));
        socklen_t addrLen = sizeof(clientAddr);

        // Recebe uma mensagem na variável buffer usando a função recvfrom
        ssize_t numBytes = recvfrom(clientSockfd, buffer, sizeof(buffer), 0, (struct sockaddr*)&clientAddr, &addrLen);
        if (numBytes < 0) {
            perror("Erro na recepção da mensagem do cliente");
            exit(1);
        }

        //Verifica o conteúdo da mensagem para determinar a ação a ser tomada.
        
        // Se a mensagem for "list", imprime a lista de fontes de informação usando a função printFonteInformacaoList, 
        // envia a mensagem "OK" de confirmação para o cliente e envia a lista de fontes de informação para o cliente usando a função sendFonteInformacaoList.
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

        // Se a mensagem começar com "info", extrai o nome da fonte de informação da mensagem, 
        // envia as informações da fonte para o cliente usando a função sendFonteInfo e envia a mensagem para o cliente.
        else if (strncmp(buffer, "info", 4) == 0) {
            
            char nome_fonte[MAX_BUFFER_SIZE];
            sscanf(buffer + 5, "%s", nome_fonte); 

            sendFonteInfo(nome_fonte, sockfd, &clientAddr, fonte_informacao_list);

        }
        
        // Se a mensagem começar com "play", extrai o nome da fonte de informação da mensagem, 
        // adiciona o subscritor à lista de subscritores usando a função addSubscritor, 
        // exibe a lista de subscritores usando a função showSubscritorList e envia os dados da fonte de informação para o cliente usando a função Play_Option.
        else if (strncmp(buffer, "play", 4) == 0) {
            
            char nome_fonte[MAX_BUFFER_SIZE];
            sscanf(buffer + 5, "%s", nome_fonte);

            addSubscritor(subscritor_list,nome_fonte,&clientAddr);

            showSubscritorList(subscritor_list);

            Play_Option(nome_fonte, sockfd, &clientAddr, fonte_informacao_list);
        }

        // Repete este processo de receber mensagens e processar ações indefinidamente.
        printf("Mensagem enviada para o cliente: %s\n", buffer);


    }

    // Liberta a memória alocada para a lista de subscritores.
    free(subscritor_list);

    return NULL;

}


int main() {

    int sockfd;
    struct sockaddr_in serverAddr, fontSourceAddr, clientAddr;
    int clientSockfd;
    char buffer[MAX_BUFFER_SIZE];
    FonteInformacao fonte_informacao;

    //Aloca memória para a estrutura FonteInformacaoList usando a função malloc.
    FonteInformacaoList* fonte_informacao_list = (FonteInformacaoList*)malloc(sizeof(FonteInformacaoList));    
    
    // Cria o socket sockfd para comunicação com os clientes.
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Erro na criação do socket");
        exit(1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));


    // Configuração do endereço do servidor
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associação do socket ao endereço do servidor
    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no bind");
        exit(1);
    }

    // Cria duas threads separadas: uma para a comunicação com o cliente (clientThread) 
    // e outra para a comunicação com a fonte de informação (fontSourceThread).
    pthread_t fontSourceThread;
    pthread_t clientThread;


    // Verificação se as threads foram criadas com sucesso
    if (pthread_create(&clientThread, NULL, Cliente_Thread, fonte_informacao_list) != 0) {
        perror("Erro na criação da thread da fonte de informação");
        exit(1);
    }     

    // Verificação se as threads foram criadas com sucesso
    if (pthread_create(&fontSourceThread, NULL, fonteThread, fonte_informacao_list) != 0) {
        perror("Erro na criação da thread da fonte de informação");
        exit(1);
    }

    // Aguarda o término da thread do cliente usando a função pthread_join.
    pthread_join(fontSourceThread, NULL);
    pthread_join(clientThread, NULL);


    // Libera a memória alocada para a estrutura FonteInformacaoList
    free(fonte_informacao_list);

    // Fecha os sockets
    close(sockfd);

    return 0;
}
