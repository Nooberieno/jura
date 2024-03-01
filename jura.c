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

#define CurrentJuraVersion "2.4"
#define JuraTabStop 8
#define JuraQuitTimes 1
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

enum Highlight{
	HL_NORMAL = 0,
	HL_COMMENT,
	HL_MLCOMMENT,
	HL_KEYWORD1,
	HL_KEYWORD2,
	HL_STRING,
	HL_NUMBER,
	HL_MATCH
};

#define HighlightDigits (1<<0)
#define HighlightStrings (1<<1)
#define NoHighlight (1<<1)

/* data */

struct Syntax{
	char *filetype;
	char **filematch;
	char **keywords;
	char *singleline_comment_start;
	char *multiline_comment_start;
	char *multiline_comment_end;
	int flags;
};

typedef struct editorline{
	int idx;
	int size;
	int rendersize;
	char *chars;
	char *render;
	unsigned char *hl;
	int hl_open_comment;
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
	struct Syntax *syntax;
	struct termios og_terminal;
};

struct Config config;

/* filetypes */

char *CExtensions[] = {".c", ".h", NULL};
char *CKeywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef", "static", "enum", "class", "case",
	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|", NULL
};

char *CppExtensions[] = {".cpp", ".CPP", ".cc", ".cp", ".cxx", ".C", ".hh", ".hpp", NULL};
char *CppKeywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef", "static", "enum", "class", "case", "alignas", "alignof",
	"and", "and_eq", "asm", "bitand", "bitor", "class", "compl", "constexpr", "const_cast", "deltype", "delete", "dynamic_cast", "explicit", "export", "false",
	"friend", "inline", "mutable", "namspace", "new", "noexcept", "not", "not_eq", "nullptr", "operator", "or", "or_eq", "private", "protected", "public",
	"reinterpret_cast", "static_assert", "static_cast", "template", "this", "thread_local", "throw", "true", "try", "typeid", "typename", "virtual", "xor", "xor_eq",
	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|", NULL
};
char *PythonExtensions[] = {".py", NULL};
char *PythonKeywords[] = {
	"False", "None", "True", "and", "as", "assert", "async", "await", "await", "break", "class", "continue", "def", "del", "elif", "else", "except", "finally", "for",
	"for", "from", "global", "if", "import", "in", "is", "lambda", "nonlocal", "not", "or", "pass", "raise", "return", "try", "while", "with", "yield",
	"int|", "float|", "complex|", "bool|", "str|", "list|", "tuple|", "set|", "dict|", "NoneType|", NULL
};
char *LuaExtensions[] = {".lua", NULL};
char *LuaKeywords[] = {
	"and", "break", "do", "else", "elseif", "end", "false", "for", "function", "if", "in", "local", "nil", "not", "or", "repeat", "return", "then", "true", "until", "while",
	"nil|", NULL
};
char *RustExtensions[] = {".rs", NULL};
char *RustKeywords[] = {
	"as", "async", "await", "break", "const", "continue", "crate", "dyn", "else", "enum", "extern", "false", "fn", "for", "if", "impl", "in", "let", "loop", "match",
	"mod", "move", "mut", "pub", "ref", "return", "self", "static", "struct", "super", "trait", "true", "type", "unsafe", "use", "where", "while",
	"bool|", "char|", "i8|", "i16|", "i32|", "i64|", "i128|", "u8|", "u16|", "u32|", "u64|", "u128|", "f32|", "f64|", "str|", "tuple|", "array|", "slice|", "pointer|",
	"reference|", "Option|", "Result|", "struct|", "enum|", "trait|", "impl|", NULL 
};
char *JSExtensions[] = {".js", NULL};
char *JSKeywords[] = {
	"break", "case", "catch", "class", "const", "continue", "debugger", "default", "delete", "do", "else", "export", "extends", "finally", "for", "function", "if",
	"import", "in", "instanceof", "new", "return", "super", "switch", "this", "throw", "try", "typeof", "var", "void", "while", "with", "yield", NULL
};
char *TSExtensions[] = {"ts", ".ts", NULL};
char *TSKeywords[] = {
	"break", "case", "catch", "class", "const", "continue", "debugger", "default", "delete", "do", "else", "export", "extends", "finally", "for", "function", "if",
	"import", "in", "instanceof", "new", "return", "super", "switch", "this", "throw", "try", "typeof", "var", "void", "while", "with", "yield", "type", "interface",
	"implements", "enum", "readonly",
	"undefined|", "null|", "boolean|", "number|", "string|", "object|", "symbol|", "BigInt|", "any|", NULL
};
char *CSExtensions[] = {".cs", NULL};
char *CSKeywords[] = {
	"abstract", "as", "base", "bool", "break", "byte", "case", "catch", "char", "checked", "class", "const", "continue", "default", "delegate", "do", "else", "enum",
	"event", "explicit", "extern", "false", "finally", "fixed", "for", "foreach", "goto", "if", "implicit", "in", "interface", "internal", "is", "lock", "namespace",
	"new", "object", "operator", "out", "override", "params", "privte", "protected", "public", "readonly", "sbyte", "sealed", "short", "sizeof", "stackalloc", "statoc",
	"struct", "switch", "this", "throw", "true", "typeof", "unchecked", "unsafe", "using", "var", "virtual", "void", "volatile", "while", "yield",
	"bool|", "byte|", "char|", "decimal|", "double|", "float|", "int|", "long|", "object|", "sbyte|", "short|", "short|", "string|", "uint|", "ulong|", "ushort|", NULL
};
char *JavaExtensions[] = {".java", NULL};
char *JavaKeywords[] = {
	"abstract", "assert", "break", "case", "catch", "class", "continue", "default", "do", "else", "enum", "extends", "final", "finally", "for", "if", "implements",
	"import", "instanceof", "interface", "native", "now", "package", "private", "protected", "public", "return", "static", "strictfp", "super", "switch", "syncronized",
	"this", "throw", "throws", "transient", "try", "void", "volatile", "while",
	"boolean|", "byte|", "char|", "double|", "float|", "int|", "long|", "short|", NULL
};
char *GolangExtensions[] = {".go", ".mod", ".sum", NULL};
char *GoKeywords[] = {
	"break", "case", "chan", "const", "continue", "default", "defer", "else", "fallthrough", "for", "func", "go", "if", "import", "interface", "map", "package", "range", "return", "select", "struct", "switch", "type", "var",
	"bool|", "byte|", "complex64|", "complex128|", "error|", "float32|", "float64|", "int|", "int8|", "int16|", "int32|", "int64|", "rune|", "string|", "uint|", "uint8|", "uint16|", "uint32|", "uint64|", "uintptr|", NULL

};
char *SwiftExtensions[] = {".swift", ".xcodeproj", ".storyboard", ".xib", ".xcdatamodeld", NULL};
char *SwiftKeywords[] = {
	"associatedtype", "break", "case", "catch", "class", "continue", "convenience", "deinit", "didSet", "do", "else", "enum", "extension", "fallthrough", "final", "for", "func", "guard", "if", "import", "in", "indirect", "init", "inout", "internal", "is", "as", "lazy", "let", "localized", "mutating", "nil", "nonce", "nonisolated", "objc", "open", "optional", "override", "postfix", "precedencegroup", "private", "protocol", "public", "required", "rethrows", "return", "self", "set", "some", "static", "struct", "subscript", "super", "switch", "synchronized", "throw", "throws", "try", "typealias", "var", "weak", "where", "while", "willSet",
	"Bool|", "Double|", "Float|", "Int|", "Int8|", "Int16|", "Int32|", "Int64|", "String|", "UInt|", "UInt8|", "UInt16|", "UInt32|", "UInt64|", "Character|", "Any|", "AnyObject|", "Array|", "Dictionary|", "FloatingPointNumber|", "Hashable|", "IntegerLiteralConvertible|", "Numeric|", "Optional|", "Range|", "RawRepresentable|", "Sequence|", "SignedNumeric|", "Strideable|", "Subscript|", "Comparable|", "Codable|", "Decodable|", "Encodable|", "Error|", "UnsafeMutableRawPointer|", "UnsafeMutableRawBufferPointer|", "UnsafePointer|", "UnsafeRawBufferPointer|", NULL
};
char *HTMLExtensions[] = {".html", NULL};
char *HTMLKeywords[] = {
	"<!DOCTYPE html>",
	"<html>", "<head>", "<title>", "</title>", "</head>", "<body>", "</body>", "</html>",
	"<h1>", "<h2>", "<h3>", "<h4>", "<h5>", "<h6>", "</h1>", "</h2>", "</h3>", "</h4>", "</h5>", "<h6>",
	"<p>", "</p>",
	"<ul>", "<ol>", "<li>", "</li>", "</ul>", "</ol>",
	"<img src=\"...\"/>", "<a href=\"...\"></a>",
	"<b>", "<i>", "<u>", "<sup>", "<sub>", "</b>", "</i>", "</u>", "</sup>", "<sub>",
	"<table>", "<tr>", "<th>", "<td>", "</table>", "<tr>", "<th>", "<td>",
	"<form>", "<input type=\"...\"/>", "<button>", "</form>", "<script>", "</script>"
	"<br/>", "<hr/>", NULL
};
char *JSONExtensions[] = {".json", NULL};
char *JSONSyntax[] = {
	"{", "}", "[", "]", ",", ":", NULL
};
char *CssExtensions[] = {".css", NULL};
char *CssSyntax[] = {
	/* Selectors */
"body", "*", "h1", "h2", "h3", "h4", "h5", "h6", "p", "a", "ul", "ol", "li", "img", "table", "tr", "th", "td", "form", "input", "button", ".", "#...",

/* Properties */
"background-color", "color", "font-family", "font-size", "font-weight", "text-align", "margin", "padding", "border", "width", "height", "display", "float", "flex", "grid", "position", "z-index",

/* Values */
"inherit", "initial", "auto", "none",
"transparent", "black", "white", "red", "green", "blue", "yellow", "purple", "orange", "teal", "pink", "gray",
"Arial", "Verdana", "Helvetica", "Times New Roman", "Courier New",
"12px", "16px", "20px", "24px",
"bold", "italic", "normal",
"left", "center", "right", "justify",
"10px", "20px", "auto",
"display: block", "display: inline", "display: flex", NULL
};

char *TextExtensions[] = {".txt", NULL};
char *BinaryExtensions[] = {".appimage", ".AppImage", "Appimage", NULL};
char *ShellExtensions[] = {".sh", NULL};

struct Syntax SyntaxDatabase[] = {
	{
		"c",
		CExtensions,
		CKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"c++",
		CppExtensions,
		CppKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"Python",
		PythonExtensions,
		PythonKeywords,
		"#", "...", "...",
		HighlightDigits | HighlightStrings
	},
	{
		"Lua",
		LuaExtensions,
		LuaKeywords,
		"--", "--[[", "]]--",
		HighlightDigits | HighlightStrings
	},
	{
		"Rust",
		RustExtensions,
		RustKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"JavaScript",
		JSExtensions,
		JSKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"TypeScript",
		TSExtensions,
		TSKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"C#",
		CSExtensions,
		CSKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"Java",
		JavaExtensions,
		JavaKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"GO",
		GolangExtensions,
		GoKeywords,
		"//", "/*", "*/",
		HighlightDigits| HighlightStrings
	},
	{
		"Swift",
		SwiftExtensions,
		SwiftKeywords,
		"//", "/*", "*/",
		HighlightDigits| HighlightStrings
	},
	{
		"HTML",
		HTMLExtensions,
		HTMLKeywords,
		NULL, NULL, NULL,
		HighlightDigits | HighlightStrings
	},
	{
		"JSON",
		JSONExtensions,
		JSONSyntax,
		NULL, NULL, NULL,
		HighlightStrings
	},
	{
		"css",
		CssExtensions,
		CssSyntax,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"text file",
		TextExtensions,
		NULL,
		NULL, NULL, NULL,
		NoHighlight
	},
	{
		"Binary",
		BinaryExtensions,
		NULL,
		NULL, NULL, NULL,
		NoHighlight
	},
	{
		"Shell script",
		ShellExtensions,
		NULL,
		NULL, NULL, NULL,
		NoHighlight
	}
};

#define SyntaxDatabaseEntries (sizeof(SyntaxDatabase) / sizeof(SyntaxDatabase[0]))

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

int getCursorPosition(int *lines, int *cols){
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

/* syntax highlighting */

int is_seperator(int c){
	return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];{}", c) != NULL;
}

void UpdateSyntax(eline *line){
	line->hl = realloc(line->hl, line->rendersize);
	memset(line->hl, HL_NORMAL, line->rendersize);
	if(config.syntax == NULL) return;
	if(config.syntax->flags == NoHighlight) return;
	char **keywords = config.syntax->keywords;
	char *scs = config.syntax->singleline_comment_start;
	char *mcs = config.syntax->multiline_comment_start;
	char *mce = config.syntax->multiline_comment_end;
	int scs_len = scs ? strlen(scs) : 0;
	int mcs_len = mcs ? strlen(mcs) : 0;
	int mce_len = mce ? strlen(mce) : 0;
	int prev_sep = 1;
	int in_string = 0;
	int in_comment = (line->idx > 0 && config.line[line->idx - 1].hl_open_comment);
	int i = 0;
	while(i < line->rendersize){
		char c = line->render[i];
		unsigned char prev_hl = (i > 0) ? line->hl[i - 1] : HL_NORMAL;
		if(scs_len && !in_string){
			if(!strncmp(&line->render[i], scs, scs_len)){
				memset(&line->hl[i], HL_COMMENT, line->rendersize - i);
				break;
			}
		}
		if(mcs_len && mce_len && !in_string && !in_string){
			if(in_comment){
				line->hl[i] = HL_MLCOMMENT;
				if(!strncmp(&line->render[i], mce, mce_len)){
					memset(&line->hl[i], HL_MLCOMMENT, mce_len);
					i += mce_len;
					in_comment = 0;
					prev_sep = 1;
					continue;
				}else{
					i++;
					continue;
				}
			}else if(!strncmp(&line->render[i], mcs, mcs_len)){
				memset(&line->hl[i], HL_MLCOMMENT, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}
		if(config.syntax->flags & HighlightStrings){
			if(in_string){
				line->hl[i] = HL_STRING;
				if(c == '\\' && i + 1 < line->rendersize){
					line->hl[i + 1] = HL_STRING;
					i += 2;
					continue;
				}
				if(c == in_string) in_string = 0;
				i++;
				prev_sep = 1;
				continue;
			}else{
				if(c == '"' || c == '\''){
					in_string = c;
					line->hl[i] = HL_STRING;
					i++;
					continue;
				}
			}
		}
		if(config.syntax->flags & HighlightDigits){
			if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)) {
				line->hl[i] = HL_NUMBER;
				i++;
				prev_sep = 0;
				continue;
			}
		}
		if(prev_sep){
			int j;
			for(j = 0; keywords[j]; j++){
				int klen = strlen(keywords[j]);
				int kw2 = keywords[j][klen - 1] == '|';
				if(kw2) klen--;
				if(!strncmp(&line->render[i], keywords[j], klen) && is_seperator(line->render[i + klen])){
					memset(&line->hl[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
					i += klen;
					break;
				}
			}
			if(keywords[j] != NULL){
				prev_sep = 0;
				continue;
			}
		}
		prev_sep = is_seperator(c);
		i++;
	}
	int changed = (line->hl_open_comment != in_comment);
	line->hl_open_comment = in_comment;
	if(changed && line->idx + 1 < config.numlines)
		UpdateSyntax(&config.line[line->idx + 1]);
}

int SyntaxToColor(int hl){
	switch(hl){
		case HL_COMMENT:
		case HL_MLCOMMENT: return 36;
		case HL_KEYWORD1: return 33;
		case HL_KEYWORD2: return 32;
		case HL_STRING: return 35;
		case HL_NUMBER: return 31;
		case HL_MATCH: return 34;
		default: return 37;
	}
}

void SelectSyntaxHighlight(){
	config.syntax = NULL;
	if(config.filename == NULL) return;
	char *ext = strrchr(config.filename, '.');
	for(unsigned int j = 0; j < SyntaxDatabaseEntries; j++){
		struct Syntax *s = &SyntaxDatabase[j];
		unsigned int i = 0;
		while(s->filematch[i]){
			int is_ext = (s->filematch[i][0] == '.');
			if((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(config.filename, s->filematch[i]))){
				config.syntax = s;
				int fileline;
				for(fileline = 0; fileline < config.numlines; fileline++){
					UpdateSyntax(&config.line[fileline]);
				}
				return;
			}
			i++;
		}
	}
}

/* line operations */

int LineXToRenderx(eline *line, int x){
	int renderx = 0;
	int j;
	for(j = 0; j < x; j++){
		if(line->chars[j] == '\t')
			renderx += (JuraTabStop - 1) - (renderx % JuraTabStop);
		renderx++;
	}
	return renderx;
}

int LineRenderxToX(eline *line, int renderx){
	int cur_renderx = 0;
	int x;
	for(x = 0; x < line->size; x++){
		if(line->chars[x] == '\t')
			cur_renderx += (JuraTabStop -1) - (cur_renderx % JuraTabStop);
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
	line->render = malloc(line->size + tabs*(JuraTabStop - 1) + 1);
	int idx = 0;
	for(j = 0; j < line->size; j++){
		if(line->chars[j] == '\t'){
			line->render[idx++] = ' ';
			while(idx % JuraTabStop != 0) line->render[idx++] = ' ';
		}else{
			line->render[idx++] = line->chars[j];
		}
	}
	line->render[idx] = '\0';
	line->rendersize = idx;
	UpdateSyntax(line);
}

void InsertLine(int at, char *s, size_t len){
	if(at < 0 || at > config.numlines) return;
	config.line = realloc(config.line, sizeof(eline) * (config.numlines + 1));
	memmove(&config.line[at + 1], &config.line[at], sizeof(eline) * (config.numlines - at));
	for(int j = at + 1; j <= config.numlines; j++) config.line[j].idx++;
	config.line[at].idx = at;
	config.line[at].size = len;
	config.line[at].chars = malloc(len + 1);
	memcpy(config.line[at].chars, s, len);
	config.line[at].chars[len] = '\0';
	config.line[at].rendersize = 0;
	config.line[at].render = NULL;
	config.line[at].hl = NULL;
	config.line[at].hl_open_comment = 0;
	UpdateLine(&config.line[at]);
	config.numlines++;
	config.mod++;
}

void FreeLine(eline *line){
	free(line->render);
	free(line->chars);
	free(line->hl);
}

void RemoveLine(int at){
	if(at < 0 || at >= config.numlines) return;
	FreeLine(&config.line[at]);
	memmove(&config.line[at], &config.line[at + 1], sizeof(eline) * (config.numlines - at - 1));
	for(int j = at; j < config.numlines - 1; j++) config.line[j].idx--;
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

/* editor operations */

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
	SelectSyntaxHighlight();
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
		SelectSyntaxHighlight();
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
	static int saved_hl_line;
	static char *saved_hl = NULL;
	if(saved_hl){
		memcpy(config.line[saved_hl_line].hl, saved_hl, config.line[saved_hl_line].rendersize);
		free(saved_hl);
		saved_hl = NULL;
	}
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
			saved_hl_line = current;
			saved_hl = malloc(line->rendersize);
			memcpy(saved_hl, line->hl, line->rendersize);
			memset(&line->hl[match - line->render], HL_MATCH, strlen(query));
			break;
		}
	}
}

void Find(){
	int saved_cx = config.x;
	int saved_cy = config.y;
	int saved_coloff = config.coloff;
	int saved_offline = config.offline;
	char *query = Prompt("Search: %s (ESC|Arrow keys|Enter)", FindCallback);
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
			int welkom = snprintf(welcome, sizeof(welcome), "jura  -- version %s", CurrentJuraVersion);
			if(welkom > config.screencols) welkom = config.screencols;
			int padding = (config.screencols - welkom) / 2;
			if(padding){
				AttachBuffer(buff, "-", 1);
				padding--;
			}
			while(padding--) AttachBuffer(buff, " ", 1);
			AttachBuffer(buff, welcome, welkom);
		}else {
		AttachBuffer(buff, "-", 1);
		}
		}else{
			int len = config.line[fileline].rendersize - config.coloff;
			if(len < 0) len = 0;
			if(len > config.screencols) len = config.screencols;
			char *c = &config.line[fileline].render[config.coloff];
			unsigned char *hl = &config.line[fileline].hl[config.coloff];
			int current_color = -1;
			int j;
			for(j = 0; j < len; j++){
				if(iscntrl(c[j])){
					char sym = (c[j] <= 26) ? '@' + c[j] : '?';
					AttachBuffer(buff, "\x1b[7m", 4);
					AttachBuffer(buff, &sym, 1);
					AttachBuffer(buff, "\x1b[m", 3);
					if(current_color != -1){
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						AttachBuffer(buff, buf, clen);
					}
				}else if(hl[j] == HL_NORMAL){
					if(current_color != -1){
						AttachBuffer(buff, "\x1b[39m", 5);
						current_color = -1;
					}
					AttachBuffer(buff, &c[j], 1);
				} else{
					int color = SyntaxToColor(hl[j]);
					if(color != current_color){
						current_color = color;
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
						AttachBuffer(buff, buf, clen);
					}
					AttachBuffer(buff, &c[j], 1);
				}
			}
			AttachBuffer(buff, "\x1b[39m", 5);
		}
		AttachBuffer(buff, "\x1b[K", 3);
			AttachBuffer(buff, "\r\n", 2);
	}
}

void DrawStatusBar(struct buffer *buff){
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

void DrawMessageBar(struct buffer *buff){
	AttachBuffer(buff, "\x1b[K", 3);
	int msglen = strlen(config.statusmsg);
	if(msglen > config.screencols) msglen = config.screencols;
	if(msglen && time(NULL) - config.statusmsg_time < 5)
		AttachBuffer(buff, config.statusmsg, msglen);
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define BUFFER_SIZE 1024

void RefreshScreen() {
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
	static int quit_times = JuraQuitTimes;
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
	quit_times = JuraQuitTimes;
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
	config.syntax = NULL;
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
