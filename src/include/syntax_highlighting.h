#ifndef SYNTAX_HIGHLIGHTING_H
#define SYNTAX_HIGHLIGHTING_H

#include "config.h"

typedef enum Highlight{ //things that need to be highlighted
	Normal = 0,
	Comment,
	MultiLineComment,
	Keywords,
	Types,
	StringColor,
	Number,
	Match
}Highlight;

int is_seperator(int c);

void UpdateSyntax(eline *line);

void SelectSyntaxHighlight();

#endif