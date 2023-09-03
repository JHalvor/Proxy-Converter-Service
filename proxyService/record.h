#pragma once

#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>

/* The grades represent the highest grade that the person
 * has completed, not every grade that the person completed.
 */
enum Grade
{
    Grade_None,
    Grade_Bachelor,
    Grade_Master,
    Grade_PhD
};

typedef enum Grade Grade;

/* A person can have taken several of IFI's 1st year courses.
 * Whether a person has taken a course or not is indicated by
 * a single bit in the person's record.
 */
#define Course_IN1000   (1<<0)
#define Course_IN1010   (1<<1)
#define Course_IN1020   (1<<2)
#define Course_IN1030   (1<<3)
#define Course_IN1050   (1<<4)
#define Course_IN1060   (1<<5)
#define Course_IN1080   (1<<6)
#define Course_IN1140   (1<<7)
#define Course_IN1150   (1<<8)
#define Course_IN1900   (1<<9)
#define Course_IN1910   (1<<10)

/* This struct is the internal representation of the record
 * describing a person.
 * If any of the variables has_* are false, the content of the
 * subsequent variable is undefined and it has not meaning.
 * 
 * If has_username is true, then username must point to a
 * 0-terminated string that is allocated separately for this
 * Record.
 * 
 */
struct Record
{
    bool      has_source;
    char      source;     /* The sender of this record */

    bool      has_dest;
    char      dest;     /* The destination of this record */

    bool      has_username;
    char*     username; /* The username of the person in this record */

    bool      has_id;
    uint32_t  id; /* The user ID of the person in this record */

    bool      has_group;
    uint32_t  group; /* The group ID of the person in this record */

    bool      has_semester;
    uint8_t   semester; /* The current semester of this person */

    bool      has_grade;
    Grade     grade; /* The highest grade completed so far by this person */

    bool      has_courses;
    uint16_t  courses; /* The 1st year courses taken by this person */
};

typedef struct Record Record;

/* These defines are used for the binary representation of
 * a Record.
 * Each flag indicates if a value exists or not in the binary
 * representation or not.
 * The flags take a single byte, leading to this format:
 *     flag(1 byte)
 *     [src(1 byte)]
 *     [dst(1 byte)]
 *     [strlen(4 bytes) username(strlen bytes)]
 *     [id(4 bytes)]
 *     [group(4 bytes)]
 *     [semester(1 byte)] 
 *     [grade(1 byte)]
 *     [courses](2 bytes)
 * The [ ] brackets indicate that these bytes are optional. They
 * exist in the binary record if and only if their respective flag
 * is set to 1.
 * All numbers are stored in network byte order.
 * Note that the username is not 0-terminated in the binary record.
 */
#define FLAG_SRC      ( 1<<7 )
#define FLAG_DST      ( 1<<6 )
#define FLAG_USERNAME ( 1<<5 )
#define FLAG_ID       ( 1<<4 )
#define FLAG_GROUP    ( 1<<3 )
#define FLAG_SEMESTER ( 1<<2 )
#define FLAG_GRADE    ( 1<<1 )
#define FLAG_COURSES  ( 1<<0 )

/* Allocate space for a Record and initialize it to zero/false */
Record* newRecord();

/* Set all values in the Record to zero/false. Ignores the content
 * of username, ie. the function assumes that the Record is invalid
 * when this function is called.
 */
void    initRecord(   Record* r );

/* Set all values in the Record to zero/false. Assumes that username
 * is valid and must be freed if non-null.
 */
void    clearRecord(  Record* r );

/* In the given record r, set has_source to true and the variable
 * source to src.
 */
void    setSource(    Record* r, char src );
void    setDest(      Record* r, char dst ); /* Like setSource, but for dest */

/* In the given record r, set has_username to true. Allocate
 * enough memory to store name and copy name into it. Store the
 * address of the newly allocated memory in the variable username.
 */
void    setUsername(  Record* r, char* rname );

void    setId(        Record* r, uint32_t id ); /* Like setSource, but for id */
void    setGroup(     Record* r, uint32_t group ); /* Like setSource, but for group */
void    setSemester(  Record* r, uint8_t semester ); /* Like setSource, but for semester */
void    setGrade(     Record* r, Grade grade ); /* Like setSource, but for grade */

/* In the given record r, set has_courses to true and add the bit
 * for the given course code to the variable courses. Other courses
 * are not overwritten.
 */
void    setCourse(    Record* r, int oneCourseCode );

/* Free the memory for the Record if the pointer is non-null.
 * The memory for username is also freed.
 */
void    deleteRecord( Record* r );

