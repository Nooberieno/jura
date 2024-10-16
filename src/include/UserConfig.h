#ifndef UCONFIG_H
#define UCONFIG_H

#define FirstCharLength 2

typedef struct UserConfig{ //Contains everything that can be configured
	int Comment_Color;
	int Keywords_Color;
	int Types_Color;
	int StringColor_;
	int Number_Color;
	int Match_Color;
	char First_Char[FirstCharLength];
}UserConfig;

UserConfig UConfig;

int SyntaxToColor(int hl);

void DefaultConfig();

void SaveConfig(struct UserConfig *uconfig, char *filename);

void LoadConfig(struct UserConfig *UConfig, char *filename);

#endif