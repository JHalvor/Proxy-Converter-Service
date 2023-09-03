#pragma once

#include <inttypes.h>
#include <stdbool.h>

struct BinaryFile
{
    uint8_t* buffer;
    uint32_t size;
    uint32_t offset;
    bool     expect_sleep;
};

typedef struct BinaryFile BinaryFile;

/* Read the given binary file from disk and store the
 * content entirely in an internal buffer.
 * Returns NULL if the file is not found.
 */
BinaryFile* bin_read_open( char* filename );

/* Read either a sleeptime or a binary record from the
 * BinaryFile structure.
 * The read operation alternates between reading a 4-byte
 * sleeptime and a binary record. It starts reading a sleeptime
 * value.
 *
 * The sleeptime or record is copied into buffer if bufsz
 * is sufficient.
 * It returns 0 when it reaches the end of the buffer.
 */
int bin_read( BinaryFile* bf, char* buffer, uint32_t bufsz );

/* Releases the resource associated with this binary
 * file.
 */
void bin_close( BinaryFile* bf );

