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

  /* 2. ziskani adresy serveru pomoci DNS */

  if ((server = gethostbyname(server_hostname)) == NULL)
  {
    fprintf(stderr, "ERROR: no such host as %s\n", server_hostname);
    exit(EXIT_FAILURE);
  }

  /* 3. nalezeni IP adresy serveru a inicializace struktury server_address */

  bzero((char *)&server_address, sizeof(server_address));
  server_address.sin_family = AF_INET;
  bcopy((char *)server->h_addr, (char *)&server_address.sin_addr.s_addr, server->h_length);
  server_address.sin_port = htons(port_number);

  /* Vytvoreni soketu */

  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
  {
    perror("ERROR: socket");
    exit(EXIT_FAILURE);
  }

  /* Navazani spojeni */

  if (mode == "tcp")
  {
    if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
      perror("ERROR: connect");
      exit(EXIT_FAILURE);
    }
    else if (mode == "udp")
    {
      if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
      {
        perror("ERROR: connect");
        exit(EXIT_FAILURE);
      }
    }
  }

  while (true)
  {
    /* nacteni zpravy od uzivatele */

    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE, stdin);

    /* odeslani zpravy serveru */

    if (mode == "tcp")
    {
      bytestx = send(client_socket, buf, strlen(buf), 0);
    }
    else if (mode == "udp")
    {
      bytestx = sendto(client_socket, buf, strlen(buf), 0, (struct sockaddr *)&server_address, sizeof(server_address));
    }

    if (bytestx < 0)
    {
      perror("ERROR: send");
      exit(EXIT_FAILURE);
    }

    bzero(buf, BUFSIZE);

    /* prijmuti odpovedi od serveru */
    if (mode == "tcp")
    {
      bytesrx = recv(client_socket, buf, BUFSIZE, 0);
    }
    else if (mode == "udp")
    {
      bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *)&server_address, &serverlen);
    }

    /* vypsani odpovedi */
    if (bytesrx < 0)
    {
      perror("ERROR: recv");
      exit(EXIT_FAILURE);
    }
    printf("%s", buf);

    /* ukonceni spojeni */
    if (mode == "tcp" && strcmp(buf, "BYE\n") == 0)
    {
      close(client_socket);
      break;
    }
  }

  return EXIT_SUCCESS;
}
