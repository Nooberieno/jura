#ifndef TERMINAL_H
#define TERMINAL_H

extern void die(const char *s);

extern void disableRawMode();

extern void enableRawMode();

extern int ReadKey();

extern int getCursorPosition(int *lines, int *cols);

extern int getWindowSize(int *lines, int *cols);

#endif