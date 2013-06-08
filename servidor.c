/* UFRGS
 * INF01151 - SISTEMAS OPERACIONAIS II N
 * SEMESTRE 2013/1
 * TRABALHO 3 : COMUNICAÇÃO INTER-PROCESSOS USANDO SOCKETS UNIX
 * 
 * André Figueiredo
 * Bruno Fritzen
 * 
 * Programa Servidor
 * 
 * Para compilar utilizar "gcc servidor.c -o servidor -lpthread
 * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
/*#include <unistd.h>*/
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define PORTA_SRV 2100					// porta TCP do servidor
#define SALA_CHAT_MAX_USERS 2			// qtd de usuarios maximo na sala de chat
#define MSG_MAX_SIZE 50

enum erros {ABRESOCK, BIND, ACCEPT, LISTEN, RECEIVE}; 

void TrataErro(SOCKET, int);
void *trataCliente(void* cliSocket);


// Listas finitas de Clientes, limitadas pelo tam da sala
int sockClientes[SALA_CHAT_MAX_USERS];
struct sockaddr_in addrCliente[SALA_CHAT_MAX_USERS];
socklen_t addrClienteTamanho[SALA_CHAT_MAX_USERS];
int currSock = 0;
int i;

pthread_mutex_t lock;

int main(int argc, char* argv[])
{
    //int sockCliente;
    int sockServidor = 0;

    struct sockaddr_in addrServidor;

    int result;
    
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
		printf("\ninicializacao do mutex falhou\n");
		return 1;
    }
    // Cria o socket na familia AF_INET (Internet) e do tipo TCP (SOCK_STREAM)
    sockServidor = socket(AF_INET, SOCK_STREAM, 0);
    if (sockServidor == INVALID_SOCKET)
    {
        TrataErro(sockServidor, ABRESOCK);
    }

    // Define domínio, IP e porta a receber dados
    addrServidor.sin_family = AF_INET;
    addrServidor.sin_addr.s_addr = htonl(INADDR_ANY); // recebe de qualquer IP
    addrServidor.sin_port = htons(PORTA_SRV);

    // Associa socket com estrutura addr_serv
    result = bind(sockServidor, (struct sockaddr *)&addrServidor, sizeof(addrServidor));
    if (result != 0)
    {
        TrataErro(sockServidor, BIND);
    }

    // Coloca socket em estado de escuta para as conexoes na porta especificada permite ateh 8 conexoes simultaneas
    result = listen(sockServidor, SALA_CHAT_MAX_USERS);
    if (result != 0)
    {
        TrataErro(sockServidor, LISTEN);
    }

	printf("\nServidor de chat inicializado com sucesso.\n");

    // permite conexoes entrantes utilizarem o socket
    while(1)
    {
		addrClienteTamanho[currSock] = (socklen_t)sizeof(addrCliente[currSock]);
		sockClientes[currSock] = accept(sockServidor, (struct sockaddr *)&addrCliente[currSock], &addrClienteTamanho[currSock]);
		
		if (sockClientes[currSock] < 0)
		{
        	TrataErro(sockServidor, ACCEPT);
    	}
    	else
    	{
			pthread_t pth;
			pthread_create(&pth, NULL, (void*)&trataCliente, (void*)sockClientes[currSock]);
			currSock++;
    	}	
    }

    // FIM DO PROGRAMA
    printf("Fim da conexao\n");
    
    close(sockServidor);
    
    for (i = 0; i < currSock; i++){
		close(sockClientes[i]);
	}
    pthread_mutex_destroy(&lock);
    exit(1);
}

void TrataErro(SOCKET socket, int tipoerro)
{
	char tipo[20];

	switch (tipoerro)
	{
		case ABRESOCK:
			strcpy(tipo, "Open Socket");
			break;
		case BIND:
			strcpy(tipo, "Bind");
			break;
		case ACCEPT:
			strcpy(tipo, "Accept");
			break;
		case LISTEN:
			strcpy(tipo, "Listen");
			break;
		case RECEIVE:
			strcpy(tipo, "Receive");
			break;
		default:
			strcpy(tipo, "Indefinido. Verificar");
			break;
	}
	
    printf("Erro no %s\n", tipo);
    close(socket);
    
    exit(1);
}









void *trataCliente(void* cliSocket) {
	int sockCliente = (int) cliSocket;
	int result;
	char recvbuf[MSG_MAX_SIZE];

		// fica esperando chegar mensagem
    	while(1)
    	{
			result = recv(sockCliente, recvbuf, MSG_MAX_SIZE, 0);
			if (result < 0)
			{
				close(sockCliente);
				printf("Deu erro RECEIVE. SocketClient (%i) fechado. result = %i\n", sockCliente, result);
				//TrataErro(sockServidor, RECEIVE);
			}

        	// mostra na tela
        	if (strcmp((const char *)&recvbuf, "/sair") == 0) {
            	close(sockCliente);
        	}
        	else {
				pthread_mutex_lock(&lock);
				int resultBroadcast = 0;

				for (i = 0; i <= currSock; i++) {
					if (send(sockClientes[i], (const char *)&recvbuf, sizeof(recvbuf), 0) >= 0){
						resultBroadcast++;
					}
				}
				
				if (resultBroadcast < 0)
					printf("ATENCAO : Mensagem nao foi enviada para os clientes: %s\n", recvbuf);
				else if (resultBroadcast == currSock)
					printf("Mensagem enviada para todos os clientes: %s\n", recvbuf);
				else
					printf("Mensagem enviada para %d dos %d clientes: %s\n", resultBroadcast+1, currSock, recvbuf);
				pthread_mutex_unlock(&lock);
        	}
    	}

	//the function must return something - NULL will do
	return NULL;
}
