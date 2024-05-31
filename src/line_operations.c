#include <string.h>
#include <stdlib.h>

#include <jura/syntax_highlighting.h>
#include <jura/config.h>
#include <jura/jura.h>

int LineXToRenderx(eline *line, int x){ //Calculate which X coordinate we need to render the cursor at from the X coordinate
	int renderx = 0;
	int j;
	for(j = 0; j < x; j++){
		if(line->chars[j] == '\t') renderx += (JuraTabStop - 1) - (renderx % JuraTabStop);
		renderx++;
	}
	return renderx;
}

int LineRenderxToX(eline *line, int renderx){ //Convert the X coordinate that is being rendered to the X coordinate for the cursor
	int cur_renderx = 0;
	int x;
	for(x =0; x < line->size; x++){
		if(cur_renderx < renderx){
			if(line->chars[x] == '\t') cur_renderx += (JuraTabStop - 1) - (cur_renderx % JuraTabStop);
			cur_renderx++;
		}
	}
	return x;
}

void UpdateLine(eline *line){ //Update how the line gets rendered
	int tabs = 0;
	int j;
	for(j = 0; j < line->size; j++)
		if(line->chars[j] == '\t') tabs++;
	free(line->render);
	line->render = malloc(line->size + tabs*(JuraTabStop -1) + 1);
	int idx = 0;
	for(j = 0; j < line->size; j++){
		if(line->chars[j] == '\t'){
			line->render[idx++] = ' ';
			while(idx % JuraTabStop != 0) line->render[idx++] = ' ';
		}else{
			line->render[idx++] = line->chars[j];
		}
	}
	line->render[idx] = '\0';
	line->rendersize = idx;
	UpdateSyntax(line);
}

void InsertLine(int at, char *s, size_t len){ //Insert a Line
	if(at < 0 || at > config.numlines) return; 
	config.line = realloc(config.line, sizeof(eline) * (config.numlines + 1)); 
	memmove(&config.line[at + 1], &config.line[at], sizeof(eline) * (config.numlines - at));
	for(int j = at + 1; j <= config.numlines; j++) config.line[j].idx++; 
	config.line[at].idx = at;
	config.line[at].size = len;
	config.line[at].chars = malloc(len + 1);
	memcpy(config.line[at].chars, s, len);
	config.line[at].chars[len] = '\0';
	config.line[at].rendersize = 0;
	config.line[at].render = NULL;
	config.line[at].hl = NULL;
	config.line[at].hl_open_comment = 0;
	UpdateLine(&config.line[at]);
	config.numlines++; 
	config.mod++; 
}

void FreeLine(eline *line){ //free memory for a line
	free(line->render);
	free(line->chars);
	free(line->hl);
}

void RemoveLine(int at){ //Remove a line
	if(at < 0 || at >= config.numlines) return;
	FreeLine(&config.line[at]);
	memmove(&config.line[at], &config.line[at + 1], sizeof(eline) * (config.numlines - at - 1));
	for(int j = at; j < config.numlines - 1; j++) config.line[j].idx--;
	config.numlines--; 
	config.mod++; 
}

void LineInsertChar(eline *line, int at, int c){ //Insert a character into a line
	if(at < 0 || at > line->size) at = line->size; 
	line ->chars = realloc(line->chars, line->size + 2); 
	memmove(&line->chars[at + 1], &line->chars[at], line->size - at + 1); 
	line->size++;
	line->chars[at] = c; 
	UpdateLine(line); 
	config.mod++; 
}

void LineAppendString(eline *line, char *s, size_t len){ //Add a string of characters to the end of a line
	line->chars = realloc(line->chars, line->size + len + 1); 
	memcpy(&line->chars[line->size], s, len); 
	line->size +=len; 
	line->chars[line->size] = '\0';
	UpdateLine(line);
	config.mod++;
}

void LineRemoveChar(eline *line, int at){ //Remove a character from the line
	if(at < 0 || at >= line->size) return; 
	memmove(&line->chars[at], &line->chars[at + 1], line->size - at); 
	line->size--; 
	UpdateLine(line); 
	config.mod++; 
}