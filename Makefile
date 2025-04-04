CC = gcc
CFLAGS = -Wall -pthread

all: server game

server: server.c
	$(CC) $(CFLAGS) server.c -o server

game: game.c
	$(CC) $(CFLAGS) game.c -o game

clean:
	rm -f server game *.o server.log

