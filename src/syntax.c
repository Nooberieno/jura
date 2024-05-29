#include <stdio.h>

#include "include/syntax.h"
#include "include/config.h"

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

extern Syntax SyntaxDatabase[] = {
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