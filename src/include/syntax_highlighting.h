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

extern int is_seperator(int c);

extern void UpdateSyntax(eline *line);

extern void SelectSyntaxHighlight();

#endif