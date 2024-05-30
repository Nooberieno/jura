CC = gcc

install: jura.c 
	$(CC) UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c -o jura
	sudo mv jura /usr/bin/jura
	rm UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c

develop: jura.c
	$(CC) -g -Og UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c -Wall -Wextra -Werror -pedantic -o jura