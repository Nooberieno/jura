#ifndef INPUT_H
#define INPUT_H

char *Prompt(char *prompt, void(*callback)(char *, int));

void MoveCursor(int key);

void ProcessKeypress();

void CleanConfig();

#endif