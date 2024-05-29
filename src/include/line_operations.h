#ifndef LINE_OPS_H
#define LINE_OPS_H

#include "config.h"

extern int LineXToRenderx(eline *line, int x);

extern int LineRenderxToX(eline *line, int renderx);

extern void UpdateLine(eline *line);

extern void InsertLine(int at, char *s, size_t len);

extern void FreeLine(eline *line);

extern void RemoveLine(int at);

extern void LineInsertChar(eline *line, int at, int c);

extern void LineAppendString(eline *line, char *s, size_t len);

extern void LineRemoveChar(eline *line, int at);

#endif
