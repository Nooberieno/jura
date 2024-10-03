#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

#include <jura/input.h>
#include <jura/output.h>
#include <jura/terminal.h>
#include <jura/jura.h>
#include <jura/config.h>
#include <jura/find.h>
#include <jura/editor_operations.h>
#include <jura/io.h>
#include <jura/line_operations.h>

char *Prompt(char *prompt, void(*callback)(char *, int)){ //Enter in a prompt and if a callback function is wanted, call it with parameters
	size_t bufsize = 128;
	char *buf = malloc(bufsize);
	size_t buflen = 0;
	buf[0] = '\0';
	while(1){
		SetStatusMessage(prompt, buf);
		RefreshScreen();
		int c = ReadKey();
		if(c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
			if(buflen != 0) buf[--buflen] = '\0';
		}else if(c == '\x1b'){
			SetStatusMessage("");
			if(callback) callback(buf, c);
			free(buf);
			return NULL;
		}else if(c == '\r' && buflen != 0){
			SetStatusMessage("");
			if(callback) callback(buf, c);
			return buf;
		}else if(!iscntrl(c) && c < 128){
			if(buflen == bufsize -1){
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}
		if(callback) callback(buf, c);
	}
}

void MoveCursor(int key){ //Move the cursor according to the user input
	eline *line = (config.y >= config.numlines) ? NULL : &config.line[config.y];
	switch (key){
		case ARROW_LEFT:
		if(config.x != 0){
		config.x--;
		}else if(config.y > 0){ 
			config.y--;
			config.x = config.line[config.y].size;
		}
		break;
		case ARROW_RIGHT:
		if(line && config.x < line->size){ 
		config.x++;
		}else if(line && config.x == line->size){
			config.y++;
			config.x = 0;
		}
		break;
		case ARROW_UP:
		if(config.y != 0){ 
		config.y--;
		}
		break;
		case ARROW_DOWN:
		if(config.y < config.numlines){
		config.y++;
		}
		break;
	}
	line = (config.y >= config.numlines) ? NULL : &config.line[config.y];
	int linelen = line ? line->size : 0;
	if(config.x > linelen){
		config.x = linelen;
	}
}

void CleanConfig(){ //Free the config memory
	if(config.line){
		for(int i = 0; i < config.numlines; i++){
			FreeLine(&config.line[i]);
		}
		free(config.line);
	}
	free(config.filename);
	if(config.syntax){
		free(config.syntax);
	}
}

void ProcessKeypress(){ //Tell jura what to do when certain keys are pressed
	static int quit_times = JuraQuitTimes;
	int c = ReadKey();
	switch (c){
	case '\r': 
		InsertNewLine();
		break;
	case CTRL_KEY('q'):
		if(config.mod && quit_times > 0){ 
			SetStatusMessage("WARNING! File has unsaved changes." "Press Ctrl-Q %d more times to quit", quit_times); 
			quit_times--; 
			return;
		}
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3); 
		CleanConfig();
		exit(0); 
		break;
	case CTRL_KEY('s'): 
		Save(); 
		SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-G = go to line"); 
		break;
	case CTRL_KEY('f'): 
		Find(); 
		SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-G = go to line"); 
		break;
	case CTRL_KEY('g'):
		Golf(); 
		break;
	case BACKSPACE:
	case CTRL_KEY('h'):
	case DEL_KEY: 
		if(c == DEL_KEY) MoveCursor(ARROW_RIGHT); 
		RemoveChar(); 
		break;
	case PAGE_UP:
	case PAGE_DOWN:
		{
			if(c == PAGE_UP){
				config.y = config.offline;
			}else if(c == PAGE_DOWN){
				config.y = config.offline + config.screenlines - 1;
			}
			int times = config.screencols; 
			while(times--)
				MoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN); 
		}
		break;
	case ARROW_UP:
	case ARROW_DOWN:
	case ARROW_LEFT:
	case ARROW_RIGHT:
		MoveCursor(c); 
		break;
	case CTRL_KEY('l'):
	case '\x1b': 
		break; 
	default:
		InsertChar(c); 
		break;
	}
	quit_times = JuraQuitTimes;
}
