#include <string.h>
#include <stdlib.h>

#include <jura/find.h>
#include <jura/config.h>
#include <jura/jura.h>
#include <jura/syntax_highlighting.h>
#include <jura/input.h>
#include <jura/output.h>
#include <jura/line_operations.h>

void FindCallback(char *query, int key){ //Find every sequence that matches the query and move between them
	static int last_match = -1;
	static int direction = 1;
	static int saved_hl_line;
	static char *saved_hl = NULL;
	if(saved_hl){
		memcpy(config.line[saved_hl_line].hl, saved_hl, config.line[saved_hl_line].rendersize);
		free(saved_hl);
		saved_hl = NULL;
	}
	if(key == '\r' || key == '\x1b'){
		last_match = -1;
		direction = 1;
		return;
	}
	else if(key == ARROW_RIGHT || ARROW_DOWN) direction = 1;
	else if(key == ARROW_LEFT || ARROW_UP) direction = -1;
	else{
		last_match = -1;
		direction = 1;
	}
	if(last_match == -1) direction = 1;
	int current = last_match;
	int i;
	for(i = 0; i < config.numlines; i++){
		current += direction;
		if(current == -1) current = config.numlines - 1;
		else if(current == config.numlines) current = 0;
		eline *line = &config.line[current];
		char *match = strstr(line->render, query);
		if(match){
			last_match = current;
			config.y = current;
			config.x = LineRenderxToX(line, match - line->render);
			config.offline = config.numlines;
			saved_hl_line = current;
			saved_hl = malloc(line->rendersize);
			memcpy(saved_hl, line->hl, line->rendersize);
			memset(&line->hl[match - line->render], Match, strlen(query));
			break;
		}
	}
}

void Find(){ //Allow the user to type a sequence they want to search for an trigger the FindCallback function with that sequence
	int saved_cx = config.x;
	int saved_cy = config.y;
	int saved_coloff = config.coloff;
	int saved_offline = config.offline;
	char *query = Prompt("Search: %s (ESC|Arrow keys|Enter)", FindCallback); 
	if(query){
		free(query); 
	}else { 
		config.x = saved_cx;
		config.y = saved_cy;
		config.coloff = saved_coloff;
		config.offline = saved_offline;
	}
}

void Golf(){ //Go to the line the user typed in as a query
	char *query = Prompt("Go to line: %s (ESC to cancel)", NULL);
	if(query == NULL) return;
	int i = atoi(query);
	if(i == 0 || i > config.numlines){
		SetStatusMessage("Not a valid line number");
		free(query);
		return;
	}else if(i > 0 && i < config.numlines){
		config.y = i - 1;
		config.x = 0;
	}
	free(query);
}