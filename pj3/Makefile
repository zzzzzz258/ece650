TARGETS=ringmaster player
FLAGS = --std=c++98 -Wall -Werror -pedantic -ggdb3

all: $(TARGETS)
clean:
	rm -f $(TARGETS)

ringmaster: ringmaster.o socket_lib.o
	g++ -o ringmaster ringmaster.o socket_lib.o

player: player.o socket_lib.o
	g++ -o player player.o socket_lib.o

%o: %.c
	g++ $(FLAGS) -c $<

socket_lib.o: socket_lib.cpp socket_lib.h
	g++ $(FLAGS) -c socket_lib.cpp
