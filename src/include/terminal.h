#ifndef TERMINAL_H
#define TERMINAL_H

void die(const char *s);

void disableRawMode();

void enableRawMode();

int ReadKey();

int getCursorPosition(int *lines, int *cols);

int getWindowSize(int *lines, int *cols);

#endif