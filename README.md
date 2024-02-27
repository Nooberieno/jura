# jura
A small lightweight text editor like nano.

# Usage
Type ./jura "filename" note that this only works in the folder the binary is located in to change this, download jura from the AUR or follow the Extras guide.

If you downloaded jura from the AUR or follow the Extras guide you should be able to type jura "filename"

Use the arrows to traverse the lines.

Use ctrl-S to save a file.

Use ctrl-Q to quit.

Use ctrl-F to search. 

When searching press enter to go to the first match found

# Installation Guide Non-Arch Linux
Clone the repository and open the folder in the terminal.

Install GNU Make and the gcc compiler.

Run the 'make' command in the terminal.

If this does not work you can run the command manually.

Type cc jura.c -o jura -Wall -Wextra -pedantic -std=c99 in to the terminal.

You should now be able to replicate the usage.

# Installation Guide Arch Linux
jura is availlable on the aur as jura

You can also follow the regular installation

# Extras
If you want to have acces to jura from anywhere you need to adds it path to your .bashrc file.

Locate your .bashrc file(usually located in your home folder).

add this to your .bashrc file export PATH="$PATH:path/to/jura".

replace path/to/jura with the path to the jura binary you created in the installation guide.

you should now be able to run jura anywhere instead of ./jura in the folder of the binary.


