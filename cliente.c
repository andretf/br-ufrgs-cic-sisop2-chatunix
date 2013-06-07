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

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define	SOCKET	int
#define INVALID_SOCKET  ((SOCKET)~0)
#define PORTA_CLI 2001 // porta TCP do cliente
#define PORTA_SRV 2000 // porta TCP do servidor
#define STR_IPSERVIDOR "127.0.0.1"
#define MSG_MAX_SIZE 50

//#define STR_IPSERVIDOR "192.168.0.146"

int main(int argc, char* argv[])
{
  SOCKET s;
  struct sockaddr_in  s_cli, s_serv;
  int porta = 0;

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

#if 0
  // envia mensagem de conexao - aprimorar para dar IP e porta
  if ((send(s, "Conectado\n", 11,0)) == SOCKET_ERROR);
  {
    printf("erro na transmissão - %d\n", WSAGetLastError());
    closesocket(s);
    return 0;
  }
#endif

  // recebe do teclado e envia ao servidor
  char str[MSG_MAX_SIZE];
  char ch;
  int i;

  while(1)
  {
    printf("$ ");

    for(i=0; (i<MSG_MAX_SIZE) &&  (ch = getchar()) != '\n'; i++ )
      str[i] = (char)ch;
    str[i] = '\0';
    
    if ((send(s, (const char *)&str, sizeof(str),0)) < 0)
    {
      printf("erro na transmissão\n");
      close(s);
      return 0;
    }
    if(strcmp((const char *)&str, "q")==0)
      break;
  }

  // fecha socket e termina programa
  printf("Fim da conexao\n");
  close(s);
  return 0;
}
