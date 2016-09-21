/* Introduction to Operating Systems
   
  == Problem Set 1 ==
  Simple Socket: Client
    Write a simple C program that creates and initializes a server socket. 
    Once initialized, the server should accept a client connection, close 
    the connection, and then exit.

    You should provide a way to specify the server's listening port. 
    This can be hardcoded or passed via the command line.

    *Borrowed heavily from : http://beej.us/net2/html/syscalls.html

    gcc -o server server-simple-socket.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>

#define PORT            8888              /* connection port */
#define SERVER_ADDR     "127.0.0.1"      /* localhost */
#define MAXBUF          1024

int main() {   
  
  int sockfd, new_sock_fd;
  struct sockaddr_in dest;
  char buffer[MAXBUF];
  int yes=1;

  /*---Open socket for streaming with error checking---*/
  // AF_INET - address format, internet addresses
  // SOCK_STREAM - type of socket
  // 0 - let socket choose protocol based on type
  if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
    perror("Socket");
    exit(errno);
  }

  // lose the pesky "Address already in use" error message
  if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) {
      perror("setsockopt");
      exit(1);
  } 

  dest.sin_family = AF_INET;
  dest.sin_port = htons(PORT); // system byte order -> TCP/IP order (big endian)
  dest.sin_addr.s_addr = inet_addr(SERVER_ADDR); //inet_addr puts in TCP order
  memset(&(dest.sin_zero), '\0', 8); // initialize to zeros

  /*---Associate socket with port using bind---*/
  //sockfd - the socket file descriptor
  //dest - pointer to sockaddr that has port/address info
  if (bind(sockfd, (struct sockaddr *)&dest, sizeof(struct sockaddr)) < 0 ) {
    perror("Bind");
    exit(errno);
  }

  /*---Listen for incoming connection---*/
  //sockfd - the socket file descriptor
  //backlog - number of incoming connection allowed on queue
  if (listen(sockfd, 10) < 0 ) {
    perror("Listen");
    exit(errno);
  }

  /*---Accept incoming connections---*/
  //accept returns new socket file descriptor (so original can still listen)
  //incoming - the client's socket 
  struct sockaddr_in incoming;
  uint sin_size = sizeof(struct sockaddr_in);

  new_sock_fd = accept(sockfd, (struct sockaddr *)&incoming, &sin_size);
  if (new_sock_fd < 0 ) {
    perror("Accept");
    exit(errno);
  }

  /*---Send actual data to client---*/
  // new_sock_fd - the new client socket for this connection
  char *msg = "Whatup client! This is the server!\n";
  int len, bytes_sent;

  len = strlen(msg);
  bytes_sent = send(new_sock_fd, msg, len, 0);
  if (bytes_sent < 0 ) {
    perror("Send");
    exit(errno);
  }
  fprintf(stdout, "server: got connection from %s\n",
                                           inet_ntoa(incoming.sin_addr));
  
  close(sockfd);
  close(new_sock_fd);
  
}