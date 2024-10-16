#ifndef CONFIG_H
#define CONFIG_H

#include <termios.h>
#include <time.h>

typedef struct editorline{ //contains everything needed for a line in the editor
	int idx;
	int size;
	int rendersize;
	char *chars;
	char *render;
	unsigned char *hl;
	int hl_open_comment;
}eline;

typedef struct Syntax{ //track everything needed to change the syntax color
	char *filetype;
	char **filematch;
	char **keywords;
	char *singleline_comment_start;
	char *multiline_comment_start;
	char *multiline_comment_end;
	int flags;
}Syntax;

typedef struct Config{ //contains everything needed for the editor
	int x, y;
	int renderx;
	int offline;
	int coloff;
	int screenlines;
	int screencols;
	int numlines;
	eline *line;
	int mod;
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
	struct Syntax *syntax;
	struct termios og_terminal;
}Config;

Config config;

#endif