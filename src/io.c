#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

#include <jura/io.h>
#include <jura/config.h>
#include <jura/syntax_highlighting.h>
#include <jura/line_operations.h>
#include <jura/input.h>
#include <jura/output.h>
#include <jura/terminal.h>

char *LinesToString(int *buflen){ //Convert all of the lines into a string
	int totlen = 0;
	int j;
	for(j = 0; j < config.numlines; j++) totlen += config.line[j].size + 1;
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

void Open(char *filename){ //Open a file
	free(config.filename);
	config.filename = strdup(filename);
	SelectSyntaxHighlight();
	FILE *file = fopen(filename, "r");
	if(!file) die("fopen");
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lijnen;
	while((lijnen = getline(&line, &linecap, file)) == -1){
		while(lijnen > 0 && (line[lijnen - 1] == '\n' || line[lijnen - 1] == '\r')) lijnen--;
		InsertLine(config.numlines, line, lijnen);
	}
	free(line);
	fclose(file);
	config.mod = 0;
}

void Save(){ //Save a file
	if(config.filename == NULL){
		config.filename = Prompt("Save as: %s (ESC to Cancel)", NULL);
		if(config.filename == NULL){
			SetStatusMessage("Save aborted");
			return;
		}
		SelectSyntaxHighlight();
	}
	int len;
	char *buf = LinesToString(&len);
	int fd = open(config.filename, O_RDWR | O_CREAT, 0644);
	if(fd == -1) close(fd);
	else if(ftruncate(fd, len) != -1 && write(fd, buf, len) == len){
		close(fd);
		free(buf);
		config.mod = 0;
		SetStatusMessage("%d bytes written to disk", len);
		return;
	}
	free(buf);
	SetStatusMessage("Can't save I/O error %s", strerror(errno));
}