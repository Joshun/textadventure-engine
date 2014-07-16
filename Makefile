CC = gcc -g -Wall -Werror

all: adventure

adventure: adventure.c player.o data.o runtime.o
	$(CC) -o $@ $^

player.o: player.c data.o runtime.o
	$(CC) -c $<

data.o: data.c runtime.o
	$(CC) -c $<

runtime.o: runtime.c
	$(CC) -c $<

clean:
	rm -f adventure *.o
