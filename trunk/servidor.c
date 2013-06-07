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
#define MAX_PACKET 50
#define PORTA_SRV 2000					// porta TCP do servidor
#define SALA_CHAT_MAX_USERS 2			// qtd de usuarios maximo na sala de chat
#define MSG_MAX_SIZE 50

enum erros {ABRESOCK, BIND, ACCEPT, LISTEN, RECEIVE}; 

void TrataErro(SOCKET, int);
void *trataCliente(void* cliSocket);

int main(int argc, char* argv[])
{
    int sockCliente;
    int sockServidor = 0;

    struct sockaddr_in addrServidor;
    struct sockaddr_in addrCliente;
    
    int addrServidorTamanho = sizeof(addrServidor);
    socklen_t addrClienteTamanho = sizeof(addrCliente);

    int result;

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
    result = bind(sockServidor, (struct sockaddr *)&addrServidor, addrServidorTamanho);
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

    // permite conexoes entrantes utilizarem o socket
    while(1) {
	sockCliente = accept(sockServidor, (struct sockaddr *)&addrCliente, &addrClienteTamanho);
	if (sockCliente < 0) {
		continue;
        	//TrataErro(sockServidor, ACCEPT);
    	} else {
		printf(">SocketClient Accepted\n");
		pthread_t pth;
		pthread_create(&pth, NULL, (void*)&trataCliente, (void*)sockCliente);
    	}
    }

    // FIM DO PROGRAMA
    printf("Fim da conexao\n");
    
    close(sockServidor);
    close(sockCliente);
    
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
    	while(1) {
		result = recv(sockCliente, recvbuf, MSG_MAX_SIZE, 0);
		if (result < 0) {
            		close(sockCliente);
			printf("Deu erro RECEIVE. SocketClient (%i) fechado. result = %i\n", sockCliente, result);
            		//TrataErro(sockServidor, RECEIVE);
        	}

        	// mostra na tela
        	if (strcmp((const char *)&recvbuf, "q") == 0) {
            		break;
        	} else {
            		printf("%s\n", recvbuf);
        	}
    	}

	//the function must return something - NULL will do
	return NULL;

}
