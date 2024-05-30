#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "include/UserConfig.h"
#include "include/syntax_highlighting.h"

UserConfig UConfig;

void DefaultConfig(){ //Sets the default config
	char *home_dir = getenv("HOME");
	char config_path[256];
	strcpy(config_path, home_dir);
    strcat(config_path, "/config.jura");
	FILE *file = fopen(config_path, "w");
	fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", 36, 33, 34, 31, 35, 32, "-");
	fclose(file);
}

void SaveConfig(struct UserConfig *config, char *filename){ //Saves the config container to the config file
	FILE *file = fopen(filename, "w");
	fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", config->Comment_Color, config->Keywords_Color, config->Types_Color, config->StringColor_, config->Number_Color, config->Match_Color, config->First_Char);
	fclose(file);
}

void LoadConfig(struct UserConfig *config, char *filename){ //Loads a config file to the config container
	FILE *file = fopen(filename, "r");
	if(file != NULL){
		fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", &config->Comment_Color, &config->Keywords_Color, &config->Types_Color, &config->StringColor_, &config->Number_Color, &config->Match_Color, config->First_Char);
		fclose(file);
	}else{ 
		DefaultConfig();
		char *home_dir = getenv("HOME");
		char config_path[256];
		strcpy(config_path, home_dir);
    	strcat(config_path, "/config.jura");
		FILE *file = fopen(config_path, "r");
		fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", &config->Comment_Color, &config->Keywords_Color, &config->Types_Color, &config->StringColor_, &config->Number_Color, &config->Match_Color, config->First_Char);
		fclose(file);
	}
}

int SyntaxToColor(int hl){
	switch(hl){
		case Comment:
		case MultiLineComment: return UConfig.Comment_Color;
		case Keywords: return UConfig.Keywords_Color;
		case Types: return UConfig.Types_Color;
		case StringColor: return UConfig.StringColor_;
		case Number: return UConfig.Number_Color;
		case Match: return UConfig.Match_Color;
		default: return 37;
	}
}