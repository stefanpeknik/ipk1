#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <signal.h>

#define BUFSIZE 1024

int client_socket, port_number, bytestx, bytesrx;
socklen_t serverlen;
const char *server_hostname;
struct hostent *server;
struct sockaddr_in server_address;
char buf[BUFSIZE];

std::string host;
int port = 0;
std::string mode;
int main(int argc, char *argv[])
{

  return EXIT_SUCCESS;
}
