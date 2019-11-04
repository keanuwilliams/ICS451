# Makefile for Homework

PROG = server
SRC  = server.c

CC     = gcc
CFLAGS = -ansi -pedentic-errors -Wall
LDFLAGS = 

OBJ = $(SRC: .c = .o)

.PHONY: all

all: $(PROG)

$(PROG): $(OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean

clean:
	rm -f $(PROG)

