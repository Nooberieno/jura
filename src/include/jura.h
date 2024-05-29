#ifndef JURA_H
#define JURA_H

#define CurrentJuraVersion "5.0"
#define JuraTabStop 4
#define JuraQuitTimes 2
#define CTRL_KEY(k) (k & 0x1f)

typedef enum Key{
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	PAGE_UP,
	PAGE_DOWN
}Key;

extern void init();

extern int main(int argc, char *argv[]);

#endif