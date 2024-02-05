CC=g++
CXXFLAGS= -std=c++17 -pthread

all:	clean server

server: main.cpp
	${CC} ${CXXFLAGS} -o server server.cpp main.cpp message.cpp game.cpp player.cpp

clean:
	rm -f server

