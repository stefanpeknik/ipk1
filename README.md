# IPKCPC Client Application

## Introduction
This application is a client for the IPKCPC protocol. It allows communication with a server using either TCP or UDP.

## Usage
To start the IPKCPC client application, open the terminal and navigate to the directory where the application is downloaded. Then, run the following command:

```ipkcpc -h <host> -p <port> -m <mode>```

Replace `<host>` with the IPv4 address of the server, `<port>` with the server port, and `<mode>` with either `tcp` or `udp`.
The order of options is not fixxed.
Program can also be compiled and run using a Makefile:
- `make runudp` to compile the program and execute `./ipkcpc -h 0.0.0.0 -p 2023 -m udp`, 
- `make runtcp` to compile the program and execute `./ipkcpc -h 0.0.0.0 -p 2023 -m tcp`.

## Important Notes
The IPKCPC client application is a command-line interface (CLI) tool and does not have a graphical user interface (GUI).
Make sure to use the correct IPv4 address and port number for the server when starting the client.
The client only supports communication with a server that implements the IPKCPC protocol.
The client runs only on linux-based operating systems.

## UDP connection
User can start typing expressions immidiately after start of the program. If server response is error (might happen because of invalid given expression or internal server problem) error message is printed and user can try to send another expression.

#### Limits
UDP messages are limited to size of 255 characters. If user inputs text larger then 255 characters, the first 255 are used as input and sent to a server.
If the client does not recieve response from a server in 10 seconds, it times out.

#### SIGINT handling
If the program recieves SIGINT signal, it closes socket and ends.

## TCP connection
User has to start the communication with a server by saying `HELLO` and can start sending expressions after server response in a way such as `SOLVE (+ 1 2)`. In case of not well-formed message or some internal server problem the server responds with a `BYE` which indicates end of a connection, so the client closes as well.

#### Limits
TCP messages are limited to 1022, as the static buffer is of a size of 1024 characters, but the 1023rd character is reserved for a character `'\n'` and the 1024th character is reserved for a character `'\0'`.

#### SIGINT handling
If the program recieves SIGINT signal, it sends a `BYE` message to the server, awaits server response and after it closes socket and ends.

## Testing
Testing was done manually against a referencial server on wsl2 with Ubuntu distro.
How to testing:
- open terminal,
- start referencial server,
- open another terminal,
- start this client,
- start sending messages and validate responses.

Example of such a test for UDP connection:
- `./ipkpd -h 0.0.0.0 -p 2023 -m udp`,
- `./ipkcpc -h 0.0.0.0 -p 2023 -m udp`, 

```
(+ 1 2)
OK:3
(+)
ERR:<err msg>
(+ 1 2 3)
OK:6
```

Example of such a test for TCP connection:
- `./ipkpd -h 0.0.0.0 -p 2023 -m tcp`,
- `./ipkcpc -h 0.0.0.0 -p 2023 -m tcp`, 

```
HELLO
HELLO
SOLVE (+ 1 2)
RESULT 3
BYE
BYE
```

## Sources
Parts of code were taken from / inspired by code in [IPK-Projekty](https://git.fit.vutbr.cz/NESFIT/IPK-Projekty) repository in stubs written in C language.