#include "include/editor_operations.h"
#include "include/line_operations.h"

void InsertChar(int c){ //Insert a character into a line and update the X coordinate for the cursor
	if(config.y == config.numlines) InsertLine(config.numlines, "", 0);
	LineInsertChar(&config.line[config.y], config.x, c);
	config.x++;
}

void InsertNewLine(){ //Insert a newline and update the internal cursor coordinates
	if(config.x == 0) InsertLine(config.y, "", 0);
	else{
		eline *line = &config.line[config.y];
		InsertLine(config.y + 1, &line->chars[config.x], line->size - config.x);
		line = &config.line[config.y];
		line->size = config.x;
		line->chars[line->size] = '\0';
		UpdateLine(line);
	}
	config.y++;
	config.x = 0;
}

void RemoveChar(){ // Remove a character from the file
	if(config.y == config.numlines) return;
	if(config.x == 0 && config.y == 0) return;
	eline *line = &config.line[config.y];
	if(config.x > 0){
		LineRemoveChar(line, config.x - 1);
		config.x--;
	}else{
		config.x = config.line[config.y - 1].size;
		LineAppendString(&config.line[config.y - 1], line->chars, line->size);
		RemoveLine(config.y);
		config.y--;
	}
}
