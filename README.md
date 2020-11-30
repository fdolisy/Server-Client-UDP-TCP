# Server Client UDP/TCP

This program creates a simple udp/tcp server that listens to multiple ports and echos back whatever the input is sent.
The input is also sent to a log file that keeps track of all messages sent and their IP addresses. Can be
tested with the included client file or with commands.

## Installation

Unzip and extract all files

## Executing
First,
Compile the log file -- gcc log_s.c -o log_s
Execute the log program -- ./log_s

Second,
If you want the server to be the created server
Compile the server -- gcc echo_s.c -o echo_s
Exectue the server program -- ./echo_s port1 [port2 port3]
port# is the desiered port number to listen to, port2 and port3 are optional

If you want the server to be a pre built UDP server
nc -e /bin/cat -k -l port
where port is a port number you want others to connect to

If you want the server to be a pre built TCP server
nc -e /bin/cat -k -u -l port
where port is a port number you want others to connect to

Third,
If you want to test with the created TCP client
Compile the client -- gcc echo_c.c -o echo_c
Execture the client -- ./echo_c port
port is the one of the port numbers used in the server

If you want to test with a pre made TCP client
 -- nc 127.0.0.1 port
where port is a port number inputted when running the server

If you want to test with a pre made UDP client
 -- nc -u 127.0.0.1 port
where port is a port number inputted when running the server

## Author
FDolisy, fcd180001@utdallas.edu
