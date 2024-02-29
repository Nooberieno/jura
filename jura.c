#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

/* defines */

#define JURA_VERSION "2.0"
#define JURA_TAB_STOP 8
#define JURA_QUIT_TIMES 1
#define CTRL_KEY(k) ((k) & 0x1f)

enum Key{
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	PAGE_UP,
	PAGE_DOWN
};

/* data */

typedef struct eline{
	int size;
	int rendersize;
	char *chars;
	char *render;
}eline;

struct Config{
	int x, y;
	int renderx;
	int offline;
	int coloff;
	int screenlines;
	int screencols;
	int numlines;
	eline *line;
	int mod;
	char *filename;
	char statusmsg[80];
	time_t statusmsg_time;
	struct termios og_terminal;
};

struct Config config;

/* prototypes */

void SetStatusMessage(const char *fmt, ...);
void RefreshScreen();
char *Prompt(char *prompt, void (*callback)(char *, int));

/* terminal */

void die(const char *s){
	write(STDOUT_FILENO, "\x1b[2J", 4);
	write(STDOUT_FILENO, "\x1b[H", 3);
	perror(s);
	exit(1);
}

void disableRawMode(){
	if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &config.og_terminal) == -1)
		die("tcsetattr");
}

void enableRawMode(){
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

int ReadKey(){
	int nread;
	char c;
	while((nread=read(STDIN_FILENO, &c, 1)) != 1){
		if(nread == -1 && errno != EAGAIN) die("read");
	}
	if(c == '\x1b'){
		char seq[3];
		if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
		if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
		if(seq[0] == '['){
			if(seq[1] >= '0' && seq[1] <= '9'){
				if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
				if(seq[2] == '~'){
					switch(seq[1]){
						case '3': return DEL_KEY;
						case '5': return PAGE_UP;
						case '6': return PAGE_DOWN;
					}
				}
			}else{
				switch(seq[1]){
					case 'A': return ARROW_UP;
					case 'B': return ARROW_DOWN;
					case 'C': return ARROW_RIGHT;
					case 'D': return ARROW_LEFT;
				}
			}
		}
		return '\x1b';
	}else{
		return c;
	}
}

int getCursorPosition(int lines, int *cols){
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

int getWindowSize(int *lines, int *cols){
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

/* line operations */

int LineXToRenderx(eline *line, int x){
	int renderx = 0;
	int j;
	for(j = 0; j < x; j++){
		if(line->chars[j] == '\t')
			renderx += (JURA_TAB_STOP - 1) - (renderx % JURA_TAB_STOP);
		renderx++;
	}
	return renderx;
}

int LineRenderxToX(eline *line, int renderx){
	int cur_renderx = 0;
	int x;
	for(x = 0; x < line->size; x++){
		if(line->chars[x] == '\t')
			cur_renderx += (JURA_TAB_STOP -1) - (cur_renderx % JURA_TAB_STOP);
		cur_renderx++;
		if(cur_renderx > renderx) return x;
	}
	return x;
}

void UpdateLine(eline *line){
	int tabs = 0;
	int j;
	for(j = 0; j < line->size; j++)
		if(line->chars[j] == '\t') tabs++;
	free(line->render);
	line->render = malloc(line->size + tabs*(JURA_TAB_STOP - 1) + 1);
	int idx = 0;
	for(j = 0; j < line->size; j++){
		if(line->chars[j] == '\t'){
			line->render[idx++] = ' ';
			while(idx % JURA_TAB_STOP != 0) line->render[idx++] = ' ';
		}else{
			line->render[idx++] = line->chars[j];
		}
	}
	line->render[idx] = '\0';
	line->rendersize = idx;
}

void InsertLine(int at, char *s, size_t len){
	if(at < 0 || at > config.numlines) return;
	config.line = realloc(config.line, sizeof(eline) * (config.numlines + 1));
	memmove(&config.line[at + 1], &config.line[at], sizeof(eline) * (config.numlines - at));
	config.line[at].size = len;
	config.line[at].chars = malloc(len + 1);
	memcpy(config.line[at].chars, s, len);
	config.line[at].chars[len] = '\0';
	config.line[at].rendersize = 0;
	config.line[at].render = NULL;
	UpdateLine(&config.line[at]);
	config.numlines++;
	config.mod++;
}

void FreeLine(eline *line){
	free(line->render);
	free(line->chars);
}

void RemoveLine(int at){
	if(at < 0 || at >= config.numlines) return;
	FreeLine(&config.line[at]);
	memmove(&config.line[at], &config.line[at + 1], sizeof(eline) * (config.numlines - at - 1));
	config.numlines--;
	config.mod++;
}

void LineInsertChar(eline *line, int at, int c){
	if(at < 0 || at > line->size) at = line->size;
	line ->chars = realloc(line->chars, line->size + 2);
	memmove(&line->chars[at + 1], &line->chars[at], line->size - at + 1);
	line->size++;
	line->chars[at] = c;
	UpdateLine(line);
	config.mod++;
}

void LineAppendString(eline *line, char *s, size_t len){
	line->chars = realloc(line->chars, line->size + len + 1);
	memcpy(&line->chars[line->size], s, len);
	line->size +=len;
	line->chars[line->size] = '\0';
	UpdateLine(line);
	config.mod++;
}

void LineRemoveChar(eline *line, int at){
	if(at < 0 || at >= line->size) return;
	memmove(&line->chars[at], &line->chars[at + 1], line->size - at);
	line->size--;
	UpdateLine(line);
	config.mod++;
}

/*  editor operations */

void InsertChar(int c){
	if(config.y == config.numlines){
		InsertLine(config.numlines, "", 0);
	}
	LineInsertChar(&config.line[config.y], config.x, c);
	config.x++;
}

void InsertNewline(){
	if(config.x==0){
		InsertLine(config.y, "", 0);
	}else{
		eline *line = &config.line[config.y];
		InsertLine(config.y + 1, &line->chars[config.x], line->size - config.x);
		line = &config.line[config.y];
		line->size = config.x;
		line->chars[line->size] = '\0';
		UpdateLine(line);
	}
	config.y++;
	config.x = 0;
}

void RemoveChar(){
	if(config.y == config.numlines) return;
	if(config.x == 0 && config.y == 0) return;
	eline *line = &config.line[config.y];
	if(config.x > 0){
		LineRemoveChar(line, config.x - 1);
		config.x--;
	}else{
		config.x = config.line[config.y - 1].size;
		LineAppendString(&config.line[config.y - 1], line->chars, line->size);
		RemoveLine(config.y);
		config.y--;
	}
}

/* file I/O */

char *LinesToString(int *buflen){
	int totlen = 0;
	int j;
	for(j = 0; j < config.numlines; j++)
		totlen += config.line[j].size + 1;
	*buflen = totlen;
	char *buf = malloc(totlen);
	char *p = buf;
	for(j = 0; j < config.numlines; j++){
		memcpy(p, config.line[j].chars, config.line[j].size);
		p += config.line[j].size;
		*p = '\n';
		p++;
	}
	return buf;
}

void Open(char *filename){
	free(config.filename);
	config.filename = strdup(filename);
	FILE *fp = fopen(filename, "r");
	if(!fp) die("fopen");
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lijnen;
	while((lijnen = getline(&line, &linecap, fp)) != -1){
		while(lijnen > 0 && (line[lijnen - 1] == '\n' || line[lijnen - 1] == '\r'))
			lijnen--;
		InsertLine(config.numlines, line, lijnen);
	}
	free(line);
	fclose(fp);
	config.mod = 0;
}

void Save(){
	if(config.filename == NULL){
		config.filename = Prompt("Save as: %s (ESC to cancel)", NULL);
		if(config.filename == NULL){
			SetStatusMessage("Save aborted");
			return;
		}
	}
	int len;
	char *buf = LinesToString(&len);
	int fd = open(config.filename, O_RDWR | O_CREAT, 0644);
	if(fd != -1){
		if(ftruncate(fd, len) != -1){
			if(write(fd, buf, len) == len){
				close(fd);
				free(buf);
				config.mod = 0;
				SetStatusMessage("%d bytes written to disk", len);
				return;
			}
		}
		close(fd);
	}
	free(buf);
	SetStatusMessage("Can't save I/O error %s", strerror(errno));
}

/* find */

void FindCallback(char *query, int key){
	static int last_match = -1;
	static int direction = 1;
	if(key == '\r' || key == '\x1b'){
		last_match = -1;
		direction = 1;
		return;
	}else if(key == ARROW_RIGHT || ARROW_DOWN){
		direction = 1;
	}else if(key == ARROW_LEFT || ARROW_UP){
		direction = -1;
	}else{
		last_match = -1;
		direction = 1;
	}
	if(last_match == -1) direction = 1;
	int current = last_match;
	int i;
	for(i = 0; i < config.numlines; i++){
		current += direction;
		if(current == -1) current = config.numlines - 1;
		else if(current == config.numlines) current = 0;
		eline *line = &config.line[current];
		char *match = strstr(line->render, query);
		if(match){
			last_match = current;
			config.y = current;
			config.x = LineRenderxToX(line, match - line->render);
			config.offline = config.numlines;
			break;
		}
	}
}

void Find(){
	int saved_cx = config.x;
	int saved_cy = config.y;
	int saved_coloff = config.coloff;
	int saved_offline = config.offline;
	char *query = Prompt("Search: %s (ESC to cancel | Use arrow keys to search matches)", FindCallback);
	if(query){
		free(query);
	}else {
		config.x = saved_cx;
		config.y = saved_cy;
		config.coloff = saved_coloff;
		config.offline = saved_offline;
	}
}

/* append buffer */

struct buffer{
	char *b;
	int len;
};
#define StartBuffer {NULL, 0}

void AttachBuffer(struct buffer *buff, const char *s, int len){
	char *new = realloc(buff->b, buff->len + len);
	if(new == NULL) return;
	memcpy(&new[buff->len], s, len);
	buff->b = new;
	buff->len += len;
}

void FreeBuffer(struct buffer *buff){
	free(buff->b);
}

/* output */

void Scroll(){
	config.renderx = 0;
	if(config.y < config.numlines){
		config.renderx = LineXToRenderx(&config.line[config.y], config.x);
	}
	if(config.y < config.offline){
		config.offline = config.y;
	}
	if(config.y >= config.offline + config.screenlines){
		config.offline = config.y - config.screenlines + 1;
	}
	if(config.renderx < config.coloff){
		config.coloff = config.renderx;
	}
	if(config.renderx >= config.coloff + config.screencols){
		config.coloff = config.renderx - config.screencols + 1;
	}
}

void DrawLines(struct buffer *buff){
	int y;
	for (y = 0; y < config.screenlines; y++){
		int fileline = y + config.offline;
		if(fileline >= config.numlines){
		if(config.numlines == 0 && y == config.screenlines/3){
			char welcome[80];
			int welkom = snprintf(welcome, sizeof(welcome), "jura  -- version %s", JURA_VERSION);
			if(welkom > config.screencols) welkom = config.screencols;
			int padding = (config.screencols - welkom) / 2;
			if(padding){
				AttachBuffer(buff, "~", 1);
				padding--;
			}
			while(padding--) AttachBuffer(buff, " ", 1);
			AttachBuffer(buff, welcome, welkom);
		}else {
		AttachBuffer(buff, "~", 1);
		}
		}else{
			int len = config.line[fileline].rendersize - config.coloff;
			if(len < 0) len = 0;
			if(len > config.screencols) len = config.screencols;
			char *c = &config.line[fileline].render[config.coloff];
			int j;
			for(j = 0; j < len; j++){
				if(isdigit(c[j])){
					AttachBuffer(buff, "\x1b[31m", 5);
					AttachBuffer(buff, &c[j], 1);
					AttachBuffer(buff, "\x1b[39m", 5);
				} else{
					AttachBuffer(buff, &c[j], 1);
				}
			}
		}
		AttachBuffer(buff, "\x1b[K", 3);
			AttachBuffer(buff, "\r\n", 2);
	}
}

void DrawStatusBar(struct buffer *buff){
	AttachBuffer(buff, "\x1b[7m", 4);
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), "%.20s - %d lines %s", config.filename ? config.filename : "[No Name]", config.numlines, config.mod ? "(modified)" : "");
	int rlen = snprintf(rstatus, sizeof(rstatus), "%d/%d", config.y + 1, config.numlines);
	if(len > config.screencols) len = config.screencols;
	AttachBuffer(buff, status, len);
	while(len < config.screencols){
		if(config.screencols - len == rlen){
			AttachBuffer(buff, rstatus, rlen);
			break;
		}else{
			AttachBuffer(buff, " ", 1);
			len++;
		}
	}
	AttachBuffer(buff, "\x1b[m", 3);
	AttachBuffer(buff, "\r\n", 2);
}

void DrawMessageBar(struct buffer *buff){
	AttachBuffer(buff, "\x1b[K", 3);
	int msglen = strlen(config.statusmsg);
	if(msglen > config.screencols) msglen = config.screencols;
	if(msglen && time(NULL) - config.statusmsg_time < 5)
		AttachBuffer(buff, config.statusmsg, msglen);
}

void RefreshScreen(){
	Scroll();
	struct buffer buff = StartBuffer;
	AttachBuffer(&buff, "\x1b[?25l", 6);
	AttachBuffer(&buff, "\x1b[H", 3);
	DrawLines(&buff);
	DrawStatusBar(&buff);
	DrawMessageBar(&buff);
	char buf[32];
	snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (config.y - config.offline) + 1, (config.renderx - config.coloff) + 1);
	AttachBuffer(&buff, buf, strlen(buf));
	AttachBuffer(&buff, "\x1b[?25h", 6);
	write(STDOUT_FILENO, buff.b, buff.len);
	FreeBuffer(&buff);
}

void SetStatusMessage(const char *fmt, ...){
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(config.statusmsg, sizeof(config.statusmsg), fmt, ap);
	va_end(ap);
	config.statusmsg_time = time(NULL);
}

/* input */

char *Prompt(char *prompt, void(*callback)(char *, int)){
	size_t bufsize = 128;
	char *buf = malloc(bufsize);
	size_t buflen = 0;
	buf[0] = '\0';
	while(1){
		SetStatusMessage(prompt, buf);
		RefreshScreen();
		int c = ReadKey();
		if(c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
			if(buflen != 0) buf[--buflen] = '\0';
		}else if(c == '\x1b'){
			SetStatusMessage("");
			if(callback) callback(buf, c);
			free(buf);
			return NULL;
		}else if(c == '\r'){
			if(buflen != 0){
				SetStatusMessage("");
				if(callback) callback(buf, c);
				return buf;
			}
		}else if(!iscntrl(c) && c < 128){
			if(buflen == bufsize - 1){
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}
		if(callback) callback(buf, c);
	}
}

void MoveCursor(int key){
	eline *line = (config.y >= config.numlines) ? NULL : &config.line[config.y];
	switch (key){
		case ARROW_LEFT:
		if(config.x != 0){
		config.x--;
		}else if(config.y > 0){
			config.y--;
			config.x = config.line[config.y].size;
		}
		break;
		case ARROW_RIGHT:
		if(line && config.x < line->size){
		config.x++;
		}else if(line && config.x == line->size){
			config.y++;
			config.x = 0;
		}
		break;
		case ARROW_UP:
		if(config.y != 0){
		config.y--;
		}
		break;
		case ARROW_DOWN:
		if(config.y < config.numlines){
		config.y++;
		}
		break;
	}
	line = (config.y >= config.numlines) ? NULL : &config.line[config.y];
	int linelen = line ? line->size : 0;
	if(config.x > linelen){
		config.x = linelen;
	}
}

void ProcessKeypress(){
	static int quit_times = JURA_QUIT_TIMES;
	int c = ReadKey();
	switch (c){
	case '\r':
		InsertNewline();
		break;
	case CTRL_KEY('q'):
		if(config.mod && quit_times > 0){
			SetStatusMessage("WARNING! File has unsaved changes." "Press Ctrl-Q %d more times to quit", quit_times);
			quit_times--;
			return;
		}
		write(STDOUT_FILENO, "\x1b[2J", 4);
		write(STDOUT_FILENO, "\x1b[H", 3);
		exit(0);
		break;
	case CTRL_KEY('s'):
		Save();
		SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
		break;
	case CTRL_KEY('f'):
		Find();
		SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
		break;
	case BACKSPACE:
	case CTRL_KEY('h'):
	case DEL_KEY:
		if(c == DEL_KEY) MoveCursor(ARROW_RIGHT);
		RemoveChar();
		break;
	case PAGE_UP:
	case PAGE_DOWN:
		{
			if(c == PAGE_UP){
				config.y = config.offline;
			}else if(c == PAGE_DOWN){
				config.y = config.offline + config.screenlines - 1;
			}
			int times = config.screencols;
			while(times--)
				MoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN);
		}
		break;
	case ARROW_UP:
	case ARROW_DOWN:
	case ARROW_LEFT:
	case ARROW_RIGHT:
		MoveCursor(c);
		break;
	case CTRL_KEY('l'):
	case '\x1b':
		break;
	default:
		InsertChar(c);
		break;
	}
	quit_times = JURA_QUIT_TIMES;
}

/* init */

void init(){
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
	if (getWindowSize(&config.screenlines, &config.screencols) == -1) die("getWindowSize");
	config.screenlines -= 2;
}

int main(int argc, char *argv[]){
	enableRawMode();
	init();
	if(argc >= 2){
		Open(argv[1]);
	}
	SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find");
	while (1){
		RefreshScreen();
		ProcessKeypress();
	}
    return 0;
}
