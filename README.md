# Simple Shell
This program was an assignment for my Operating Systems class. The assignment PDF can be found [here](lab2.pdf), with my answers to the writeup questions available [here](writeup.txt). The short version is to implement a shell supporting the following features:

- Background-ing of processes with `&`
- Input/Output redirection with `>`, `>>`, and `<`
- Pipelines with `|`

All operators are assumed to be separated with spaces.

## Operation
To run it, build it (see Building), and then type `./simsh`. Then type commands as usual. Send an `EOF` (usually `Ctrl-D`) or type `exit` on its own to exit the shell.

## Building
### Prerequisites
- Relatively new `gcc` (supports `-std=c11`)
- Relatively new `cmake` (&geq; 3.6)
### Building
Type `cmake .`