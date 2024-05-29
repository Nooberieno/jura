#ifndef JURA_H
#define JURA_H

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#define CurrentJuraVersion "5.0"
#define JuraTabStop 4
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
};

#endif