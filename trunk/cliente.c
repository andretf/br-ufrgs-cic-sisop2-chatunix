/* UFRGS
 * INF01151 - SISTEMAS OPERACIONAIS II N
 * SEMESTRE 2013/1
 * TRABALHO 3 : COMUNICAÇÃO INTER-PROCESSOS USANDO SOCKETS UNIX
 * 
 * André Figueiredo
 * Bruno Fritzen
 * 
 * Programa Cliente
 *
 * Para executar o programa cliente utilizar ./cliente NUM. O parametro NUM sera somado ao valor inicial da porta para comunicacao
 * para poder utilizar varios clientes na mesma maquina. Por exemplo, rodar ./cliente 1 em um terminal e ./cliente 2 em outro e
 * assim por diante.
 * */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define PORTA_CLI 2001 // porta TCP do cliente
#define PORTA_SRV 2100 // porta TCP do servidor
#define STR_IPSERVIDOR "127.0.0.1"
#define MSG_MAX_SIZE 50

//#define STR_IPSERVIDOR "192.168.0.146"

void *trataMsgRecebida(void* servSocket);

int i;

int main(int argc, char* argv[])
{
  SOCKET s;
  struct sockaddr_in  s_cli, s_serv;
  char ch;
  int porta = 0;
	char nome[MSG_MAX_SIZE] = "";

  if (argc == 2)
    porta = atoi(argv[1]);

  // abre socket TCP
  if ((s = socket(AF_INET, SOCK_STREAM, 0))==INVALID_SOCKET)
  {
    printf("Erro iniciando socket\n");
    return(0);
  }

  // seta informacoes IP/Porta locais
  s_cli.sin_family = AF_INET;
  s_cli.sin_addr.s_addr = htonl(INADDR_ANY);
  s_cli.sin_port = htons(PORTA_CLI + porta);

  // associa configuracoes locais com socket
  if ((bind(s, (struct sockaddr *)&s_cli, sizeof(s_cli))) != 0)
  {
    printf("erro no bind\n");
    close(s);
    return(0);
  }

  // seta informacoes IP/Porta do servidor remoto
  s_serv.sin_family = AF_INET;
  s_serv.sin_addr.s_addr = inet_addr(STR_IPSERVIDOR);
  s_serv.sin_port = htons(PORTA_SRV);

  // connecta socket aberto no cliente com o servidor
  if(connect(s, (struct sockaddr*)&s_serv, sizeof(s_serv)) != 0)
  {
    //printf("erro na conexao - %d\n", WSAGetLastError());
    printf("erro na conexao");
    close(s);
    exit(1);
  }

	puts("\nBem vindo a sala de chat.");
	puts("-------------------------");
	puts("Use os comandos disponiveis no chat:");
	puts("   \\sair\tSair da sala.");
	puts("\nDigite seu nome e pressione enter.\nApos isso voce entrara na sala e podera interagir.");
	printf("Nome: ");

	for (i = 0; (i<MSG_MAX_SIZE) &&  (ch = getchar()) != '\n'; i++ )
		nome[i] = (char)ch;
	nome[i] = '\0';


	puts("\n---------------------------------");

  // Cara entrou na sala
  /*char m[1];
  strcat(m, "> ");
  strcat(m, nome);
  strcat(m, " entrou na sala.");
  send(s, (const char *)&m, sizeof(m), 0);
*/
send(s, "toc toc", 7, 0);
  // recebe do teclado e envia ao servidor
  char str[MSG_MAX_SIZE*2+10];
  char msg[MSG_MAX_SIZE];

  pthread_t pth;
  pthread_create(&pth, NULL, (void*)&trataMsgRecebida, (void*)s);  

  strcat(nome, " diz: ");
  while(1)
  {
	strcpy(str, "");
	strcat(str, nome);

    for(i=0; (i < MSG_MAX_SIZE) &&  (ch = getchar()) != '\n'; i++)
		msg[i] = (char)ch;
    msg[i] = '\0';
    
    strcat(str, msg);
        
    if ((send(s, (const char *)&str, strlen(str), 0)) < 0)
    {
      printf("erro na transmissão da msg\n");
      close(s);
      return 0;
    }
    
    if(strcmp((const char *)&str, "/sair") == 0) {
      break;
    }
  }

  // Cara saiu na sala
  /*char ml[1];
  strcat(ml, "> ");
  strcat(ml, nome);
  strcat(ml, " saiu da sala.");
  send(s, (const char *)&ml, sizeof(ml), 0);
*/
  // fecha socket e termina programa
  printf("Fim da conexao\n");
  close(s);

  return 0;
}

void *trataMsgRecebida(void* servSocket) {

	int sockServidor = (int) servSocket;
	int result;
	char recvbuf[MSG_MAX_SIZE];
	// fica esperando chegar mensagem
    	while(1) {
			result = recv(sockServidor, recvbuf, MSG_MAX_SIZE, 0);
			if (result < 0) {
				close(sockServidor);
			printf("Deu erro RECEIVE. SocketServidor (%i) fechado. result = %i\n", sockServidor, result);
			}

        	// mostra na tela
        	if (strcmp((const char *)&recvbuf, "/sair") == 0) {
            		break;
        	} else {
            		printf(">> %s\n", recvbuf);
        	}
    	}
	//the function must return something - NULL will do
	return NULL;
}
