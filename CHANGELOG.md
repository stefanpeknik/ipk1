# Changelog

## Limitations
Program can be run only on linux-based operating systems.

UDP messages are limited to size of 255 characters. If user inputs text larger then 255 characters, the first 255 are used as input and sent to a server.
If the client does not recieve response from a server in 10 seconds, it times out.

TCP messages are limited to 1022, as the static buffer is of a size of 1024 characters, but the 1023rd character is reserved for a character `'\n'` and the 1024th character is reserved for a character `'\0'`.

## Git history
- Create CHANGELOG.md
- Update README.md
- add header
- change comments to english
- refactoring
- fix bug
- close socket in both modes when sigint
- add timeout for UDP
- fix potential error handling
- trim newline char from msg if udp
- limit max msg length in UDP
- fix udp ERR msg print
- rename variable
- move signal handler
- remove unwanted close of connection
- add comments
- fix args handling
- edit makefile
- Update .gitignore
- quick fix
- move signal handler call
- move signal handler
- add comment
- refactor and cleanup
- refactor
- change cpp version to compile with
- edit readme
- create handler for ctrl c
- setup communication with server
- setup connection
- parse args
- declare vars
- create dafult body with includes
- create makefile
- init commit
- Initial commit