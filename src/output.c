#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>

#include "include/output.h"
#include "include/config.h"
#include "include/line_operations.h"
#include "include/jura.h"
#include "include/UserConfig.h"
#include "include/syntax_highlighting.h"

void Scroll(){ //Tells jura what to do in certain scroll edgecases
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

void DrawWelcomeMessage(struct buffer *buff){ //Draw a welcome message to the screen
    char welcome[80];
    int welkom = snprintf(welcome, sizeof(welcome), "jura  -- version %s", CurrentJuraVersion);
    if (welkom > config.screencols) welkom = config.screencols;
    int padding = (config.screencols - welkom) / 2;
    if (padding) {
        AttachBuffer(buff, UConfig.First_Char, 1);
        padding--;
    }
    while (padding--) AttachBuffer(buff, " ", 1);
    AttachBuffer(buff, welcome, welkom);
}

void DrawEmptyLine(struct buffer *buff) { //Draw an empty line to the screen
    AttachBuffer(buff, UConfig.First_Char, 1);
}

void DrawControlCharacter(struct buffer *buff, char c, int current_color){ //Draw a character that should not be displayed normally (like a control character or a weird binary thing)
    char sym = (c <= 26) ? '@' + c : '?';
    AttachBuffer(buff, "\x1b[7m", 4); 
    AttachBuffer(buff, &sym, 1);
    AttachBuffer(buff, "\x1b[m", 3); 
    if (current_color != -1) {
        char buf[16];
        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
        AttachBuffer(buff, buf, clen); 
    }
}

void DrawNormalCharacter(struct buffer *buff, char c, int *current_color){ //Draw a character that should be displayed normally
    if (*current_color != -1) {
        AttachBuffer(buff, "\x1b[39m", 5);
        *current_color = -1;
    }
    AttachBuffer(buff, &c, 1);
}

void DrawSyntaxCharacter(struct buffer *buff, char c, int color, int *current_color){ //Draw a character that has a different color due to its syntax
    if (color != *current_color) {
        *current_color = color;
        char buf[16];
        int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
        AttachBuffer(buff, buf, clen); 
    }
    AttachBuffer(buff, &c, 1);
}

void DrawLine(struct buffer *buff, int fileline){ //Draw a single line to the screen
    int len = config.line[fileline].rendersize - config.coloff;
    if (len < 0) len = 0;
    if (len > config.screencols) len = config.screencols;
    char *c = &config.line[fileline].render[config.coloff];
    unsigned char *hl = &config.line[fileline].hl[config.coloff];
    int current_color = -1;
    for (int j = 0; j < len; j++) {
        if (iscntrl(c[j])) {
            DrawControlCharacter(buff, c[j], current_color);
        } else if (hl[j] == Normal) {
            DrawNormalCharacter(buff, c[j], &current_color);
        } else {
            int color = SyntaxToColor(hl[j]);
            DrawSyntaxCharacter(buff, c[j], color, &current_color);
        }
    }
    AttachBuffer(buff, "\x1b[39m", 5); 
}

void DrawLines(struct buffer *buff){ //Draw the lines to the screen
    for (int y = 0; y < config.screenlines; y++) {
        int fileline = y + config.offline;
        if (fileline >= config.numlines) {
            if (config.numlines == 0 && y == config.screenlines / 3) {
                DrawWelcomeMessage(buff);
            } else {
                DrawEmptyLine(buff);
            }
        } else {
            DrawLine(buff, fileline);
        }
        AttachBuffer(buff, "\x1b[K", 3); 
        AttachBuffer(buff, "\r\n", 2); 
    }
}

void DrawStatusBar(struct buffer *buff){ //Set the top status bar
	AttachBuffer(buff, "\x1b[7m", 4);
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), "%.20s - %d lines %s", config.filename ? config.filename : "[No Name]", config.numlines, config.mod ? "(modified)" : "");
	int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", config.syntax ? config.syntax->filetype : "no filetype detected", config.y + 1, config.numlines);
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

void DrawMessageBar(struct buffer *buff){ //Set the botom message bar
	AttachBuffer(buff, "\x1b[K", 3);
	int msglen = strlen(config.statusmsg);
	if(msglen > config.screencols) msglen = config.screencols;
	if(msglen && time(NULL) - config.statusmsg_time < 5) AttachBuffer(buff, config.statusmsg, msglen);
}

void RefreshScreen(){ //Refresh the screen
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

void SetStatusMessage(const char *fmt, ...){ //set the message for the message bar
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(config.statusmsg, sizeof(config.statusmsg), fmt, ap);
	va_end(ap);
	config.statusmsg_time = time(NULL);
}
