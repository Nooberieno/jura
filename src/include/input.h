#ifndef INPUT_H
#define INPUT_H

extern char *Prompt(char *prompt, void(*callback)(char *, int));

extern void MoveCursor(int key);

extern void ProcessKeypress();

#endif