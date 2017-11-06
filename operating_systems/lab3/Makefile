CC=g++
CFLAGS=-std=c++11 -Werror -Wall
LDFLAGS= -lglut -lGLU -lGL -lrt -pthread
TARG_FILES = $(shell find . -name "conn_*.cpp") 
ALL_TARGS = $(TARG_FILES:./conn%.cpp=pong%)

all: $(ALL_TARGS)

pong_%: conn_%.o main.o render.o client.o host.o
	$(CC) $< main.o render.o client.o host.o -o $@ $(LDFLAGS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean: 
	rm -f *.o *.e
