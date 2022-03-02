CC = gcc
CFLAGS = -Wall -Wextra -Werror -pedantic -g
new: clean hinfosvc

hinfosvc: server.c
	$(CC) $(CFLAGS) server.c -o hinfosvc

clean:
	rm -f *.o
	rm -f hinfosvc
