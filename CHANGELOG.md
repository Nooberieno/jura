# 1.0
Basic terminal text editor with saving functionality

# 1.1
Very basic searching using Ctrl-F keybind

# 1.2
Expanded search to incremental search

This means the cursor updates when a new match is found

# 1.3
After canceling a search the cursor goes back to the position it was in before searching

# 1.4
Ability to go to next or previous search with the arrow keys

# 2.0
Syntax highlighting for digits

# 2.1
Highlights matches when searching with the Ctrl-F keybind

# 2.2
Changed digit highlighting to support decimals which use a '.' 

Changed digit highlighting to not highlight digits that are followed by a letter, for example int32 is not highlighted but 32int is

# 2.3
File detection for files that end with the .c .h or .cpp file extension

Digit highlighting is now only enabled for these files

These files also have highlighting for both double and single quoted strings

# 2.4
jura now fully supports syntax highlighting in following programming languages:

C,
C++,
Python,
Lua,
Rust,
Javascript,
Typescript,
C#,
Java,
GO,
Swift,

jura now also support detects and partially highlights certain other filetypes, these include:

JSON,
HTML,
CSS,

Any files that do not have syntax highlighting are not detected and will display "no filetype detected"

# 2.5
jura now also fully supports base TeX and base LaTeX

for jura to detect TeX files they need to have the .tex extension

for jura to detect LaTeX files they need to have the .ltx extension

# 2.6
jura now supports PHP, Ruby and Kotlin

# 3.0
Added a config file in the users home folder that jura creates on first startup

# 3.1
Added an editconfig command. 

type jura or ./jura (depending on your installation) followed by "editconfig" to adjust your config file

# 3.2
Added an setconfig command. 

type jura or ./jura (depending on your installation) followed by "setconfig" "path/to/your/config"

# 3.3
Added a default argument to the setconfig command and a makeconfig command

type ./jura setconfig default to restore the default options

If for some reason the config.jura file gets deleted, you can restore it with the default configuration by openening jura or running the makeconfig command

type ./jura makeconfig to ensure the config exists, if not it will be created

**Bug fixes**

fixed a bug that did not allow for a custom config to be set if it was outside of the folder where the binary was made

fixed the bug that did not restore the terminal emulator properly

# 3.4
fixed a bug that caused the empty line character from the config to not actually be displayed

removed some old code that reused certain libraries and declared unused variable

added documentation to make the code more accesable

# 3.5
this release introduces the official jura wiki found at https://github.com/Nooberieno/jura/wiki  
and added more support for the community on the github repository

**Bug fixes**
fixed the bug that did not resize the editor in jura when the terminal changes, but it does require a keypress to update
fixed a visual bug that made the terminal layout look weird when using print statements in the code

# 4.0
this release adds the feature to go to a specific line with the Ctrl + G keybind