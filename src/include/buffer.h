#ifndef BUFFER_H
#define BUFFER_H

typedef struct buffer{
	char *b;
	int len;
}buffer;

#define StartBuffer {NULL, 0}

extern void AttachBuffer(struct buffer *buff, const char *s, int len);

extern void FreeBuffer(struct buffer *buff);

#endif