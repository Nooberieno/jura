# jura
A small lightweight text editor like nano.

# Usage
Type ./jura "filename" note that this only works in the folder the binary is located in. to change this behaviour download jura from the AUR or follow the Extra Setup guide.

If you downloaded jura from the AUR or follow the Extras guide you should be able to type jura "filename"

Use the arrows to traverse the lines.

Use ctrl-S to save a file.

Use ctrl-Q to quit.

Use ctrl-F to search. 

# Installation Guide Non-Arch Linux
Clone the repository and open the folder in the terminal.

Install GNU Make and the gcc compiler.

Run the 'make' command in the terminal.

If this does not work you can run the command manually.

Type cc jura.c -o jura -Wall -Wextra -pedantic -std=c99 in to the terminal.

You should now be able to replicate the usage.

# Installation Guide Arch Linux
jura is availlable on the AUR as jura

You can also follow the regular installation

# Extra Setup
If you want to have acces to jura from anywhere you need to add it path to your .bashrc file.

Locate your .bashrc file(usually located in your home folder).

add this to your .bashrc file export PATH="$PATH:path/to/jura".

replace path/to/jura with the path to the jura binary you created in the installation guide.

you should now be able to run jura anywhere instead of ./jura in the folder of the binary.

# Syntax Highlighting Support
!!!IMPORTANT!!! jura does not highlight or recognize specialized syntax by modules, plugins or extensions and does not detect specialized file extensions for example .jsx or .scss

jura supports syntax highlighting in following programming languages:

C (.c .h files)

C++ (.cpp .CPP .cc .cp .cxx .C ..hh .hpp files)

Python (.py files)

Lua (.lua files)

Rust (.rs files)

Javascript (.js files)

Typescript (.ts files)

C# (.cs files)

Java (.java files)

GO (.go .mod .sum files)

Swift (.swift .xcodeproj .storyboard .xib .xcdatamodeld files)

PHP (.php files)

Ruby (.rb files)

Kotlin (.kt .ktm .kts files)

jura also fully supports base TeX and base LaTeX

for jura to detect TeX files they need to have the .tex extension

for jura to detect LaTeX files they need to have the .ltx extension

jura also detects and partially highlights certain other filetypes, these include:

JSON (.json files)

HTML (.html files)

CSS (.css files)

# RoadMap
for the full changelog check the CHANGELOG.md file or the version compare on the releases page.

between versions there might be bug fixes. it is important to update to the latest version of jura for the most features and the least bugs. 

check the changelog to know if any major bugs have been fixed for a newer version

Version 1.0 focused on basic features of a terminal text editor such as Saving, Exiting and Searching.

Version 2.0 focused on syntax highlighting and filetype detection.

Version 3.0 is focusing on configuration and documentation.

Somewhere in version three jura will get a wiki with the primary focus on showing how jura works and how to configure jura

# Bugs
This section contains the current known major bugs and are being worked on.

Terminal does not get properly restored after an error has been thrown.

The editor does not update in size after the terminal or terminal emulator changes size.