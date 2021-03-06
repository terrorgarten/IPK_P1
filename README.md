# hinfosvc - minimalistic server for gathering info about remote/local system
C language socket server for linux working with HTTP GET requests. Listens on the defined port for HTTP request and sends back coresponding answers. Is easily terminable via CTRL+C from the terminal or kill -9. Default port is set to 10000 but can be changed via a program argument, see Running the program lower.

## Prerequisites
* make 4.3
* gcc 11.2.0

## Requests
* /hostname - detects system hostname from /proc/sys/kernel/hostname
* /load - meassures current system load over the time of one second using /proc/stat data
* /cpu-name detects CPU name from /proc/cpuinfo
Returns 500 Bad request upon recieving unvalid requests

## Instalation
Uses make from the root directory:
```
$ make 
```

## Running the program
Launch the server with default port - 10000
```
$ ./hinfosvc
```

Launch the server with defined port - eg. 3000: Use the first program argument to specify numeric value of the port
```
$ ./hinfosvc 3000
```

## Usage
On the host machine:
```
$ make
$ ./hinfosvc 4000
```

On the client machine:
```
$ GET http://[REMOTE MACHINE IP ADDRESS]:4000/load 
43%
$ GET http://[REMOTE MACHINE IP ADDRESS]:4000/cpu-name
Intel Core i5-7200 CPU @ 2.50GHz
$ GET http://[REMOTE MACHINE IP ADDRESS]:4000/hostname
remote_machine_hostname
```

wget or curl can be used in place of GET command.

Try running the test_ipk.sh script for reference outputs on the local machine.


## Implementation details
The program loads necessary socket and address settings and then loops indefinitely until a request is accepted. Upon acceptance, the program parses the requests and returns response. Then awaits another request in the loop. The program continues to handle requests indefinitely until shut down via CTRL+C/kill on the host machine.

