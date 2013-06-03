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
 * */
 
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)


#define MAX_PACKET 1250
#define PORTA_SRV 2023 // porta TCP do servidor

enum erros {WSTARTUP, ABRESOCK, BIND, ACCEPT, LISTEN,RECEIVE}; 

void TrataErro(SOCKET, int);

int main(int argc, char* argv[])
{
    int ClienteSocket;
    int ServidorSocket = 0;

    struct sockaddr_in ServidorEndereco;
    struct sockaddr_in ClienteEndereco;
    
    int ServidorEnderecoTamanho = sizeof(ServidorEndereco);
    socklen_t* ClienteEnderecoTamanho = (socklen_t*)sizeof(ClienteEndereco);
    
    char recvbuf[MAX_PACKET];
	int result;

    // Cria o socket na familia AF_INET (Internet) e do tipo TCP (SOCK_STREAM)
    ServidorSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ServidorSocket == INVALID_SOCKET)
    {
        TrataErro(ServidorSocket, ABRESOCK);
    }

    // Define domínio, IP e porta a receber dados
    ServidorEndereco.sin_family = AF_INET;
    ServidorEndereco.sin_addr.s_addr = htonl(INADDR_ANY); // recebe de qualquer IP
    ServidorEndereco.sin_port = htons(PORTA_SRV);

    // Associa socket com estrutura addr_serv
    result = bind(ServidorSocket, (struct sockaddr *)&ServidorEndereco, ServidorEnderecoTamanho);
    if (result != 0)
    {
        TrataErro(ServidorSocket, BIND);
    }

    // Coloca socket em estado de escuta para as conexoes na porta especificada permite ateh 8 conexoes simultaneas
    result = listen(ServidorSocket, 8);
    if (result != 0)
    {
        TrataErro(ServidorSocket, LISTEN);
    }

    // permite conexoes entrantes utilizarem o socket
    ClienteSocket = accept(ServidorSocket, (struct sockaddr *)&ClienteEndereco, ClienteEnderecoTamanho);
    //extern int accept (int __fd, __SOCKADDR_ARG __addr, socklen_t *__restrict __addr_len);
    if (ClienteSocket < 0)
    {
        TrataErro(ServidorSocket, ACCEPT);
    }

    // fica esperando chegar mensagem
    while(1)
    {
		result = recv(ClienteSocket, recvbuf, MAX_PACKET, 0);
        if (result < 0)
        {
            close(ClienteSocket);
            TrataErro(ServidorSocket, RECEIVE);
        }

        // mostra na tela
        if (strcmp((const char *)&recvbuf, "q") == 0)
        {
            break;
        }
        else
        {
            printf("%s\n", recvbuf);
        }
    }

    
    // FIM DO PROGRAMA
    printf("Fim da conexao\n");
    
    close(ServidorSocket);
    close(ClienteSocket);
    
    exit(1);
}

void TrataErro(SOCKET socket, int tipoerro)
{
	char tipo[20];

	switch (tipoerro)
	{
		case WSTARTUP:
			strcpy(tipo, "Windows Startup");
			break;
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
	
    printf("Erro no %s", tipo);
    close(socket);
    
    exit(1);
}
