#pragma once

#include <stdio.h>

/* Open the given file.
 * Returns NULL if the file is not found.
 */
FILE* xml_read_open( char* filename );

/* Reads either a <sleep> record or a <record> record from the
 * file. The record is stored in the buffer if the bufsz is
 * sufficient.
 * The function returns 0 when the end of file is reached.
 */
int   xml_read( FILE* file, char* buffer, int bufsz );

/* Releases the resource associated with this XML
 * file.
 */
void  xml_close( FILE* file );

