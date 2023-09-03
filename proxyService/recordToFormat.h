#pragma once

#include "record.h"
#include <stdio.h>

/* This function takes a Record and creates a NULL-terminated
 * multi-line string that contains an XML representation for
 * this record.
 * The value strlen() for this string is written to the location
 * pointed to by bufSize.
 * It returns NULL if memory for the string could not be allocated.
 */
char* recordToXML( Record* r, int* bufSize );

/* This function uses recordToXML() to create an XML representation
 * of the Record and write it to the file f.
 */
void fprintRecordAsXML( FILE* f, Record* r );

/* This function uses recordToXML() to create an XML representation
 * of the Record and write it to stdout. Meant for debugging
 * purposes.
 */
void printRecordAsXML( Record* r );

/* This function takes a Record and creates a binary representation
 * for this record.
 * The number of bytes that contain the binary representation are written
 * to the location pointed to by bufSize.
 * It returns NULL if memory for the string could not be allocated.
 */
char* recordToBinary( Record* r, int* bufSize );

/* This function uses recordToBinary() to create a binary representation
 * of the Record and write it to the file f.
 */
void fprintRecordAsBinary( FILE* f, Record* r );

