/* Introduction to Operating Systems
   
  == Problem Set 1 ==
  Simple Socket: Client
    Write a simple C program that creates, initializes, and connects a client
    socket to a server socket. You should provide a way to specify the 
    connecting server address and port. This can be hardcoded or passed via 
    the command line.

    *Borrowed heavily from : http://beej.us/net2/html/syscalls.html

    gcc -o client client-simple-socket.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT            8888             /* connection port */
#define SERVER_ADDR     "127.0.0.1"     /* localhost */
#define MAXBUF          1024

int main() {   

  int sockfd;
  struct sockaddr_in dest;
  char buffer[MAXBUF];

  /*---Open socket for streaming---*/
  // AF_INET - address family
  // SOCK_STREAM - type of socket
  // 0 - let socket choose protocol based on type
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Socket");
    exit(errno);
  }

  /*---Initialize server address/port struct---*/
  memset(&dest, 0, sizeof(dest));
  dest.sin_family = AF_INET;
  dest.sin_port = htons(PORT);
  if ( inet_aton(SERVER_ADDR, &dest.sin_addr) == 0 ) {
    perror(SERVER_ADDR);
    exit(errno);
  }

  /*---Connect to server---*/
  if ( connect(sockfd, (struct sockaddr*)&dest, sizeof(dest)) != 0 ) {
    perror("Connect ");
    exit(errno);
  }

  /*---Get "Hello?"---*/
  // set buffer to all zeros
  memset(buffer, 0, MAXBUF);
  recv(sockfd, buffer, sizeof(buffer), 0);
  printf("%s", buffer);

  /*---Clean up---*/
  close(sockfd);
  return 0;
}

