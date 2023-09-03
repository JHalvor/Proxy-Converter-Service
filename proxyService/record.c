/*
 * This is a file that implements creation, modifiation and deletion of Records
 * in the internal representation.
 *
 * *** DO NOT CHANGE THIS FILE ***
 */
#include "record.h"

#include <stdlib.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <ctype.h>

Record* newRecord()
{
    Record* r;
    r = malloc( sizeof(Record) );
    if( r == NULL )
    {
        fprintf( stderr, "%s:%d Failed to allocate heap memory for a Record\n", __FILE__, __LINE__ );
    }
    initRecord( r );
    return r;
}

void initRecord( Record* r)
{
    memset( r, 0, sizeof(Record) );
}

void clearRecord( Record* r)
{
    if( r->has_username && r->username )
        free( r->username );

    initRecord( r );
}

void setSource( Record* r, char src )
{
    r->has_source = true;
    r->source = src;
}

void setDest( Record* r, char dst )
{
    r->has_dest = true;
    r->dest = dst;
}

void setUsername( Record* r, char* username )
{
    if( !username ) return;
    if( r->username ) free( r->username );
    r->has_username = true;
    r->username = strdup( username );

    if( r->username == NULL )
    {
        r->has_username = false;
        fprintf( stderr, "%s:%d Failed to allocate heap memory for the username string %s\n", __FILE__, __LINE__, username );
    }
}

void setId( Record* r, uint32_t id )
{
    r->has_id = true;
    r->id = id;
}

void setGroup( Record* r, uint32_t group )
{
    r->has_group = true;
    r->group = group;
}

void setSemester( Record* r, uint8_t semester )
{
    r->has_semester = true;
    r->semester = semester;
}

void setGrade( Record* r, Grade grade )
{
    r->has_grade = true;
    r->grade = grade;
}

void setCourse( Record* r, int oneCourseCode )
{
    r->has_courses = true;
    r->courses |= oneCourseCode;
}

void deleteRecord( Record* r )
{
    if( r )
    {
        if( r->has_username ) free( r->username );
        free( r );
    }
}

