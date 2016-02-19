CC = gcc
CFLAGS = -Wall -Werror
DFLAGS = -g -DCSE320
BIN = utfconverter

SRC = $(wildcard *.c)

all: $(BIN)

$(BIN): $(SRC)
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean

debug:
	$(CC) $(CFLAGS) $(DFLAGS) utfconverter.c -o utfconverter

clean:
	rm -f *.o $(BIN)

