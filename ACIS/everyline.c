// everyline.[ch] module:
//	Provided I/O support functions, most notably one to read every line
//	of a file and invoke a callback with that line.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdbool.h>

#include "everyline.h"


// int n = foreveryline( filename, eachlinef );
//	open the given filename, read every line from that a file,
//	remove the trailing newline (if present) and invoke the given
//	callback eachlinef with the filename, the line number and the line.
//	Return the number of lines read, -1 if the file can't be opened.
//
int foreveryline( char *filename, everylinecb eachlinef )
{
	FILE *f = fopen(filename, "r");
	if (f == NULL) {
		return -1;
	}
	char buffer[200]; // Assume each line is <200 characters long
	int lineno = 0;
	while (fgets(buffer, sizeof(buffer), f) != NULL) {
		(*eachlinef)(filename, ++lineno, buffer);
	}
	fclose(f);
	return lineno;
}
