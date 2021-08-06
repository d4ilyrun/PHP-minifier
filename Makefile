CC = gcc
CFLAGS = -g -std=c99

TEST_FILE_LINK = "https://raw.githubusercontent.com/marcocesarato/PHP-Minifier/master/src/Minifier.php"
TEST_FILE = example.php

all: main
main: build dl_file

build: parser.o
	$(CC) -fsanitize=address $^ -o parser

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

dl_file:
	curl -L $(TEST_FILE_LINK) -o $(TEST_FILE)

test: build dl_file
	./parser $(TEST_FILE) $(shell cat $(TEST_FILE) | wc -c)

debug: CFLAGS+=-DDEBUG
debug: all

clean:
	rm -rf parser.o parser *.php

.PHONY: all main clean test build

