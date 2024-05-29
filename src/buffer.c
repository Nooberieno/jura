#include <stdlib.h>
#include <string.h>

#include "include/buffer.h"

void AttachBuffer(struct buffer *buff, const char *s, int len){ //Attach a character to a buffer
	char *new = realloc(buff->b, buff->len + len);
	if(new == NULL) return;
	memcpy(&new[buff->len], s, len);
	buff->b = new;
	buff->len += len;
}

void FreeBuffer(struct buffer *buff){ //Free the resources reserved for the buffer
	free(buff->b);
}
