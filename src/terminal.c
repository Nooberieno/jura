#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>

#include <jura/config.h>
#include <jura/jura.h>
#include <jura/terminal.h>

void die(const char *s){ //if an error occurers, reset the terminal settings, sow the error and exit the program
	write(STDOUT_FILENO, "\x1b[2J", 4); 
	write(STDOUT_FILENO, "\x1b[H", 3); 
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.og_terminal) == -1){ 
		perror("tcsetattr");
		exit(1);
	}
	perror(s); 
	exit(1); 
}

void disableRawMode(){ //get the terminal out of rawmode
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.og_terminal) == -1) die("tcsetattr");
}

void enableRawMode(){ //set the terminal into raw mode(allow for jura to register keypresses without the user pressing enter)
	if (tcgetattr(STDIN_FILENO, &config.og_terminal) == -1) die("tcgetattr"); 
	atexit(disableRawMode); 
	struct termios raw = config.og_terminal; 
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); 
	raw.c_oflag &= ~(OPOST); 
	raw.c_cflag |= (CS8); 
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); 
	raw.c_cc[VMIN] = 0;
	raw.c_cc[VTIME] = 1;
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); 
}

int ReadKey(){ //read the keypresses from the users and check for certain modifier keys
	int nread;
	char c;
	while((nread=read(STDIN_FILENO, &c, 1)) != 1){
		if(nread == -1 && errno != EAGAIN) die("read");
	}
	if(c != '\x1b') return c;
	char seq[3];
	if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
	if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
	if(seq[0] != '[') return '\x1b';
	if(!(seq[1] >= '0' && seq[1] <= '9')){
		switch (seq[1])
		{
		case 'A': return ARROW_UP;
		case 'B': return ARROW_DOWN;
		case 'C': return ARROW_RIGHT;
		case 'D': return ARROW_LEFT;
		}
	}else{
		if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
		if(seq[2] != '~') return '\x1b';
		switch (seq[1])
		{
		case '3': return DEL_KEY;
		case '5': return PAGE_UP;
		case '6': return PAGE_DOWN;
		}
	}
	return '\x1b';
}

int getCursorPosition(int *lines, int *cols){ //use the ANSI escape sequence to get the cursor position in the terminal
	char buf[32];
	unsigned int i = 0;
	if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
	while(i<sizeof(buf) - 1){ 
		if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
		if (buf[1] == 'R') break; 
		i++;
	}
	buf[i] = '\0'; 
	if(buf[0] != '\x1b' || buf[1] != '[') return -1; 
	if(sscanf(&buf[2], "%d;%d", lines, cols) != 2) return -1; 
	return 0;
}


int getWindowSize(int *lines, int *cols){ //try to get the size of the window in the terminal using IOCTL or if that fails, use the cursor position to get it
	struct winsize ws;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){ 
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1; 
		return getCursorPosition(lines, cols);
	} else { 
		*cols = ws.ws_col;
		*lines = ws.ws_row;
		return 0;
	}
}