#ifndef OUTPUT_H
#define OUTPUT_H

#include "buffer.h"

void Scroll();

void DrawWelcomeMessage(struct buffer *buff);

void DrawEmptyLine(struct buffer *buff);

void DrawControlCharacter(struct buffer *buff, char c, int current_color);

void DrawNormalCharacter(struct buffer *buff, char c, int *current_color);

void DrawSyntaxCharacter(struct buffer *buff, char c, int color, int *current_color);

void DrawLine(struct buffer *buff, int fileline);

void DrawLines(struct buffer *buff);

void DrawStatusBar(struct buffer *buff);

void DrawMessageBar(struct buffer *buff);

void RefreshScreen();

void SetStatusMessage(const char *fmt, ...);

#endif