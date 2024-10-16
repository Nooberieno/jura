#ifndef SYNTAX_H
#define SYNTAX_H

#include "config.h"

#define HighlightDigits (1<<0)
#define HighlightStrings (1<<1)
#define NoHighlight (1<<1)

char *CExtensions[];
char *CKeywords[];
char *CppExtensions[];
char *CppKeywords[];
char *PythonExtensions[];
char *PythonKeywords[];
char *LuaExtensions[];
char *LuaKeywords[];
char *RubyExtensions[];
char *RubyKeywords[];
char *JSExtensions[];
char *JSKeywords[];
char *TSExtensions[];
char *TSKeywords[];
char *CSExtensions[];
char *CSKeywords[];
char *JavaExtensions[];
char *JavaKeywords[];
char *GolangExtensions[];
char *GoKeywords[];
char *SwiftExtensions[];
char *SwiftKeywords[];
char *HTMLExtensions[];
char *HTMLKeywords[];
char *JSONExtensions[];
char *JSONSyntax[];
char *CssExtensions[];
char *CssSyntax[];
char *TeXExtensions[];
char *TeXSyntax[];
char *LaTeXExtensions[];
char *LaTeXSyntax[];
char *PHPExtensions[];
char *PHPKeywords[];
char *RubyExtensions[];
char *RubyKeywords[];
char *KotlinExtensions[];
char *KotlinKeywords[];
char *TextExtensions[];
char *ShellExtensions[];
char *BinaryExtensions[];

Syntax SyntaxDatabase[];

unsigned int SyntaxDatabaseEntries;
#endif