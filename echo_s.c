// file: echo_s.c
// author: F. Dolisy
// date: 11/26/2020
// purpose: CS3377
// description:
// this program runs a multithreaded server that can listen to a max of 3 user inputted ports and both tcp and udp sockets
#include <stdio.h>
#include <signal.h>
#include <string.h> //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> //close
#include <arpa/inet.h> //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include<time.h>

int total_sockets = 0;
fd_set rset;

int max(int x, int y)
{

    if (x > y)
        return x;
    else
        return y;
}

// a socket containing its file descriptor and its type(udp or tcp)
struct Socket_Info 
{

  int fd;
  int socket_type;

};
struct Socket_Info sockets[6];
//add a socket to array
void add_socket(int fd, int type)
{

  sockets[total_sockets].fd = fd;
  sockets[total_sockets].socket_type = type;
  total_sockets++;

}

//function to make a tcp and udp socket for a specific port
int make_socket_for_port(struct sockaddr_in* serverAddr)
{
  //add a listening tcp socket for desired port
  int tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
  if(tcp_socket < 0)
  {
    perror("socket failed");
    exit(EXIT_FAILURE);
  }
  
  int ret = bind(tcp_socket, (struct sockaddr*)serverAddr, sizeof(*serverAddr));
  if(ret < 0)
  {
    perror("bind failed");
    exit(EXIT_FAILURE);
  }
  if (listen(tcp_socket, 10) < 0)
  {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  add_socket(tcp_socket, SOCK_STREAM);
  
  //add a listening udp socket for desired port
  int udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
  bind(udp_socket, (struct sockaddr*)serverAddr, sizeof(*serverAddr));
  add_socket(udp_socket, SOCK_DGRAM);
  FD_SET(tcp_socket, &rset);
  FD_SET(udp_socket, &rset);

 //return the max to be usd for the select later on
  return max(tcp_socket, udp_socket);
}

//send a message to the log
void log_message(char* from, char* message, int logSock, struct sockaddr_in *logAddr)
{
  char buf[2048];
  //get time of message
  char cur_time[128];
  time_t      t;
  struct tm*  ptm;
  t = time(NULL);
  ptm = localtime(&t);
  strftime(cur_time, 128, "%d-%b-%Y %H:%M:%S", ptm);
 
  message[strlen(message) - 1] = '\0';
  memset(buf, 0, sizeof(buf));

  sprintf(buf, " %s \"%s\" was recieved from %s", cur_time, message, from);
  sendto(logSock, (const char*)buf, sizeof(buf), 0, (struct sockaddr*)logAddr, sizeof(*logAddr));
}

int main(int argc , char *argv[])
{

  int nready,len, logSock, newSocket;
  struct sockaddr_in newAddr;
  struct sockaddr_in logAddr;
  struct sockaddr_in serverAddr;
  socklen_t addr_size = sizeof(newAddr);
  char buffer[1024];
  pid_t childpid, childpid2;
  int largestMax = -1;

  
  
  //make a socket for the log connection
  logSock = socket(AF_INET, SOCK_DGRAM, 0);
  memset(&logAddr, '\0', sizeof(logAddr));
  logAddr.sin_family = AF_INET;
  logAddr.sin_addr.s_addr = INADDR_ANY;
  logAddr.sin_port = htons(9999);
  //bind(logSock, (const struct sockaddr*)&serverAddr, sizeof(serverAddr));

  FD_ZERO(&rset);

 
  //loop through and create tcp and udp sockets for all ports entered
  int port_arg;
  for(port_arg = 1; port_arg < argc; port_arg++)
  {
    int port = atoi(argv[port_arg]);
    memset(&serverAddr, '\0', sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);
    int max = make_socket_for_port(&serverAddr);
    //used for the select function
    if (max > largestMax)
      largestMax = max;
  }
  
  fd_set saved_rset;
  memcpy(&saved_rset, &rset, sizeof(fd_set));

  while(1)
  {
    //select the active socket
    int max = largestMax + 1;
    nready = select(max, &rset, NULL, NULL, NULL);
    
    int i;
    for(i = 0; i < total_sockets; ++i)
    {
      if (FD_ISSET(sockets[i].fd, &rset) == 0)
      continue;
      
      //if active socket is of type TCP
      if (sockets[i].socket_type == SOCK_STREAM)
      {
        newSocket = accept(sockets[i].fd, (struct sockaddr*)&newAddr, &addr_size);
        if(newSocket < 0)
        {
          perror("accept");
          exit(EXIT_FAILURE);
        }
    
      }
      //multithreading to handle multiple clients
      if((childpid = fork()) == -1)
      {
        printf("closing socket\n");
        close(sockets[i].fd);
      }
      else if(childpid > 0)
      {
         while(1)
         {
             //if socket is of type TCP
            if(sockets[i].socket_type == SOCK_STREAM)
            {
              //recieve and echo a message back
              bzero(buffer, sizeof(buffer));
              recv(newSocket, buffer, 1024, 0);
              printf("Message recieved from client: %s\n", buffer);
              int buf2[2024];
              memcpy(buf2, buffer, sizeof(buffer));
              log_message(inet_ntoa(newAddr.sin_addr),buf2, logSock, &logAddr);
              char* newline = '\n';
              strncat(buf2, &newline, 1);
              send(newSocket, buffer, strlen(buffer),0);
             
            }
            else //socket is of type UDP
            {
              //recieve and echo a message back
              len = sizeof(newAddr);
              bzero(buffer, sizeof(buffer));
              recvfrom(sockets[i].fd, buffer, sizeof(buffer), 0, (struct sockaddr*)&newAddr, &len);
              printf("Message recieved from client: %s", buffer);
              log_message(inet_ntoa(newAddr.sin_addr), buffer, logSock, &logAddr);
              char* newline = '\n';
              strncat(buffer, &newline, 1);
              sendto(sockets[i].fd, (const char*)buffer, sizeof(buffer), 0, (struct sockaddr*)&newAddr, sizeof(newAddr));
            }

        }

      }
    }

    // save the set of file descriptors for the select function
    memcpy(&rset, &saved_rset, sizeof(fd_set));

  }
  
  close(newSocket);
  return 0;
}