#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "include/jura.h"
#include "include/config.h"
#include "include/terminal.h"
#include "include/UserConfig.h"
#include "include/output.h"
#include "include/io.h"
#include "include/input.h"

struct Config config;

void init(){ //Intialize the editor config
	config.x = 0;
	config.y = 0;
	config.renderx = 0;
	config.offline = 0;
	config.coloff = 0;
	config.numlines = 0;
	config.line = NULL;
	config.mod = 0;
	config.filename = NULL;
	config.statusmsg[0] = '\0';
	config.statusmsg_time = 0;
	config.syntax = NULL;
	if(getWindowSize(&config.screenlines, &config.screencols) == -1) die("getWindowSize");
	config.screenlines -= 2;
}

int main(int argc, char *argv[]){ //main program loop
	enableRawMode();
	init();
	char *home_dir = getenv("HOME");
	char config_path[256];
	strcpy(config_path, home_dir);
	strcat(config_path, "/config.jura");
	if(argc == 3 && strcmp(argv[1], "setconfig") == 0){
		if(strcmp(argv[2], "default") == 0){
			DefaultConfig();
		}else{
			printf("Attempting to open config file: %s\n\r", argv[2]);
			LoadConfig(&UConfig, argv[2]);
			SaveConfig(&UConfig, argv[2]);
		}
		printf("Config updated succesfully\n\r");
		return 0;
	}
	LoadConfig(&UConfig, config_path); 
	if (argc == 2 && (strcmp(argv[1], "editconfig") == 0)){ 
		Open(config_path); 
	}else if(argc == 2 && strcmp(argv[1], "makeconfig") == 0){ 
		FILE *file = fopen(config_path, "r"); 
		if(file != NULL){ 
			printf("Config already exists\n\r");
		}else{ 
			printf("Made config file\n\r");
		}
		return 0; 
	}else if(argc >= 2){ 
		Open(argv[1]); 
	}
	SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-G = go to line"); 
	while (1){
	struct winsize ws;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1 && ws.ws_col != config.screencols && ws.ws_row != config.screenlines){
		if(getWindowSize(&config.screenlines, &config.screencols) == -1) die("getWindowSize"); 
		config.screenlines -= 2;
	}
		RefreshScreen();
		ProcessKeypress();
	}
    return 0;
}

