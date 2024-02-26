CFLAGS = -g -Wall -std=c++14 -pthread
CC = g++
PROG_NAME=http_client

objects = http_client.o

.PHONY : all
all: $(PROG_NAME)

$(PROG_NAME): $(objects)
	$(CC) $(CFLAGS) -o $(PROG_NAME) $(objects)

http_client.o: http_client.cpp
	$(CC) $(CFLAGS) -c http_client.cpp

.PHONY : clean
clean: 
	rm $(PROG_NAME) $(objects)