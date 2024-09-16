CC = gcc

install: jura.c 
	$(CC) jura.c -o jura -std=c99
	sudo mv jura /usr/bin/jura`

develop: jura.c
	$(CC) -g -Og jura.c -o jura -Wall -Wextra -pedantic -std=c99