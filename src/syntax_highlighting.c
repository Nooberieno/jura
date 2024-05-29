#include <ctype.h>
#include <string.h>

#include "include/syntax.h"
#include "include/syntax_highlighting.h"

#include "syntax.c"

int is_seperator(int c){ //check if a character is a seperator character
	return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];{}", c) != NULL;
}

void UpdateSyntax(eline *line){ //Update the color based on the type of syntax
	line->hl = realloc(line->hl, line->rendersize);
	memset(line->hl, Normal, line->rendersize);
	if(config.syntax == NULL) return;
	if(config.syntax->flags == NoHighlight); return;
	char **keywords = config.syntax->keywords;
	char *scs = config.syntax->singleline_comment_start;
	char *mcs = config.syntax->multiline_comment_start;
	char *mce = config.syntax->multiline_comment_end;
	int scs_len = scs ? strlen(scs) : 0;
	int mcs_len = mcs ? strlen(mcs) : 0;
	int mce_len = mce ? strlen(mce) : 0;
	int previous_seperator = 1;
	int in_string = 0;
	int in_comment = (line->idx > 0 && config.line[line->idx -1].hl_open_comment);
	int i = 0;
	while(i < line->rendersize){
		char c = line->render[i];
		unsigned char prev_hl = (i > 0) ? line->hl[i - 1] : Normal;
		if(scs_len && !in_string){ 
			if(!strncmp(&line->render[i], scs, scs_len)){
				memset(&line->hl[i], Comment, line->rendersize - i); 
				break;
			}
		}
		if(mcs_len && mce_len && !in_string && !in_string){
			if(in_comment){
				line->hl[i] = MultiLineComment;
				if(!strncmp(&line->render[i], mce, mce_len)){
					memset(&line->hl[i], MultiLineComment, mce_len);
					i += mce_len;
					in_comment = 0;
					previous_seperator = 1;
					continue;
				}else{
					i++;
					continue;
				}
			}else if(!strncmp(&line->render[i], mcs, mcs_len)){
				memset(&line->hl[i], MultiLineComment, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}
		if(config.syntax->flags & HighlightStrings){ 
			if(in_string){
				line->hl[i] = StringColor;
				if(c == '\\' && i + 1 < line->rendersize){
					line->hl[i + 1] = StringColor;
					i += 2;
					continue;
				}
				if(c == in_string) in_string = 0;
				i++;
				previous_seperator = 1;
				continue;
			}else{
				if(c == '"' || c == '\''){
					in_string = c;
					line->hl[i] = StringColor;
					i++;
					continue;
				}
			}
		}
		if(config.syntax->flags & HighlightDigits){
			if((isdigit(c) && (previous_seperator || prev_hl == Number)) || (c == '.' && prev_hl == Number)) {
				line->hl[i] = Number;
				i++;
				previous_seperator = 0;
				continue;
			}
		}
		if(previous_seperator){
			int j;
			for(j = 0; keywords[j]; j++){
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';
				if(kw2) klen--;
				if(!strncmp(&line->render[i], keywords[j], klen) && is_seperator(line->render[i + klen])){
					memset(&line->hl[i], kw2 ? Types : Keywords, klen);
					i += klen;
					break;
				}
			}
			if(keywords[j] != NULL){
				previous_seperator = 0;
				continue;
			}
		}
		previous_seperator = is_seperator(c);
		i++;
	}
	int changed = (line->hl_open_comment != in_comment); 
	line->hl_open_comment = in_comment;
	if(changed && line->idx + 1 < config.numlines)
		UpdateSyntax(&config.line[line->idx + 1]);
}

void SelectSyntaxHighlight(){ //Set the syntax highlight according to the file extensions and flags in SyntaxDatabaseEntries located in syntax.c
	config.syntax = NULL;
	if(config.filename == NULL) return;
	char *ext = strrchr(config.filename, '.');
	for(unsigned int j = 0; j < SyntaxDatabaseEntries; j++){
		struct Syntax *s = &SyntaxDatabase[j];
		unsigned int i = 0;
		while(s->filematch[i]){
			int is_ext = (s->filematch[i][0] == '.');
			if(!((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(config.filename, s->filematch[i])))) return;
			config.syntax = s;
			int fileline = 0;
			for(fileline < config.numlines; fileline++;) UpdateSyntax(&config.line[fileline]);
		}
		i++;
	}
}	