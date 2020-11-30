// file: echo_s.c
// author: F. Dolisy
// date: 11/26/2020
// purpose: CS3377
// description:
// this program runs a tcp client that echos whatever is sent, the port number is also user input
#include <stdio.h> // perror, printf
#include <stdlib.h> // exit, atoi
#include <unistd.h> // write, read, close
#include <arpa/inet.h> // sockaddr_in, AF_INET, SOCK_STREAM, INADDR_ANY, socket etc...
#include <string.h> // strlen, memset
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 

int main(int argc, char const *argv[]) 
{

  int serverFd,len;
  struct sockaddr_in server;
  int port = atoi(argv[1]);
  char chr[1024];
  //create a tcp socket to communicate
  serverFd = socket(AF_INET, SOCK_STREAM, 0);
  if (serverFd < 0) {
    perror("Cannot create socket");
    exit(1);
  }
  
  memset(&server, '\0', sizeof(server));
  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(port);
  len = sizeof(server);
  
  if (connect(serverFd, (struct sockaddr *)&server, len) < 0) {
    perror("Cannot connect to server");
    exit(2);
  }
  bind(serverFd, (struct sockaddr*)&server, sizeof(server));
  while(1)
  {
   
    scanf("%[^\n]%*c", &chr);
    send(serverFd, chr, strlen(chr), 0);
    printf(" MY SEND OFF %s\n", chr);
    char reciv[2024];
    //memset(recv, 0, sizeof(recv));
    bzero(reciv, sizeof(reciv));
    recv(serverFd, reciv, 2024, 0);
    printf("%s", reciv);
    printf("\n");
  }
  
  close(serverFd);
  return 0;
}