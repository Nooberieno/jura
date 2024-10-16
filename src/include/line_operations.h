#ifndef LINE_OPS_H
#define LINE_OPS_H

#include "config.h"

int LineXToRenderx(eline *line, int x);

int LineRenderxToX(eline *line, int renderx);

void UpdateLine(eline *line);

void InsertLine(int at, char *s, size_t len);

void FreeLine(eline *line);

void RemoveLine(int at);

void LineInsertChar(eline *line, int at, int c);

void LineAppendString(eline *line, char *s, size_t len);

void LineRemoveChar(eline *line, int at);

#endif
