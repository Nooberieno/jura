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