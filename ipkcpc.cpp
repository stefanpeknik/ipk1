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
#include <sys/time.h>

#define BUFSIZE 1024

/* globalni promenne */
int client_socket, port_number, bytestx, bytesrx;
socklen_t serverlen;
const char *server_hostname;
struct hostent *server;
struct sockaddr_in server_address;
char buf[BUFSIZE];

std::string mode;

/* Zachytavac signalu ctrl+C (SIGINT) */
void signalHandler(int signum)
{
  if (mode == "tcp")
  {
    /* odeslani ukonceni spojeni */
    strcpy(buf, "BYE\n");
    bytestx = send(client_socket, buf, strlen(buf), 0);
    /* vypsani konecne odpovedi */
    if (bytesrx < 0)
    {
      perror("ERROR: recv");
      exit(EXIT_FAILURE);
    }
    printf("%s", buf);

    /* ukonceni spojeni */
    close(client_socket);
  }

  exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
  /* 1. test vstupnich parametru: */

  if (argc != 7)
  {
    fprintf(stderr, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
    return EXIT_FAILURE;
  }

  for (int i = 1; i < argc; i++)
  {
    std::string arg = argv[i];

    if (arg == "-h" && i + 1 < argc)
    {
      server_hostname = argv[++i];
    }
    else if (arg == "-p" && i + 1 < argc)
    {
      std::string port_str = argv[++i];
      char *p;
      port_number = strtol(port_str.c_str(), &p, 10);
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

  if (strcmp(server_hostname, "") == 0 || port_number == 0 || mode.empty())
  {
    fprintf(stderr, "Usage: %s -h <host> -p <port> -m <mode>\n", argv[0]);
    return 1;
  }

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

  if (mode == "tcp")
  {
    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) <= 0)
    {
      perror("ERROR: socket");
      exit(EXIT_FAILURE);
    }
  }
  else if (mode == "udp")
  {
    if ((client_socket = socket(AF_INET, SOCK_DGRAM, 0)) <= 0)
    {
      perror("ERROR: socket");
      exit(EXIT_FAILURE);
    }

    /* Nastaveni timeoutu */
    struct timeval timeout;
    timeout.tv_sec = 10;
    timeout.tv_usec = 0;

    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&timeout, sizeof(timeout)) < 0)
    {
      perror("ERROR: setsockopt");
      exit(EXIT_FAILURE);
    }
  }

  /* Navazani spojeni */

  if (mode == "tcp")
  {
    if (connect(client_socket, (const struct sockaddr *)&server_address, sizeof(server_address)) != 0)
    {
      perror("ERROR: connect");
      exit(EXIT_FAILURE);
    }
  }

  /* Zachyceni ctrl+C */
  bzero(buf, BUFSIZE);
  signal(SIGINT, signalHandler);

  while (true)
  {
    /* nacteni zpravy od uzivatele */

    bzero(buf, BUFSIZE);
    fgets(buf, BUFSIZE - 1, stdin);

    /* odeslani zpravy serveru */

    if (mode == "tcp")
    {
      size_t msg_len = strlen(buf);
      if (msg_len > 0 && buf[msg_len - 1] != '\n')
      {
        buf[msg_len] = '\n';
        buf[msg_len + 1] = '\0';
      }
      bytestx = send(client_socket, buf, strlen(buf), 0);
      if (bytestx < 0)
        perror("ERROR in sendto");
      continue;
    }
    else if (mode == "udp")
    {
      int msg_len = strlen(buf);
      if (msg_len > 0 && buf[msg_len - 1] == '\n')
      {
        buf[msg_len--] = '\0';
      }
      if (msg_len > 255)
      {
        msg_len = 255;
      }
      memmove(&buf[2], buf, msg_len);
      buf[0] = '\0';
      buf[1] = (char)msg_len;
      serverlen = sizeof(server_address);
      printf("Sending %d bytes to server\n", msg_len + 2);
      bytestx = sendto(client_socket, buf, msg_len + 2, 0, (struct sockaddr *)&server_address, serverlen);
      if (bytestx < 0)
        perror("ERROR: sendto");
      continue;
    }

    bzero(buf, BUFSIZE);

    /* prijmuti odpovedi od serveru */
    if (mode == "tcp")
    {
      bytesrx = recv(client_socket, buf, BUFSIZE, 0);
      if (bytesrx < 0)
        perror("ERROR in recvfrom");
      continue;
    }
    else if (mode == "udp")
    {
      bytesrx = recvfrom(client_socket, buf, BUFSIZE, 0, (struct sockaddr *)&server_address, &serverlen);
      if (bytesrx < 0)
      {
        if (errno == EAGAIN || errno == EWOULDBLOCK)
        {
          perror("ERROR: timeout occurred");
        }
        else
        {
          perror("ERROR: recvfrom");
        }
        continue;
      }
    }

    /* vypsani odpovedi */
    if (mode == "udp")
    {
      /* konstrola status code */
      if (buf[1] == '\1')
      {
        printf("ERR:%s\n", std::string(buf + 3, (int)buf[2]).c_str());
      }
      else if (buf[1] == '\0')
      {
        printf("OK:%s\n", std::string(buf + 3, (int)buf[2]).c_str());
      }
    }
    else
    {
      printf("%s", buf);
    }

    /* ukonceni spojeni */
    if (mode == "tcp" && strcmp(buf, "BYE\n") == 0)
    {
      close(client_socket);
      break;
    }
    bzero(buf, BUFSIZE);
  }

  return EXIT_SUCCESS;
}
