CC = gcc

compile: include/buffer.h include/config.h include/editor_operations.h include/find.h include/input.h include/io.h include/jura.h include/line_operations.h include/output.h include/syntax_highlighting.h include/syntax.h include/terminal.h include/UserConfig.h
	sudo mkdir -p /usr/include/jura
	sudo cp include/* /usr/include/jura

install: UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c
	$(CC) UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c -o jura
	sudo mv jura /usr/bin/jura

develop: UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c
	$(CC) -g -Og UserConfig.c config.c syntax.c terminal.c syntax_highlighting.c line_operations.c editor_operations.c io.c find.c buffer.c output.c input.c jura.c -Wall -Wextra -Werror -pedantic -o jura