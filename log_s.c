 // file: echo_s.c
// author: F. Dolisy
// date: 11/26/2020
// purpose: CS3377
// description:
// this program runs a udp multithreaded server that sends all text recived to a file called echo.log
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 

#define PORT	9999
#define MAXLINE 2024 

// Driver code 
int main(int argc, char *argv[]) 
{
	  int sockfd; 
    char buffer[MAXLINE]; 
    struct sockaddr_in servaddr, cliaddr; 
    pid_t childpid;
    //create/open a log file
    FILE *f;
    f = fopen("echo.log", "a+"); // a+ (create + append) option will allow appending which is useful in a log file
    if (f == NULL) 
    {
      perror("COULD NOT OPEN FILE");
      return -1;
    } 
   
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    memset(&servaddr, 0, sizeof(servaddr)); 
    memset(&cliaddr, 0, sizeof(cliaddr)); 
      
    // Filling server information 
    servaddr.sin_family    = AF_INET; // IPv4 
    servaddr.sin_addr.s_addr = INADDR_ANY; 
    servaddr.sin_port = htons(PORT); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&servaddr,  
            sizeof(servaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
      
    int len, n; 
    len = sizeof(cliaddr);
    // fork to handle multiple clients
    if((childpid = fork()) == -1)
    {
        printf("closing socket\n");
        close(sockfd);
    }
    else if(childpid > 0)
    {
        while(1)
        {
              //send to log file
              fopen("echo.log", "a+");
              n = recvfrom(sockfd, (char *)buffer, MAXLINE,  0, ( struct sockaddr *) &cliaddr, &len); 
              buffer[n] = '\n'; 
              fputs(buffer, f); 
              fprintf(f, "\n");
              fclose(f);
        } 
    } 
   
    fclose(f);
    return 0; 
} 