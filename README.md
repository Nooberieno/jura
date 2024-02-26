# Jura
A small lightweight text editor like nano.

# Usage
Type ./jura and then the file you wish to edit.

Use the arrows to traverse the lines.

Use ctrl-S to save a file.

Use ctrl-Q to quit.

# Installation Guide
Clone the repository and open the folder in the terminal.

Install GNU Make and the gcc compiler.

Run the 'make' command in the terminal.

If this does not work you can run the command manually.

Type cc jura.c -o jura -Wall -Wextra -pedantic -std=c99 in to the terminal.

You should now be able to replicate the usage.

# Extras
If you want to have acces to jura from anywhere you need to adds it path to your .bashrc file.

Locate your .bashrc file(usually located in your home folder).

add this to your .bashrc file export PATH="$PATH:path/to/jura".

replace path/to/jura with the path to the jura binary you created in the installation guide.

you should now be able to run jura anywhere instead of ./jura in the folder of the binary.


