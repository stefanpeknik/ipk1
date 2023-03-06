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
  /* 1. test vstupnich parametru: */

  for (int i = 1; i < argc; i++)
  {
    std::string arg = argv[i];

    if (arg == "-h" && i + 1 < argc)
    {
      host = argv[++i];
    }
    else if (arg == "-p" && i + 1 < argc)
    {
      std::string port_str = argv[++i];
      char *p;
      port = strtol(port_str.c_str(), &p, 10);
      if (*p)
      {
        std::cerr << "Invalid port number: " << port_str << std::endl;
        return EXIT_FAILURE;
      }
    }
    else if (arg == "-m" && i + 1 < argc)
    {
      mode = argv[++i];
      if (mode != "tcp" && mode != "udp")
      {
        std::cerr << "Invalid mode: " << mode << std::endl;
        return EXIT_FAILURE;
      }
    }
    else
    {
      std::cerr << "Unknown argument: " << arg << std::endl;
      return EXIT_FAILURE;
    }
  }

  if (host.empty() || port == 0 || mode.empty())
  {
    fprintf(stderr, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
    return 1;
  }

  server_hostname = host.c_str();
  port_number = port;


  return EXIT_SUCCESS;
}
