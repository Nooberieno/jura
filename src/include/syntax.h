#ifndef SYNTAX_H
#define SYNTAX_H

#include "config.h"

#define HighlightDigits (1<<0)
#define HighlightStrings (1<<1)
#define NoHighlight (1<<1)

extern char *CExtensions[];
extern char *CKeywords[];
extern char *CppExtensions[];
extern char *CppKeywords[];
extern char *PythonExtensions[];
extern char *PythonKeywords[];
extern char *LuaExtensions[];
extern char *LuaKeywords[];
extern char *RubyExtensions[];
extern char *RubyKeywords[];
extern char *JSExtensions[];
extern char *JSKeywords[];
extern char *TSExtensions[];
extern char *TSKeywords[];
extern char *CSExtensions[];
extern char *CSKeywords[];
extern char *JavaExtensions[];
extern char *JavaKeywords[];
extern char *GolangExtensions[];
extern char *GoKeywords[];
extern char *SwiftExtensions[];
extern char *SwiftKeywords[];
extern char *HTMLExtensions[];
extern char *HTMLKeywords[];
extern char *JSONExtensions[];
extern char *JSONSyntax[];
extern char *CssExtensions[];
extern char *CssSyntax[];
extern char *TeXExtensions[];
extern char *TeXSyntax[];
extern char *LaTeXExtensions[];
extern char *LaTeXSyntax[];
extern char *PHPExtensions[];
extern char *PHPKeywords[];
extern char *RubyExtensions[];
extern char *RubyKeywords[];
extern char *KotlinExtensions[];
extern char *KotlinKeywords[];
extern char *TextExtensions[];
extern char *ShellExtensions[];
extern char *BinaryExtensions[];

extern Syntax SyntaxDatabase[];

extern unsigned int SyntaxDatabaseEntries;
#endif