# fsearch - Multithreaded File Search

![GitHub](https://img.shields.io/github/license/DavidLekei/fsearch)
![GitHub](https://img.shields.io/badge/Valgrind-0%20Leaks-brightgreen)
![GitHub last commit](https://img.shields.io/github/last-commit/DavidLekei/fsearch)

## **DISCLAIMER: This is a project made for LEARNING purposes only.**

# Usage:

> fsearch \<filename> [OPTIONS]

Available Options:

	-s : Specify the start path to being the search. If not specified, will start at /

	-d : Enable Debug info printing.

# Dependencies

	> pthread

# Build

	Binary file is always output to fsearch/bin
	To change this, modify the BIN_DIR variable in Makefile

	> make

	Compiles normal binary file

	> make debug

	Compiles binary file with debug flags on, 0 optimization, and symbols

	> make mpi

	Currently not implemented, but may be in the future to use multiple processes to search large file systems

# What I Learned

	This project was just meant to be a learning experience and as a referesher for writing multithreaded programs in C.
	It ended up being slightly trickier than anticipated, but I'm glad it was, as it was a great learning experience in writing parallel programs.

	This project allowed me to further strengthen my understanding of pointers in C.

	By wrapping printf in the dprint() function, I learned how to pass variable length arguments to a function, which can be very useful in the future.
	While very simple, and not perfect, I learned that the '\r' character will allow me to print to the same line over and over again, instead of printing multiple lines when only updating a value. (Used for dispalying number of files checked)
	Another useful thing I learned was how to do colored output through printf! The color codes specified in fsearch.h will be very useful in other projects.
	I enjoyed setting up a timer and specifying a callback function for when the timer interval goes off.