#ifndef OUTPUT_H
#define OUTPUT_H

#include "buffer.h"

extern void Scroll();

extern void DrawWelcomeMessage(struct buffer *buff);

extern void DrawEmptyLine(struct buffer *buff);

extern void DrawControlCharacter(struct buffer *buff, char c, int current_color);

extern void DrawNormalCharacter(struct buffer *buff, char c, int *current_color);

extern void DrawSyntaxCharacter(struct buffer *buff, char c, int color, int *current_color);

extern void DrawLine(struct buffer *buff, int fileline);

extern void DrawLines(struct buffer *buff);

extern void DrawStatusBar(struct buffer *buff);

extern void DrawMessageBar(struct buffer *buff);

extern void RefreshScreen();

extern void SetStatusMessage(const char *fmt, ...);

#endif