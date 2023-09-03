#pragma once

#include "record.h"

/* This function reads bytes from a buffer, interprets them as an XML
 * representation of a Record, and creates a Record that contains this
 * information.
 *
 * The parameter buffer points to the bytes that can be read.
 *   The buffer can contain information for more than one Record
 *   and incomplete information.
 * The parameter bufSize is the number of bytes that can be read.
 * The parameter bytesRead points to an integer where the actually
 *   processed bytes are stored when the function returns non-NULL.
 *
 * The function returns NULL if the buffer does not contain information
 * to fill a Record completely.
 * If the function returns a Record, it is allocated on the heap and
 * filled with information read from the buffer. The integer pointed
 * to by bytesread contains the number of bytes from the buffer that
 * were used to fill this Record.
 */
Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread );

/* This function reads bytes from a buffer, interprets them as a binary
 * representation of a Record, and creates a Record that contains this
 * information.
 *
 * The parameter buffer points to the bytes that can be read.
 *   The buffer can contain information for more than one Record
 *   and incomplete information.
 * The parameter bufSize is the number of bytes that can be read.
 * The parameter bytesRead points to an integer where the actually
 *   processed bytes are stored when the function returns non-NULL.
 *
 * The function returns NULL if the buffer does not contain information
 * to fill a Record completely.
 * If the function returns a Record, it is allocated on the heap and
 * filled with information read from the buffer. The integer pointed
 * to by bytesread contains the number of bytes from the buffer that
 * were used to fill this Record.
 */
Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread );

