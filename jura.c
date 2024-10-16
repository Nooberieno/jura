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

/* defines used throughout the program */

#define CurrentJuraVersion "4.2" //version that shows on startup
#define JuraTabStop 8 // how large the tab size is
#define JuraQuitTimes 1 //how many times needed to close the programing when not saving
#define CTRL_KEY(k) ((k) & 0x1f) //Control key input shortcut
#define FirstCharLength 2 //length of the empty line character

typedef struct UserConfig{ //contains everything that can be configured
	int Comment_Color;
	int Keywords_Color;
	int Types_Color;
	int StringColor_;
	int Number_Color;
	int Match_Color;
	char First_Char[FirstCharLength];
}UserConfig;

struct UserConfig UConfig;

void DefaultConfig(){ //loads the default values in the config.jura file that jura uses for configuration
	char *home_dir = getenv("HOME");
	char config_path[256];
	strcpy(config_path, home_dir);
    strcat(config_path, "/config.jura");
	FILE *file = fopen(config_path, "w");
	fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", 36, 33, 34, 31, 35, 32, "-");
	fclose(file);
}

void SaveConfig(struct UserConfig *config, char *filename){ //saves a file to the config container
	FILE *file = fopen(filename, "w");
	fprintf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", config->Comment_Color, config->Keywords_Color, config->Types_Color, config->StringColor_, config->Number_Color, config->Match_Color, config->First_Char);
	fclose(file);
}

void LoadConfig(struct UserConfig *config, char *filename){ //loads a config file to the config container
	FILE *file = fopen(filename, "r");
	if(file != NULL){
		fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", &config->Comment_Color, &config->Keywords_Color, &config->Types_Color, &config->StringColor_, &config->Number_Color, &config->Match_Color, config->First_Char);
		fclose(file);
	}else{ //if the configuration is empty load the default configuration and load them into the config container
		DefaultConfig();
		char *home_dir = getenv("HOME");
		char config_path[256];
		strcpy(config_path, home_dir);
    	strcat(config_path, "/config.jura");
		FILE *file = fopen(config_path, "r");
		fscanf(file, "%d\n%d\n%d\n%d\n%d\n%d\n%s", &config->Comment_Color, &config->Keywords_Color, &config->Types_Color, &config->StringColor_, &config->Number_Color, &config->Match_Color, config->First_Char);
		fclose(file);
	}
}

enum Key{ //important keys that jura uses
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_UP,
	ARROW_DOWN,
	DEL_KEY,
	PAGE_UP,
	PAGE_DOWN
};

enum Highlight{ //things that need to be highlighted
	Normal = 0,
	Comment,
	MultiLineComment,
	Keywords,
	Types,
	StringColor,
	Number,
	Match
};
//defines for the flags in the syntax struct
#define HighlightDigits (1<<0)
#define HighlightStrings (1<<1)
#define NoHighlight (1<<1)

/* data */

struct Syntax{ //track everything needed to change the syntax color
	char *filetype;
	char **filematch;
	char **keywords;
	char *singleline_comment_start;
	char *multiline_comment_start;
	char *multiline_comment_end;
	int flags;
};

typedef struct editorline{ //contains everything needed for a line in the editor
	int idx;
	int size;
	int rendersize;
	char *chars;
	char *render;
	unsigned char *hl;
	int hl_open_comment;
}eline;

struct Config{ //contains everything needed for the editor
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

/* filetypes with keywords and types */

char *CExtensions[] = {".c", ".h", NULL};
char *CKeywords[] = {
	"switch", "if", "while", "for", "break", "continue", "return", "else", "struct", "union", "typedef", "static", "enum", "class", "case",
	"int|", "long|", "double|", "float|", "char|", "unsigned|", "signed|", "void|", NULL //types have a \ at the end
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
char *TSExtensions[] = {".ts", NULL};
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
char *SwiftExtensions[] = {".swift", NULL};
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

char *TeXExtensions[] = {".tex", NULL};
char *TeXSyntax[] = {
	"\\alpha", "\\beta", "\\gamma", "\\delta", "\\epsilon", "\\varepsilon", "\\zeta", "\\eta", "\\theta", "\\vartheta", "\\iota", "\\kappa", "\\lambda", "\\mu", "\\nu", "\\xi", "\\o", "\\pi", "\\varpi", "\\rho", "\\varrho", "\\sigma",
	"\\varsigma", "\\tau", "\\upsilon", "\\phi", "\\varphi", "\\chi", "\\psi", "\\omega", "\\Alpha", "\\Beta", "\\Gamma", "\\Delta", "\\Epsilon", "\\Zeta", "\\Eta", "\\Theta", "\\Iota", "\\Kappa", "\\Lambda", "\\Mu", "\\Nu", "\\Xi", "\\O",
	"\\Pi", "\\Sigma", "\\Rho", "\\Tau", "\\Upsilon", "\\Phi", "\\Chi", "\\Psi", "\\Omega", "\\aleph", "\\hbar", "\\imath", "\\jmath", "\\ell", "\\wp", "\\Re", "\\Im", "\\partial", "\\infty", "\\prime", "\\emptyset", "\\nabla", "\\surd",
	"\\top", "\\bot", "\\|", "\\angle", "\\triangle", "\\backslash", "\\forall", "\\exists", "\\neg", "\\not", "\\flat", "\\natural", "\\sharp", "\\clubsuit", "\\diamondsuit", "\\heartsuit", "\\spadesuit", "\\sum", "\\prod", "\\coprod", "\\int",
	"\\oint", "\\bigcap", "\\bigcup", "\\bigsqcup", "\\bigvee", "\\bigwedge", "\\bigodot", "\\bigotimes", "\\bigoplus", "\\biguplus", "\\pm", "\\mp", "\\setminus", "\\cdot", "\\times", "\\ast", "\\star", "\\diamond", "\\circ", "\\bullet", "\\div",
	"\\cap", "\\cup", "\\uplus", "\\sqcap", "\\sqcup", "\\triangleleft", "\\triangleright", "\\wr", "\\bigcirc", "\\bigtriangleup", "\\bigtriangledown", "\\vee", "\\lor", "\\wedge", "\\land", "\\oplus", "\\ominus", "\\otimes", "\\oslash","\\odot",
	"\\dagger", "\\ddager", "\\amalg", "\\hsize", "\\vsize", "\\displaywidth", "\\hoffset=", "\\voffset=", "\\leq", "\\le", "\\prec", "\\preceq", "\\ll", "\\subset", "\\subseteq", "\\sqsubseteq", "\\in", "\\vdash", "\\smile", "\\frown", "\\propto",
	"\\geq", "\\ge", "\\succ", "\\succeq", "\\gg", "\\supset", "\\supseteq", "\\sqsubseteq", "\\notin", "\\dashv", "\\mid", "\\parallel", "\\equiv", "\\sim", "\\simeq", "\\asymp", "\\approx", "\\cong", "\\bowtie", "\\ni", "\\owns", "\\models",
	"\\doteq", "\\perp", "\\leftarrow", "\\gets", "\\Leftarrow", "\\rightarrow", "\\to", "\\Rightarrow", "\\leftrightarrow", "\\Leftrightarrow", "\\mapsto", "\\hookleftarrow", "\\uparrow", "\\downarrow", "\\updownarrow", "\\nearrow", "nwarrow",
	"\\ne", "\\longleftarrow", "\\Longleftarrow", "\\longrightarrow", "\\Longrightarrow", "\\longleftrightarrow", "\\Longleftrightarrow", "\\longmapsto", "\\hookrightarrow", "\\Uparrow", "\\Downarrow", "\\Updownarrow", "\\searrow", "\\swarrow",
	"\\buildrel", "\\over", "\\lbrack", "\\rbrack", "\\vert", "\\Vert", "[\\![", "]\\!]", "\\lbrace", "\\rbrace", "\\{", "\\}", "\\lfloor", "\\rfloor", "(\\!(", ")\\!)", "\\langle", "\\rangle", "\\lceil", "\\rceil", "\\langle\\!\\langle", "\\rangle\\!\\rangle",
	"\\left", "\\right", "\\bigl", "\\bigr", "\\Bigl", "\\Bigr", "\\biggl", "\\biggr", "\\everypar", "\\everymath", "\\everydisplay", "\\everycr", "\\hat", "\\widehat", "\\check", "\\tilde", "\\widetilde", "\\acute", "\\grave", "\\dot", "\\ddot", "\\breve",
	"\\bar", "\\vec", "\\^", "\\'", "\\~", "\\`", "\\.", "\\\"", "\\u", "\\-", "\\skew", "\\overline", "\\underline", "\\sqrt", "\\root", "\\of", "\\atop", "\\choose", "\\brace", "\\brack", "\\displaystyle", "\\textstyle", "\\scriptstyle", "\\scriptscriptstyle",
	"\\arccos", "\\arcsin", "\\arctan", "\\arg", "\\pmod", "\\bmod", "cos", "\\cosh", "\\cot", "\\coth", "\\csc", "\\deg", "\\det", "\\dim", "\\exp", "\\gcd", "\\hom", "\\inf", "\\ker", "\\lg", "\\lim", "\\liminf", "\\limsup", "\\ln",
	"\\log", "\\max", "\\min", "\\Pr", "\\sec", "\\sin", "\\sinh", "\\sup", "\\tan", "\\tanh", "\\footnote", "\\topinsert", "\\pageinsert", "\\midinsert", "\\underbar", "\\endinsert", "\\day", "\\month", "\\year", "\\jobname", "\\romannumeral",
	"\\uppercase", "\\lowercase", "\\dots", "\\cdots", "\\vdots", "\\ldots", "\\ddots", "\\hrulefill", "\\rightarrowfill", "\\leftarrowfill", "\\dotfill", "\\leaders", "\\hskip", "\\leaders", "\\hfill", "\\rm", "\\sl", "\\bf", "\\it", "\\tt", "\\/",
	"\\magnification", "\\magstep", "\\magstephalf", "\\font", "\\char", "\\settabs", "\\settabs\\", "\\columns", "\\cr", "\\halign", "\\halign to", "\\openup", "\\noalign", "\\tabskip=", "\\omit", "\\span", "\\multispan", "\\hidewidth", "\\crcr",
	"\\hbox to", "\\vbox to", "\\vtop to", "\\vcenter to", "\\rlap", "\\llap", "\\hfuzz", "\\vfuzz", "\\overfullrule", "\\indent", "\\noindent", "\\parindent=", "\\displayindent=", "\\leftskip=", "\\rightskip=", "\\narrower", "\\item", "\\itemitem",
	"\\hangindent=", "\\hangafter=", "\\parshape=", "\\nopagenumbers", "\\pageno", "\\pageno=", "\\folio", "\\footline", "\\headline", "\\advance", "\\def", "\\cs", "\\let", "\\long", "\\outer", "\\global", "\\gdef", "\\edef", "\\xdef", "\\noexpand",
	"\\expandafter", "\\futurelet", "\\csname", "\\endcsname", "\\string", "\\number", "\\the", "\\ifnum", "\\ifdim", "\\ifodd", "\\ifmmode", "\\if", "\\ifx", "\\ifeof", "\\iftrue", "\\iffalse", "\\ifcase", "\\else", "\\fi", "\\loop", "\\repeat",
	"\\newif", "\\ifblob", "\\blobtrue", "\\blobfalse", "\\quad", "\\qquad", "\\thinspace", "\\enspace", "\\enskip", "\\hfil", "\\hfill", "\\mskip", "\\vskip", "\\vfil", "\\vfill", "\\struct", "\\phantom", "\\vphantom", "\\hphantom", "\\smash", "lower",
	"\\raise", "\\hbox", "\\moveleft", "\\moveright", "\\vbox", "\\smallskip", "\\medskip", "\\bigskip", "\\smallbreak", "\\medbreak", "\\bigbreak", "\\filbreak", "\\baselineskip =","\\baselineskip = 12pt", "\\baselineskip = 18pt", "\\baselineskip = 24pt",
	"\\openup", "\\jot", "\\raggedright", "\\raggedbottom", "\\matrix", "\\pmatrix", "\\bordermatrix", "\\overbrace", "\\underbrace", "\\eqno", "\\leqno", "\\eqalign", "\\eqalignno", "\\leqalignno", "\\displaylines", "\\cases", "\\noalign", "\\openup", NULL
};

char *LaTeXExtensions[] = {".ltx", NULL};
char *LaTeXSyntax[] = {
	"\\alpha", "\\beta", "\\gamma", "\\delta", "\\epsilon", "\\varepsilon", "\\zeta", "\\eta", "\\theta", "\\vartheta", "\\iota", "\\kappa", "\\lambda", "\\mu", "\\nu", "\\xi", "\\o", "\\pi", "\\varpi", "\\rho", "\\varrho", "\\sigma",
	"\\varsigma", "\\tau", "\\upsilon", "\\phi", "\\varphi", "\\chi", "\\psi", "\\omega", "\\Alpha", "\\Beta", "\\Gamma", "\\Delta", "\\Epsilon", "\\Zeta", "\\Eta", "\\Theta", "\\Iota", "\\Kappa", "\\Lambda", "\\Mu", "\\Nu", "\\Xi", "\\O",
	"\\Pi", "\\Sigma", "\\Rho", "\\Tau", "\\Upsilon", "\\Phi", "\\Chi", "\\Psi", "\\Omega", "\\aleph", "\\hbar", "\\imath", "\\jmath", "\\ell", "\\wp", "\\Re", "\\Im", "\\partial", "\\infty", "\\prime", "\\emptyset", "\\nabla", "\\surd",
	"\\top", "\\bot", "\\|", "\\angle", "\\triangle", "\\backslash", "\\forall", "\\exists", "\\neg", "\\not", "\\flat", "\\natural", "\\sharp", "\\clubsuit", "\\diamondsuit", "\\heartsuit", "\\spadesuit", "\\sum", "\\prod", "\\coprod", "\\int",
	"\\oint", "\\bigcap", "\\bigcup", "\\bigsqcup", "\\bigvee", "\\bigwedge", "\\bigodot", "\\bigotimes", "\\bigoplus", "\\biguplus", "\\pm", "\\mp", "\\setminus", "\\cdot", "\\times", "\\ast", "\\star", "\\diamond", "\\circ", "\\bullet", "\\div",
	"\\cap", "\\cup", "\\uplus", "\\sqcap", "\\sqcup", "\\triangleleft", "\\triangleright", "\\wr", "\\bigcirc", "\\bigtriangleup", "\\bigtriangledown", "\\vee", "\\lor", "\\wedge", "\\land", "\\oplus", "\\ominus", "\\otimes", "\\oslash","\\odot",
	"\\dagger", "\\ddager", "\\amalg", "\\hsize", "\\vsize", "\\displaywidth", "\\hoffset=", "\\voffset=", "\\leq", "\\le", "\\prec", "\\preceq", "\\ll", "\\subset", "\\subseteq", "\\sqsubseteq", "\\in", "\\vdash", "\\smile", "\\frown", "\\propto",
	"\\geq", "\\ge", "\\succ", "\\succeq", "\\gg", "\\supset", "\\supseteq", "\\sqsubseteq", "\\notin", "\\dashv", "\\mid", "\\parallel", "\\equiv", "\\sim", "\\simeq", "\\asymp", "\\approx", "\\cong", "\\bowtie", "\\ni", "\\owns", "\\models",
	"\\doteq", "\\perp", "\\leftarrow", "\\gets", "\\Leftarrow", "\\rightarrow", "\\to", "\\Rightarrow", "\\leftrightarrow", "\\Leftrightarrow", "\\mapsto", "\\hookleftarrow", "\\uparrow", "\\downarrow", "\\updownarrow", "\\nearrow", "nwarrow",
	"\\ne", "\\longleftarrow", "\\Longleftarrow", "\\longrightarrow", "\\Longrightarrow", "\\longleftrightarrow", "\\Longleftrightarrow", "\\longmapsto", "\\hookrightarrow", "\\Uparrow", "\\Downarrow", "\\Updownarrow", "\\searrow", "\\swarrow",
	"\\buildrel", "\\over", "\\lbrack", "\\rbrack", "\\vert", "\\Vert", "[\\![", "]\\!]", "\\lbrace", "\\rbrace", "\\{", "\\}", "\\lfloor", "\\rfloor", "(\\!(", ")\\!)", "\\langle", "\\rangle", "\\lceil", "\\rceil", "\\langle\\!\\langle", "\\rangle\\!\\rangle",
	"\\left", "\\right", "\\bigl", "\\bigr", "\\Bigl", "\\Bigr", "\\biggl", "\\biggr", "\\everypar", "\\everymath", "\\everydisplay", "\\everycr", "\\hat", "\\widehat", "\\check", "\\tilde", "\\widetilde", "\\acute", "\\grave", "\\dot", "\\ddot", "\\breve",
	"\\bar", "\\vec", "\\^", "\\'", "\\~", "\\`", "\\.", "\\\"", "\\u", "\\-", "\\skew", "\\overline", "\\underline", "\\sqrt", "\\root", "\\of", "\\atop", "\\choose", "\\brace", "\\brack", "\\displaystyle", "\\textstyle", "\\scriptstyle", "\\scriptscriptstyle",
	"\\arccos", "\\arcsin", "\\arctan", "\\arg", "\\pmod", "\\bmod", "cos", "\\cosh", "\\cot", "\\coth", "\\csc", "\\deg", "\\det", "\\dim", "\\exp", "\\gcd", "\\hom", "\\inf", "\\ker", "\\lg", "\\lim", "\\liminf", "\\limsup", "\\ln",
	"\\log", "\\max", "\\min", "\\Pr", "\\sec", "\\sin", "\\sinh", "\\sup", "\\tan", "\\tanh", "\\footnote", "\\topinsert", "\\pageinsert", "\\midinsert", "\\underbar", "\\endinsert", "\\day", "\\month", "\\year", "\\jobname", "\\romannumeral",
	"\\uppercase", "\\lowercase", "\\dots", "\\cdots", "\\vdots", "\\ldots", "\\ddots", "\\hrulefill", "\\rightarrowfill", "\\leftarrowfill", "\\dotfill", "\\leaders", "\\hskip", "\\leaders", "\\hfill", "\\rm", "\\sl", "\\bf", "\\it", "\\tt", "\\/",
	"\\magnification", "\\magstep", "\\magstephalf", "\\font", "\\char", "\\settabs", "\\settabs\\", "\\columns", "\\cr", "\\halign", "\\halign to", "\\openup", "\\noalign", "\\tabskip=", "\\omit", "\\span", "\\multispan", "\\hidewidth", "\\crcr",
	"\\hbox to", "\\vbox to", "\\vtop to", "\\vcenter to", "\\rlap", "\\llap", "\\hfuzz", "\\vfuzz", "\\overfullrule", "\\indent", "\\noindent", "\\parindent=", "\\displayindent=", "\\leftskip=", "\\rightskip=", "\\narrower", "\\item", "\\itemitem",
	"\\hangindent=", "\\hangafter=", "\\parshape=", "\\nopagenumbers", "\\pageno", "\\pageno=", "\\folio", "\\footline", "\\headline", "\\advance", "\\def", "\\cs", "\\let", "\\long", "\\outer", "\\global", "\\gdef", "\\edef", "\\xdef", "\\noexpand",
	"\\expandafter", "\\futurelet", "\\csname", "\\endcsname", "\\string", "\\number", "\\the", "\\ifnum", "\\ifdim", "\\ifodd", "\\ifmmode", "\\if", "\\ifx", "\\ifeof", "\\iftrue", "\\iffalse", "\\ifcase", "\\else", "\\fi", "\\loop", "\\repeat",
	"\\newif", "\\ifblob", "\\blobtrue", "\\blobfalse", "\\quad", "\\qquad", "\\thinspace", "\\enspace", "\\enskip", "\\hfil", "\\hfill", "\\mskip", "\\vskip", "\\vfil", "\\vfill", "\\struct", "\\phantom", "\\vphantom", "\\hphantom", "\\smash", "lower",
	"\\raise", "\\hbox", "\\moveleft", "\\moveright", "\\vbox", "\\smallskip", "\\medskip", "\\bigskip", "\\smallbreak", "\\medbreak", "\\bigbreak", "\\filbreak", "\\baselineskip =","\\baselineskip = 12pt", "\\baselineskip = 18pt", "\\baselineskip = 24pt",
	"\\openup", "\\jot", "\\raggedright", "\\raggedbottom", "\\matrix", "\\pmatrix", "\\bordermatrix", "\\overbrace", "\\underbrace", "\\eqno", "\\leqno", "\\eqalign", "\\eqalignno", "\\leqalignno", "\\displaylines", "\\cases", "\\noalign", "\\openup", 
	"\\documentclass", "\\usepackage", "\\title", "\\author", "\\date", "\\maketitle", "\\tableofcontents", "\\section", "\\subsection", "\\subsubsection", "\\begin", "\\end", "\\caption", "\\label", "\\ref", "\\cite", "\\bibliographystyle", "bibliography",
	"\\newline", "\\newpage", "\\linebreak", "\\newcommand", "\\renewcommand", "\\newenvironnement", "\\textbf", "\\textit", "\\large", "\\small", "\\emph", "\\node", "\\nodeconnect", "\\ex",NULL
};

char *PHPExtensions[] = {".php", NULL};
char *PHPKeywords[] = {
	"abstract", "and", "array", "as", "break", "callable", "case", "catch", "class", "clone", "const", "continue", "declare", "die", "do", "else", "elseif", "empty", "enddeclare", "endfor", "endforeach", "endif", "endswitch", "endwhile", "extends", "final", 
	"for", "foreach", "function", "global", "goto", "if", "implements", "include", "include_once", "instanceof", "insteadof", "interface", "isset", "list", "namespace", "new", "null", "object", "old", "or", "private", "protected", "public", "require", 
	"require_once", "return", "static", "stdClass", "step", "switch", "throw", "trait", "try", "unset", "use", "var", "while", "xor",
	"bool|", "float|", "int|", "string|", "resource|", NULL
};

char *RubyExtensions[] = {".rb", ".gemspec", NULL};
char *RubyKeywords[] = {
	"BEGIN", "END", "alias", "and", "begin", "break", "case", "class", "def", "defined?", "do", "else", "elsif", "end", "ensure", "false", "for", "if", "in", "include", "initialize", "module", "next", "nil|", "not", "or", "private", "protected", "public", 
	"redo", "rescue", "retry", "return", "self", "super", "then", "true", "undef", "unless", "until", "when", "while", "yield",
	"Array|", "Bignum|", "Class|", "Dir|", "File|", "Float|", "Hash|", "IO|", "Integer|", "Module|", "NilClass|", "Object|", "Proc|", "Range|", "Regexp|", "String|", "Symbol|", "Time|", "TrueClass|", "FalseClass|", NULL
};

char *KotlinExtensions[] = {".kt", ".ktm", ".kts", NULL};
char *KotlinKeywords[] = {
	"abstract", "as", "break", "by", "catch", "class", "continue", "const", "constructor", "crossinline", "data", "delegate", "do", "else", "enum", "extends", "false", "final", "finally", "for", "fun", "if", "implements", "import", "in", "infix", "init", 
	"inner", "interface", "internal", "is", "lateinit", "noinline", "null", "object", "open", "out", "override", "package", "private", "protected", "public", "reified", "return", "sealed", "static", "super", "suspend", "tailrec", "this", "throw", "true", "try", "typealias", "val", "var", "when", "while", "yield",
	"Any|", "Boolean|", "Byte|", "Char|", "Double|", "Float|", "Int|", "Long|", "Nothing|", "Short|", "String|", "Unit|", NULL
};

char *TextExtensions[] = {".txt", NULL};
char *BinaryExtensions[] = {".AppImage", NULL};
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
		NULL, //no keywords
		NULL, NULL, NULL, //no comments
		NoHighlight //no highlight
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
	},
	{
		"TeX",
		TeXExtensions,
		TeXSyntax,
		NULL, NULL, NULL,
		HighlightDigits
	},
	{
		"LaTeX",
		LaTeXExtensions,
		LaTeXSyntax,
		NULL, NULL, NULL,
		HighlightDigits
	},
	{
		"PHP",
		PHPExtensions,
		PHPKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
	{
		"Ruby",
		RubyExtensions,
		RubyKeywords,
		"#", "=begin", "=end",
		HighlightDigits | HighlightStrings
	},
	{
		"Kotlin",
		KotlinExtensions,
		KotlinKeywords,
		"//", "/*", "*/",
		HighlightDigits | HighlightStrings
	},
};

#define SyntaxDatabaseEntries (sizeof(SyntaxDatabase) / sizeof(SyntaxDatabase[0])) //track the amount of entries

/* prototypes */

void SetStatusMessage(const char *fmt, ...);
void RefreshScreen();
char *Prompt(char *prompt, void (*callback)(char *, int));

/* terminal */

void die(const char *s){ //error handling function
	write(STDOUT_FILENO, "\x1b[2J", 4); //clear the terminal screen
	write(STDOUT_FILENO, "\x1b[H", 3); //set the cursor
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &config.og_terminal) == -1){ //restore terminal settings
		perror("tcsetattr"); //if it goes wrong exit
		exit(1);
	}
	perror(s); //show the error that occured
	exit(1); //exit
}

void disableRawMode(){
	if (tcsetattr(STDERR_FILENO, TCSAFLUSH, &config.og_terminal) == -1) //restore terminal settings
		die("tcsetattr"); //error handling
}

void enableRawMode(){
	if (tcgetattr(STDIN_FILENO, &config.og_terminal) == -1) die("tcgetattr"); //get the original terminal attributes and store them in config.og_terminal
	atexit(disableRawMode); //exit rawmode when the program stops
	struct termios raw = config.og_terminal; //store original terminal settings in config.og_terminal
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON); //stop certain inputs
	raw.c_oflag &= ~(OPOST); //disable post-processing of output
	raw.c_cflag |= (CS8); //make every character 8 bytes
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //disable certain terminal features that interfere with the program
	raw.c_cc[VMIN] = 0;//set the minimum number of bytes required before read() returns
	raw.c_cc[VTIME] = 1;// Set the maximum time in tenths of a second that read() will wait
	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) die("tcsetattr"); //apply rawmode features to the terminal
}

int ReadKey(){
	int nread;
	char c;
	while((nread=read(STDIN_FILENO, &c, 1)) != 1){ //continue reading until exactly one character is read
		if(nread == -1 && errno != EAGAIN) die("read");//if error handle it
	}
	if(c == '\x1b'){//if an escape charater is read
		char seq[3];
		if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
		if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b'; //attempt to read the two characters after the escape character
		if(seq[0] == '['){ //if the sequence starts with [ it is likely an escape sequence
			if(seq[1] >= '0' && seq[1] <= '9'){ //if the second character is a digit, it could be part of a key code
				if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b'; //read the third character of the sequence
				if(seq[2] == '~'){ //check for specific key codes
					switch(seq[1]){
						case '3': return DEL_KEY; //delete key
						case '5': return PAGE_UP; //page up key
						case '6': return PAGE_DOWN; //page down
					}
				}
			}else{ //if the second character is not a digit, it's a special key
				switch(seq[1]){
					case 'A': return ARROW_UP;
					case 'B': return ARROW_DOWN;
					case 'C': return ARROW_RIGHT;
					case 'D': return ARROW_LEFT;
				}
			}
		}
		return '\x1b'; //if it is nothing special return the escape key
	}else{
		return c; //return the character if it is not a special or escape character
	}
}

int getCursorPosition(int *lines, int *cols){
	char buf[32];
	unsigned int i = 0;
	if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1; //send the ANSI escape sequence to query cursor position
	while(i<sizeof(buf) - 1){ //read response into buffer until 'R' is encountered or buffer is full
		if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
		if (buf[1] == 'R') break; //stop when R is found
		i++;
	}
	buf[i] = '\0'; //null terminate the buffer
	if(buf[0] != '\x1b' || buf[1] != '[') return -1; //check for weird format
	if(sscanf(&buf[2], "%d;%d", lines, cols) != 2) return -1; //parse the data to receive is exact cursor position
	return 0; //return succes
}

int getWindowSize(int *lines, int *cols){
	struct winsize ws;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0){ //attempt to get window size with IOCTL
		if(write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1; //attempt to get window size with cursor position
		return getCursorPosition(lines, cols);
	} else { //if we got the window size assign the values to the winsize struct
		*cols = ws.ws_col;
		*lines = ws.ws_row;
		return 0;
	}
}

/* syntax highlighting */

int is_seperator(int c){
	return isspace(c) || c == '\0' || strchr(",.()+-/*=~%<>[];{}", c) != NULL; //check for seperate character
}

void UpdateSyntax(eline *line){
	line->hl = realloc(line->hl, line->rendersize); //reallocate memory
	memset(line->hl, Normal, line->rendersize); //initialize syntax highlighting
	if(config.syntax == NULL) return; //if no syntax return
	if(config.syntax->flags == NoHighlight) return; //if syntax highlighting is disabled return
	//extract syntax
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
	while(i < line->rendersize){ //iterate through each character
		char c = line->render[i];
		unsigned char prev_hl = (i > 0) ? line->hl[i - 1] : Normal;
		if(scs_len && !in_string){ //check for single line comment
			if(!strncmp(&line->render[i], scs, scs_len)){
				memset(&line->hl[i], Comment, line->rendersize - i); //highlight the rest of the line
				break;
			}
		}
		if(mcs_len && mce_len && !in_string && !in_string){//check for multiline comment
			if(in_comment){
				line->hl[i] = MultiLineComment;
				if(!strncmp(&line->render[i], mce, mce_len)){ //end of multiline comment detected
					memset(&line->hl[i], MultiLineComment, mce_len);
					i += mce_len;
					in_comment = 0;
					prev_sep = 1;
					continue;
				}else{
					i++;
					continue;
				}
			}else if(!strncmp(&line->render[i], mcs, mcs_len)){ //start of multiline comment detected
				memset(&line->hl[i], MultiLineComment, mcs_len);
				i += mcs_len;
				in_comment = 1;
				continue;
			}
		}
		if(config.syntax->flags & HighlightStrings){ //check for strings
			if(in_string){
				line->hl[i] = StringColor;
				if(c == '\\' && i + 1 < line->rendersize){//handle escape sequences
					line->hl[i + 1] = StringColor;
					i += 2;
					continue;
				}
				if(c == in_string) in_string = 0;//end of string detected
				i++;
				prev_sep = 1;
				continue;
			}else{
				if(c == '"' || c == '\''){//start of string detected
					in_string = c;
					line->hl[i] = StringColor;
					i++;
					continue;
				}
			}
		}
		if(config.syntax->flags & HighlightDigits){//highlight digits
			if((isdigit(c) && (prev_sep || prev_hl == Number)) || (c == '.' && prev_hl == Number)) {
				line->hl[i] = Number;
				i++;
				prev_sep = 0;
				continue;
			}
		}
		if(prev_sep){ //check for keywords and identifiers
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
				prev_sep = 0;
				continue;
			}
		}
		prev_sep = is_seperator(c);
		i++;
	}
	int changed = (line->hl_open_comment != in_comment); //check for change
	line->hl_open_comment = in_comment;
	if(changed && line->idx + 1 < config.numlines) //if change recursively update syntax
		UpdateSyntax(&config.line[line->idx + 1]);
}

int SyntaxToColor(int hl){ //return the color that matches with the config
	switch(hl){
		case Comment:
		case MultiLineComment: return UConfig.Comment_Color;
		case Keywords: return UConfig.Keywords_Color;
		case Types: return UConfig.Types_Color;
		case StringColor: return UConfig.StringColor_;
		case Number: return UConfig.Number_Color;
		case Match: return UConfig.Match_Color;
		default: return 37;
	}
}

void SelectSyntaxHighlight(){
	config.syntax = NULL; //reset syntax configuration
	if(config.filename == NULL) return; //no filename, return
	char *ext = strrchr(config.filename, '.'); //find extension
	for(unsigned int j = 0; j < SyntaxDatabaseEntries; j++){ //look through the database
		struct Syntax *s = &SyntaxDatabase[j];
		unsigned int i = 0;
		while(s->filematch[i]){ //check for match in the database
			int is_ext = (s->filematch[i][0] == '.');
			if((is_ext && ext && !strcmp(ext, s->filematch[i])) || (!is_ext && strstr(config.filename, s->filematch[i]))){
				config.syntax = s; //set the syntax highlighting configuration
				int fileline;
				for(fileline = 0; fileline < config.numlines; fileline++){ //update the syntax for every single line
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
	int renderx = 0; //initialize the renderx position to 0
	int j;
	for(j = 0; j < x; j++){ //iterate through each character up to position x
		if(line->chars[j] == '\t')
			renderx += (JuraTabStop - 1) - (renderx % JuraTabStop); //if the character is a tab, calculate the space it represent and update renderx accordingly
		renderx++; //increment renderx for each character (including tabs)
	}
	return renderx; //return the calculated renderx position
}

int LineRenderxToX(eline *line, int renderx){
	int cur_renderx = 0; //initialize the current renderx position to 0
	int x;
	for(x = 0; x < line->size; x++){ //iterate through each character in the line's raw text
		if(line->chars[x] == '\t')
			cur_renderx += (JuraTabStop -1) - (cur_renderx % JuraTabStop); //if the character is a tab, calculate the number of spaces it represents and increment the current renderx position accordingly
		cur_renderx++;  //increment current renderx for each character (including tabs)
		if(cur_renderx > renderx){ //check if the current renderx position exceeds the target renderx position
			return x; //return the character index when the target renderx position is reached
		} 
	}
	return x;//return the last character index if the target renderx position is not reached
}

void UpdateLine(eline *line){
	int tabs = 0;
	int j;
	for(j = 0; j < line->size; j++) //count the number of tab characters in the line
		if(line->chars[j] == '\t') tabs++;
	//free the previous render buffer and allocate memory for the new one
	free(line->render);
	line->render = malloc(line->size + tabs*(JuraTabStop - 1) + 1);
	int idx = 0;//initialize the index for the new render buffer
	for(j = 0; j < line->size; j++){ //iterate through each character in the line's raw text
		if(line->chars[j] == '\t'){
			line->render[idx++] = ' '; //replace tab characters with spaces, accounting for tab stops
			while(idx % JuraTabStop != 0) line->render[idx++] = ' '; //fill in spaces until the next tab stop
		}else{
			line->render[idx++] = line->chars[j]; //copy non-tab characters directly to the render buffer
		}
	}
	line->render[idx] = '\0';//null terminate the buffer
	line->rendersize = idx; //update the size of the render buffer
	UpdateSyntax(line); //update the syntax highlighting for the line
}

void InsertLine(int at, char *s, size_t len){
	if(at < 0 || at > config.numlines) return; //ensure at is within the current line count
	config.line = realloc(config.line, sizeof(eline) * (config.numlines + 1)); //increase the memory allocated for lines by one
	memmove(&config.line[at + 1], &config.line[at], sizeof(eline) * (config.numlines - at)); //move existing lines to make room for the new one
	for(int j = at + 1; j <= config.numlines; j++) config.line[j].idx++; //update the line indicator
	//set up the new line
	config.line[at].idx = at;
	config.line[at].size = len;
	config.line[at].chars = malloc(len + 1);
	memcpy(config.line[at].chars, s, len);
	config.line[at].chars[len] = '\0';//null terminate the line
	//initialize render and syntax highlighting buffers for the new line
	config.line[at].rendersize = 0;
	config.line[at].render = NULL;
	config.line[at].hl = NULL;
	config.line[at].hl_open_comment = 0;
	UpdateLine(&config.line[at]);//update render and syntax highlighting for the new line
	config.numlines++; //increase the number of lines in the line count
	config.mod++; //make sure jura knows the file has been modified
}

void FreeLine(eline *line){ //free memory for a line
	free(line->render);
	free(line->chars);
	free(line->hl);
}

void RemoveLine(int at){
	if(at < 0 || at >= config.numlines) return; //make sure the at is within the current line count
	FreeLine(&config.line[at]); //free memory for a line
	memmove(&config.line[at], &config.line[at + 1], sizeof(eline) * (config.numlines - at - 1)); //move existing lines
	for(int j = at; j < config.numlines - 1; j++) config.line[j].idx--; //update the line indicator
	config.numlines--; //decrease the number of lines in the line counter
	config.mod++; //make sure jura knows the file has been modified
}

void LineInsertChar(eline *line, int at, int c){
	if(at < 0 || at > line->size) at = line->size; //ensure that at is within the line count and adjust at to the end of the line if it is out of bounds
	line ->chars = realloc(line->chars, line->size + 2); //make space for the new character
	memmove(&line->chars[at + 1], &line->chars[at], line->size - at + 1); //move the characters around
	line->size++; //increase the size of the line to make space for the new character
	line->chars[at] = c; //insert the character at the at position
	UpdateLine(line); //update rendering and syntax highlighting
	config.mod++; //make sure jura knows the file has been modified
}

void LineAppendString(eline *line, char *s, size_t len){
	line->chars = realloc(line->chars, line->size + len + 1); //increase the size of the buffer to add space for the string
	memcpy(&line->chars[line->size], s, len); //copy the new string to the end of the line's character buffer
	line->size +=len; //increase the size of the line
	line->chars[line->size] = '\0'; //null terminate the character buffer
	UpdateLine(line); //update the rendering and syntax highlighting
	config.mod++; //make sure jura knows the file has been modified
}

void LineRemoveChar(eline *line, int at){
	if(at < 0 || at >= line->size) return; //make sure at is possible within the line size
	memmove(&line->chars[at], &line->chars[at + 1], line->size - at); //move the all characters to remove a character
	line->size--; //decrease the size of the line
	UpdateLine(line); //update the rendering and syntax highlighting
	config.mod++; //make sure jura knows the file has been modified
}

/* editor operations */

void InsertChar(int c){
	if(config.y == config.numlines){ //check if the cursor is at end of the document
		InsertLine(config.numlines, "", 0); //if it is insert a new line at the end of the document
	}
	LineInsertChar(&config.line[config.y], config.x, c); //insert the typed character at the current cursor position 
	config.x++; //increase the x position of the cursor by one
}

void InsertNewline(){
	if(config.x==0){ //if the cursor is at the beginning of the line (config.x == 0), insert a new empty line before the current line (config.y)
		InsertLine(config.y, "", 0);
	}else{ //otherwise, split the current line at the cursor position (config.x)
		eline *line = &config.line[config.y];
		InsertLine(config.y + 1, &line->chars[config.x], line->size - config.x);
		line = &config.line[config.y];
		line->size = config.x;
		line->chars[line->size] = '\0';
		UpdateLine(line);
	}
	config.y++; //move the cursor to the beginning of the newly created line
	config.x = 0;
}

void RemoveChar(){
	if(config.y == config.numlines) return; //if the cursor is at the end of the document there is nothing to remove so return
	if(config.x == 0 && config.y == 0) return; //if the cursor is at the beginning of the document there is nothing to remove so return
	eline *line = &config.line[config.y];
	if(config.x > 0){ //if the cursor is not at the beginning of a line remove the character
		LineRemoveChar(line, config.x - 1);
		config.x--;
	}else{ //if the cursor is at the beginning of a line remove the line
		config.x = config.line[config.y - 1].size;
		LineAppendString(&config.line[config.y - 1], line->chars, line->size); //append the rest of the line to the previous
		RemoveLine(config.y);
		config.y--;
	}
}

/* file I/O */

char *LinesToString(int *buflen){
	int totlen = 0;
	int j;
	for(j = 0; j < config.numlines; j++) //calculate the total length of the resulting string
		totlen += config.line[j].size + 1; //add the size of each line and the newline character
	*buflen = totlen;
	char *buf = malloc(totlen); //allocate memory for the buffer
	char *p = buf;
	for(j = 0; j < config.numlines; j++){ //concentrate each line's content into a string
		memcpy(p, config.line[j].chars, config.line[j].size); //copy the string into the buffer
		p += config.line[j].size; //move the pointer to the end of the copied characters
		*p = '\n'; //add the newline character
		p++;
	}
	return buf; //return the created string buffer
}

void Open(char *filename){
	free(config.filename); //free the previous file, if any
	config.filename = strdup(filename); //set a new file name
	SelectSyntaxHighlight(); //select syntax based on file extensions
	FILE *fp = fopen(filename, "r"); //open the file for reading
	if(!fp) die("fopen"); //error if the file cannot be opened
	//initialize everything for reading the file
	char *line = NULL;
	size_t linecap = 0;
	ssize_t lijnen;
	while((lijnen = getline(&line, &linecap, fp)) != -1){ //read the file until the end is reached
		while(lijnen > 0 && (line[lijnen - 1] == '\n' || line[lijnen - 1] == '\r'))
			lijnen--;
		InsertLine(config.numlines, line, lijnen); //insert the lines into the editor
	}
	free(line); //free up memory
	fclose(fp); //close the file
	config.mod = 0; //if the file was just openend it has not been modified
}

void Save(){
	if(config.filename == NULL){ //check if the file has a name
		config.filename = Prompt("Save as: %s (ESC to cancel)", NULL); //if not make the user input one
		if(config.filename == NULL){ //if we still dont get a filename the user pressed ESC
			SetStatusMessage("Save aborted");
			return;
		}
		SelectSyntaxHighlight(); //select the syntax for based on the file extension
	}
	int len;
	char *buf = LinesToString(&len); //get the total length of the text buffer as a string and store it in len
	int fd = open(config.filename, O_RDWR | O_CREAT, 0644); //open the file for read/write access, creating it if it doesn't exist
	if(fd != -1){ //check if the file was openend succesfully
		if(ftruncate(fd, len) != -1){ //truncate the file to the specified length
			if(write(fd, buf, len) == len){ //write the contents of the buffer to the file
				close(fd); //close the file
				free(buf); //free the buffer
				config.mod = 0; //if the file has saved it has no unsaved modifications
				SetStatusMessage("%d bytes written to disk", len);
				return;
			}
		}
		close(fd); //if there was an error during truncating close the file
	}
	free(buf); //free the memory if the file cant be opened
	SetStatusMessage("Can't save I/O error %s", strerror(errno)); //throw an error
}

/* find */

void FindCallback(char *query, int key){
	//keep track of important things
	static int last_match = -1;
	static int direction = 1;
	static int saved_hl_line;
	static char *saved_hl = NULL;
	if(saved_hl){ //if there is a saved highlighting, restore it
		memcpy(config.line[saved_hl_line].hl, saved_hl, config.line[saved_hl_line].rendersize);
		free(saved_hl);
		saved_hl = NULL;
	}
	//handle special keys
	if(key == '\r' || key == '\x1b'){ //escape key
		last_match = -1;
		direction = 1;
		return;
	}else if(key == ARROW_RIGHT || ARROW_DOWN){ //arrow keys
		direction = 1;
	}else if(key == ARROW_LEFT || ARROW_UP){
		direction = -1;
	}else{ //any other key
		last_match = -1;
		direction = 1;
	}
	if(last_match == -1) direction = 1; //if no match was found or the direction was reset start searching from the begining
	//search for the next occurence
	int current = last_match;
	int i;
	for(i = 0; i < config.numlines; i++){
		current += direction;
		if(current == -1) current = config.numlines - 1;
		else if(current == config.numlines) current = 0;
		eline *line = &config.line[current];
		//find a match
		char *match = strstr(line->render, query);
		if(match){ //if a match was found, update the cursor and highlighting
			last_match = current;
			config.y = current;
			config.x = LineRenderxToX(line, match - line->render);
			config.offline = config.numlines;
			saved_hl_line = current;
			saved_hl = malloc(line->rendersize);
			memcpy(saved_hl, line->hl, line->rendersize);
			memset(&line->hl[match - line->render], Match, strlen(query));
			break; //exit the loop once a match is found
		}
	}
}

void Find(){
	//save the current cursor position
	int saved_cx = config.x;
	int saved_cy = config.y;
	int saved_coloff = config.coloff;
	int saved_offline = config.offline;
	char *query = Prompt("Search: %s (ESC|Arrow keys|Enter)", FindCallback); //enter in the query and find the matches with the FindCallBack function
	if(query){
		free(query); //if the query is not NULL, the user pressed enter, so free the memory for the query
	}else { //if the query is NULL, the user pressed ESC, so restore the cursor
		config.x = saved_cx;
		config.y = saved_cy;
		config.coloff = saved_coloff;
		config.offline = saved_offline;
	}
}

void Golf(){
	char *query = Prompt("Go to line: %s (ESC to cancel)", NULL);//make the user enter in a prompt and store it
	if(query == NULL) return;//if the query is NULL the user pressed ESC so return
	int i = atoi(query);//extract a valid integer from the query (an input that is not a number becomes zero)
	if(i == 0 || i > config.numlines){//check if the line number is possible(not zero and not exceeding the amount of lines in the file)
		SetStatusMessage("Not a valid line number");//tell the user why the function failed
		free(query);//free the query memory
		return;//exit the function
	}else if(i > 0 && i < config.numlines){//check if the line number is valid
		config.y = i - 1;//move the cursor to that line
		config.x = 0;
	}
	free(query);//free the query memory
}

/* append buffer */

struct buffer{ //define a buffer which can hold a bunch of characters and the length
	char *b;
	int len;
};
//define a buffer macro with an empty character array, and a length of zero
#define StartBuffer {NULL, 0}

void AttachBuffer(struct buffer *buff, const char *s, int len){ //attach a character array to the buffer
	char *new = realloc(buff->b, buff->len + len); //reallocate the buffer
	if(new == NULL) return; //if reallocation fails, return without modifying the buffer
	memcpy(&new[buff->len], s, len); //copy the data to the buffer array
	buff->b = new; //update the charater array
	buff->len += len; //update the length
}

void FreeBuffer(struct buffer *buff){
	free(buff->b); //free memory for the character array
}

/* output */

void Scroll(){
	config.renderx = 0; //reset the horizontal rendering position
	if(config.y < config.numlines){ //calculate the rendering position for the current cursor line
		config.renderx = LineXToRenderx(&config.line[config.y], config.x);
	}
	if(config.y < config.offline){ //addjust vertical offset if cursor is above visible screen
		config.offline = config.y;
	}
	if(config.y >= config.offline + config.screenlines){ //adjust vertical offset is cursor is below visible screen
		config.offline = config.y - config.screenlines + 1;
	}
	if(config.renderx < config.coloff){ //adjust horizontal offset if the cursor is left of the screen
		config.coloff = config.renderx;
	}
	if(config.renderx >= config.coloff + config.screencols){ //adjust horizontal offset if the cursor is right of the screen
		config.coloff = config.renderx - config.screencols + 1;
	}
}

void DrawLines(struct buffer *buff){
	int y;
	for (y = 0; y < config.screenlines; y++){
		int fileline = y + config.offline;
		if(fileline >= config.numlines){ //check if the current file line is beyond the number of the lines in the buffer
		if(config.numlines == 0 && y == config.screenlines/3){ //construct a welkom message
			char welcome[80];
			int welkom = snprintf(welcome, sizeof(welcome), "jura  -- version %s", CurrentJuraVersion);
			if(welkom > config.screencols) welkom = config.screencols;
			int padding = (config.screencols - welkom) / 2;
			if(padding){
				AttachBuffer(buff, UConfig.First_Char, 1);
				padding--;
			}
			while(padding--) AttachBuffer(buff, " ", 1);
			AttachBuffer(buff, welcome, welkom);
		}else {
		AttachBuffer(buff, UConfig.First_Char, 1); //display empty lines
		}
		}else{
			int len = config.line[fileline].rendersize - config.coloff; //calculate the length of the line to be displayed
			if(len < 0) len = 0;
			if(len > config.screencols) len = config.screencols;
			char *c = &config.line[fileline].render[config.coloff]; //retrieve pointers to the line's render and highlight arrays
			unsigned char *hl = &config.line[fileline].hl[config.coloff];
			int current_color = -1; //initialize current color to -1
			int j;
			for(j = 0; j < len; j++){ //iterate over each character in the line
				if(iscntrl(c[j])){ //if the character is a control character, display it with inverted colors
					char sym = (c[j] <= 26) ? '@' + c[j] : '?';
					AttachBuffer(buff, "\x1b[7m", 4); //start inverted colors
					AttachBuffer(buff, &sym, 1);
					AttachBuffer(buff, "\x1b[m", 3); //reset colors
					if(current_color != -1){
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", current_color);
						AttachBuffer(buff, buf, clen); //restore previous colors
					}
				}else if(hl[j] == Normal){
					if(current_color != -1){ //if the character's highlight is normal, display it with default colors
						AttachBuffer(buff, "\x1b[39m", 5); //reset foreground color
						current_color = -1;
					}
					AttachBuffer(buff, &c[j], 1);
				} else{
					int color = SyntaxToColor(hl[j]); //if a certain syntax color is specified, display the character with that color
					if(color != current_color){
						current_color = color;
						char buf[16];
						int clen = snprintf(buf, sizeof(buf), "\x1b[%dm", color);
						AttachBuffer(buff, buf, clen); //set foreground color
					}
					AttachBuffer(buff, &c[j], 1);
				}
			}
			AttachBuffer(buff, "\x1b[39m", 5); //reset foreground color
		}
		AttachBuffer(buff, "\x1b[K", 3); //clear the line from the cursor position to the end
		AttachBuffer(buff, "\r\n", 2); //move the cursor to the beginning of the next line
	}
}

void DrawStatusBar(struct buffer *buff){
	AttachBuffer(buff, "\x1b[7m", 4); //invert the colors
	//draw the statusbar
	char status[80], rstatus[80];
	int len = snprintf(status, sizeof(status), "%.20s - %d lines %s", config.filename ? config.filename : "[No Name]", config.numlines, config.mod ? "(modified)" : "");
	int rlen = snprintf(rstatus, sizeof(rstatus), "%s | %d/%d", config.syntax ? config.syntax->filetype : "no filetype detected", config.y + 1, config.numlines);
	if(len > config.screencols) len = config.screencols; //limit the status message to fit on the screen
	AttachBuffer(buff, status, len); //add the status message to the buffer
	while(len < config.screencols){ // fill the remaining space with empty characters if necessary
		if(config.screencols - len == rlen){
			AttachBuffer(buff, rstatus, rlen);
			break;
		}else{
			AttachBuffer(buff, " ", 1);
			len++;
		}
	}
	AttachBuffer(buff, "\x1b[m", 3); //reset colors
	AttachBuffer(buff, "\r\n", 2); //make space for the message bar
}

void DrawMessageBar(struct buffer *buff){
	AttachBuffer(buff, "\x1b[K", 3); //clear the message bar
	int msglen = strlen(config.statusmsg); //calculate the length of the message bar
	if(msglen > config.screencols) msglen = config.screencols; //limit the length of the message bar to the screen width
	if(msglen && time(NULL) - config.statusmsg_time < 5) //display the status message if it exists and was recently updated (within 5 seconds)
		AttachBuffer(buff, config.statusmsg, msglen);
}

void RefreshScreen() {
    Scroll(); //ensure the cursor position is within the visible area of the screen
    struct buffer buff = StartBuffer; //start a buffer
    AttachBuffer(&buff, "\x1b[?25l", 6); //hide the cursor
    AttachBuffer(&buff, "\x1b[H", 3); //move the cursor to the home position (top-left corner)
    DrawLines(&buff); //draw the lines on the screen
    DrawStatusBar(&buff); //draw the statusbar on the screen
    DrawMessageBar(&buff); //draw the messagebar
	//draw the cursor back on the screen
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", (config.y - config.offline) + 1, (config.renderx - config.coloff) + 1);
    AttachBuffer(&buff, buf, strlen(buf));
    AttachBuffer(&buff, "\x1b[?25h", 6); //show the cursor
    write(STDOUT_FILENO, buff.b, buff.len); //display the result to the standard output (the terminal)
    FreeBuffer(&buff); //free the memory that was reserved for the buffer
}

void SetStatusMessage(const char *fmt, ...){
	va_list ap; //declare a variable argument list
	va_start(ap, fmt); //initialize the variable argument list with the format string
	vsnprintf(config.statusmsg, sizeof(config.statusmsg), fmt, ap); //format the message and the store it in config.statusmsg
	va_end(ap); //clean up the variable argument list
	config.statusmsg_time = time(NULL);//record the current time as the status message update time
}

/* input */

char *Prompt(char *prompt, void(*callback)(char *, int)){
	size_t bufsize = 128; //initial buffer size for input
	char *buf = malloc(bufsize); //allocate memory for the buffer
	size_t buflen = 0; //current length of the input buffer
	buf[0] = '\0'; //ensure buffer is NULL terminated
	while(1){
		SetStatusMessage(prompt, buf); //set status message to display the prompt and current input
		RefreshScreen(); //refresh the screen to display the current prompt and output
		int c = ReadKey(); //read a key from user input
		if(c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){ //handle backspace and delete characters
			if(buflen != 0) buf[--buflen] = '\0'; //remove the last character from input if buffer is not empty
		//handle escape sequences (for if the user exists the prompt)
		}else if(c == '\x1b'){
			SetStatusMessage(""); //clear the status message
			if(callback) callback(buf, c); //invoke the callback function with the input buffer and the escape key
			free(buf); //free assigned memory
			return NULL; //return NULL to indicate canceling the prompt
		//handle enter keypres (for if the user confirms the input)
		}else if(c == '\r'){
			if(buflen != 0){
				SetStatusMessage(""); //clear the status message
				if(callback) callback(buf, c); //invoke the callback function with the input buffer and the enter key
				return buf; //return the input buffer containing the entered text
			}
		//handle printable characters
		}else if(!iscntrl(c) && c < 128){
			if(buflen == bufsize - 1){
				bufsize *= 2; //double the buffer size if it's about to overflow
				buf = realloc(buf, bufsize); //make the reserved memory bigger for the expanded buffer
			}
			buf[buflen++] = c; //append the character to the input buffer
			buf[buflen] = '\0'; //NULL terminate the buffer
		}
		if(callback) callback(buf, c); //invoke the callback function with the input buffer and the pressed key
	}
}

void MoveCursor(int key){
	eline *line = (config.y >= config.numlines) ? NULL : &config.line[config.y]; //get the current line where the cursor is positioned
	switch (key){
		case ARROW_LEFT:
		if(config.x != 0){ //move cursor left within the line if possible
		config.x--;
		}else if(config.y > 0){ //move cursor to the end of the previous line if on the first character of the current line
			config.y--;
			config.x = config.line[config.y].size;
		}
		break;
		case ARROW_RIGHT:
		if(line && config.x < line->size){ //move cursor right within the line if possible
		config.x++;
		}else if(line && config.x == line->size){ //move cursor to the next line if on the last character of the current line
			config.y++;
			config.x = 0;
		}
		break;
		case ARROW_UP:
		if(config.y != 0){ //move the cursor up a line if possible
		config.y--;
		}
		break;
		case ARROW_DOWN:
		if(config.y < config.numlines){ //move the cursor down a line if possible
		config.y++;
		}
		break;
	}
	line = (config.y >= config.numlines) ? NULL : &config.line[config.y]; //get the current line
	int linelen = line ? line->size : 0;
	if(config.x > linelen){ //make sure the line is in a possible position on the line
		config.x = linelen;
	}
}

void CleanConfig(){ //Free the config memory
	if(config.line){
		for(int i = 0; i < config.numlines; i++){
			FreeLine(&config.line[i]);
		}
		free(config.line);
	}
	free(config.filename);
	if(config.syntax){
		free(config.syntax);
	}
}

void ProcessKeypress(){
	static int quit_times = JuraQuitTimes; //number of quit times allowed before quitting without saving
	int c = ReadKey(); //read the key pressed by the user
	switch (c){
	case '\r': //enter key
		InsertNewline();
		break;
	case CTRL_KEY('q'): // Ctrl + q
		if(config.mod && quit_times > 0){ //if there are any unsaved changed
			SetStatusMessage("WARNING! File has unsaved changes." "Press Ctrl-Q %d more times to quit", quit_times); //display a warning
			quit_times--; //decrease number of times allowed to quit without saving
			return;
		}
		write(STDOUT_FILENO, "\x1b[2J", 4); //clear entire screen
		write(STDOUT_FILENO, "\x1b[H", 3); //move cursor to the top-left corner
		exit(0); //terminate the program
		break;
	case CTRL_KEY('s'): //Ctrl + s
		Save(); 
		SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-G = go to line"); //set the base status message
		break;
	case CTRL_KEY('f'): //Ctrl + f
		Find(); //search for specific text
		SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-G = go to line"); //set the base status message
		break;
	case CTRL_KEY('g'): //Ctrl + g
		Golf(); //go to a specific line
		//the golf function sets a statusmessage so we dont set the base status message
		break;
	case BACKSPACE:
	case CTRL_KEY('h'):
	case DEL_KEY: //backspace, delete, Ctrl + h
		if(c == DEL_KEY) MoveCursor(ARROW_RIGHT); //if the user pressed the delete key, move the cursor right
		RemoveChar(); //remove the character at the current cursor position
		break;
	case PAGE_UP:
	case PAGE_DOWN:
		{
			if(c == PAGE_UP){
				config.y = config.offline; //move cursor to the top line of the screen
			}else if(c == PAGE_DOWN){
				config.y = config.offline + config.screenlines - 1; //move cursor to the bottom line of the screen
			}
			int times = config.screencols; //number of times to move cursor up or down
			while(times--)
				MoveCursor(c == PAGE_UP ? ARROW_UP : ARROW_DOWN); //move cursor up or down
		}
		break;
	case ARROW_UP:
	case ARROW_DOWN:
	case ARROW_LEFT:
	case ARROW_RIGHT:
		MoveCursor(c); //move the cursor in the specified direction
		break;
	case CTRL_KEY('l'):
	case '\x1b': //Ctrl + l and escape key
		break; //do nothing
	default:
		InsertChar(c); //insert the character at the current cursor position
		break;
	}
	quit_times = JuraQuitTimes; //reset quit times
}

/* init */

void init(){ //initialize the coniguration parameters
	config.x = 0; //cursor column position
	config.y = 0; //cursor line position
	config.renderx = 0; //horizontal rendering position
	config.offline = 0; //vertical scroll offset
	config.coloff = 0; //horizontal scroll offset
	config.numlines = 0; //total number of lines in the file
	config.line = NULL; //array of lines
	config.mod = 0; //flag indicating if modifications have been made
	config.filename = NULL; //name of the file
	config.statusmsg[0] = '\0'; //buffer for status message
	config.statusmsg_time = 0; //time at which the status message was last update
	config.syntax = NULL; //syntax highlighting rules
	//get window size to determine screen dimensions
	if (getWindowSize(&config.screenlines, &config.screencols) == -1) die("getWindowSize"); //if jura cant get the window size it terminates the program and throws and error
	config.screenlines -= 2; //adjust the screen height to accomodate message and status bar
}

int main(int argc, char *argv[]){
	enableRawMode(); //enable raw mode for terminal output
	init(); //initialize config parameters
	char *home_dir = getenv("HOME"); //get the home directory path
	char config_path[256]; //buffer for the configuration file path
	strcpy(config_path, home_dir); //copy the home directory path to config_path
    strcat(config_path, "/config.jura"); //add jura's config file path to config_path
	if(argc == 3 && strcmp(argv[1], "setconfig") == 0){ //check if the user used the setconfig command
		if(strcmp(argv[2], "default") == 0){ //check if the user typed the default argument with the setconfig command
			DefaultConfig(); //load the default configuraton
		}else{ //the user provided a personal config file
			printf("Attempting to open config file: %s\n\r", argv[2]);
			LoadConfig(&UConfig, argv[2]); //load the users config file
			SaveConfig(&UConfig, config_path); //save the setting from the users config file to the config.jura file
		}
		printf("config updated succesfully\n\r"); //print this if everything went right
		return 0; //exit the program
	}
	LoadConfig(&UConfig, config_path); //load the settings from the config file
	if (argc == 2 && (strcmp(argv[1], "editconfig") == 0)){ //check if the user provided the editconfig command
		Open(config_path); //open the config.jura file
	}else if(argc == 2 && strcmp(argv[1], "makeconfig") == 0){ //check if the user provided the makeconfig command
		FILE *file = fopen(config_path, "r"); //check for the file
		if(file != NULL){ //if it extists
			printf("Config already exists\n\r");
		}else{ //if the config does not exists
			printf("Made config file\n\r");
		}
		return 0; //exit the program
	}else if(argc >= 2){ //check if the user provided a filename
		Open(argv[1]); //if so open that file
	}
	SetStatusMessage("Ctrl-S = save | Ctrl-Q = quit | Ctrl-F = find | Ctrl-G = go to line"); //set the base status message that shows the commands
	while (1){ //main loop for running the programming
	//update the windowsize
	struct winsize ws;
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1 && ws.ws_col != config.screencols && ws.ws_row != config.screenlines){//check if the windowsize has changed
		if(getWindowSize(&config.screenlines, &config.screencols) == -1) die("getWindowSize"); //update the windowsize
		config.screenlines -= 2;//make room for the status and message bar
	}
		RefreshScreen();//update the contents of the screen
		ProcessKeypress();//handle the keypresses of the users
	}
    return 0; //exit the program if the main loop has finished (Ctrl + q has been pressed)
}
