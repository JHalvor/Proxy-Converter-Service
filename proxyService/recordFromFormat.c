/*
 * This file implements two functions that read XML and binary information from a buffer,
 * respectively, and return pointers to Record or NULL.
 *
 * *** YOU MUST IMPLEMENT THESE FUNCTIONS ***
 *
 * The parameters and return values of the existing functions must not be changed.
 * You can add function, definition etc. as required.
 */
#include "recordFromFormat.h"
#include <string.h> 
#include <stdlib.h>
/* I don't know if i am allowed to add stdlib.h as an import myself.
   But it is nessecary to prevent warning on line 169 on the use of strtoul.
*/ 

Record* XMLtoRecord( char* buffer, int bufSize, int* bytesread )
{
    Record* rec = newRecord();

    bool recordStartFound = false;
    bool recordEndFound = false;
    bool foundNewParameter = false;

    // Reads through the buffer
    for (int i = 0; i < bufSize; i++) {
        // Checks for new parameter
        if (buffer[i] == '<') {
            foundNewParameter = true;
        }

        if (foundNewParameter) {
            // Reading the next parameter
            char tempBuffer[bufSize];
            memset(tempBuffer, 0, bufSize);
            for (int j = 0; j < bufSize; j++) {
                tempBuffer[j] = buffer[i];
                if (buffer[i] == '>') {
                    foundNewParameter = false;
                    break;
                }
                i++;
            }

            // Finding start and end of record
            if (strstr(tempBuffer, "record") != NULL) {
                if (recordStartFound) {
                    recordEndFound = true;
                    fprintf(stderr, "</record>\n");
                    break;
                }
                recordStartFound = true;
                fprintf(stderr, "<record>\n");
            }

            /*  Every if else statement finds parameter name by checking if the name exists in tempBuffer with strstr
                It then retrieves the value from the tempBuffer
                Handles the values and calls the appropiate functions
            */

            // Handling source value
            else if (strstr(tempBuffer, "source") != NULL) {
                char src;
                // Reading the source value between the quotation marks
                for (int j = 0; j < bufSize; j++) {
                    if (tempBuffer[j] == '"') {
                        src = tempBuffer[j + 1];
                        break;
                    }
                }
                setSource(rec, src);
                fprintf(stderr, "<source=\"%c\" />\n", rec->source);
            }

            // Handling dest value
            else if (strstr(tempBuffer, "dest") != NULL) {
                char dst;
                // Reading the dest value between the quotation marks
                for (int j = 0; j < bufSize; j++) {
                    if (tempBuffer[j] == '"') {
                        dst = tempBuffer[j + 1];
                        break;
                    }
                }
                setDest(rec, dst);
                fprintf(stderr, "<dest=\"%c\" />\n", rec->dest);
            }

            // Handling username value
            else if (strstr(tempBuffer, "username") != NULL) {
                int usernameLength = 0;
                bool readingUsername = false;
                // Getting the username length between the quotation marks
                for (int j = 0; j < bufSize; j++) {
                    if (tempBuffer[j] == '"') {
                        if (!readingUsername) {
                            readingUsername = true;
                        } else {
                            readingUsername = false;
                            usernameLength--;
                            break;
                        }
                    }
                    if (readingUsername) {
                        usernameLength++;
                    }
                }

                // Reading the username to a new buffer that is only withing the length of the username
                char usernameBuffer[usernameLength];
                int charIndex = 0;
                for (int j = 0; j < bufSize; j++) {
                    if (tempBuffer[j] == '"') {
                        if (!readingUsername) {
                            readingUsername = true;
                            j++;
                        } else {
                            readingUsername = false;
                            break;
                        }
                    }
                    if (readingUsername) {
                        usernameBuffer[charIndex] = tempBuffer[j];
                        charIndex++;
                    }
                }
                usernameBuffer[usernameLength] = '\0';
                setUsername(rec, usernameBuffer);
                fprintf(stderr, "<username=\"%s\" />\n", rec->username);
            }

            // Handling id value
            else if (strstr(tempBuffer, "id") != NULL) {
                int idLength = 0;
                bool readingID = false;
                // Getting the id length
                for (int j = 0; j < bufSize; j++) {
                    if (readingID) {
                        idLength++;
                    }
                    if (tempBuffer[j] == '"') {
                        if (!readingID) {
                            readingID = true;
                        } else {
                            readingID = false;
                            break;
                        }
                    }
                }

                // Reading the id to a new buffer that is only withing the length of the id
                char idBuffer[idLength];
                int charIndex = 0;
                for (int j = 0; j < bufSize; j++) {
                    if (readingID) {
                        idBuffer[charIndex] = tempBuffer[j];
                        charIndex++;
                    }
                    if (tempBuffer[j] == '"') {
                        if (!readingID) {
                            readingID = true;
                        } else {
                            readingID = false;
                            break;
                        }
                    }
                }

                // Converting the id found to expected setId() int type
                uint32_t id = strtoul(idBuffer, NULL, 10);

                setId(rec, id);
                fprintf(stderr, "<id=\"%d\" />\n", rec->id);
            }

            // Handling group value
            else if (strstr(tempBuffer, "group") != NULL) {
                int groupLength = 0;
                bool readingGroup = false;
                // Getting the group length
                for (int j = 0; j < bufSize; j++) {
                    if (readingGroup) {
                        groupLength++;
                    }
                    if (tempBuffer[j] == '"') {
                        if (!readingGroup) {
                            readingGroup = true;
                        } else {
                            readingGroup = false;
                            break;
                        }
                    }
                }

                // Reading the group to a new buffer that is only withing the length of the group
                char groupBuffer[groupLength];
                int charIndex = 0;
                for (int j = 0; j < bufSize; j++) {
                    if (readingGroup) {
                        groupBuffer[charIndex] = tempBuffer[j];
                        charIndex++;
                    }
                    if (tempBuffer[j] == '"') {
                        if (!readingGroup) {
                            readingGroup = true;
                        } else {
                            readingGroup = false;
                            break;
                        }
                    }
                }

                // Converting the group found to expected setGroup() int type
                uint32_t group = strtoul(groupBuffer, NULL, 10);

                setGroup(rec, group);
                fprintf(stderr, "<group=\"%d\" />\n", rec->group);
            }

            // Handling semester value
            else if (strstr(tempBuffer, "semester") != NULL) {
                int semesterLength = 0;
                bool readingSemester = false;
                // Getting the semester length
                for (int j = 0; j < bufSize; j++) {
                    if (readingSemester) {
                        semesterLength++;
                    }
                    if (tempBuffer[j] == '"') {
                        if (!readingSemester) {
                            readingSemester = true;
                        } else {
                            readingSemester = false;
                            break;
                        }
                    }
                }

                // Reading the semester to a new buffer that is only withing the length of the semester
                char semesterBuffer[semesterLength];
                int charIndex = 0;
                for (int j = 0; j < bufSize; j++) {
                    if (readingSemester) {
                        semesterBuffer[charIndex] = tempBuffer[j];
                        charIndex++;
                    }
                    if (tempBuffer[j] == '"') {
                        if (!readingSemester) {
                            readingSemester = true;
                        } else {
                            readingSemester = false;
                            break;
                        }
                    }
                }

                // Converting the semester found to expected setSemester() int type
                uint8_t semester = strtoul(semesterBuffer, NULL, 10);

                setSemester(rec, semester);
                fprintf(stderr, "<semester=\"%d\" />\n", rec->semester);
            }

            // Handling grade value
            else if (strstr(tempBuffer, "grade") != NULL) {
                int gradeLength = 0;
                bool readingGrade = false;
                // Getting the grade length
                for (int j = 0; j < bufSize; j++) {
                    if (tempBuffer[j] == '"') {
                        if (!readingGrade) {
                            readingGrade = true;
                        } else {
                            readingGrade = false;
                            gradeLength--;
                            break;
                        }
                    }
                    if (readingGrade) {
                        gradeLength++;
                    }
                }

                // Reading the grade to a new buffer that is only withing the length of the grade
                char gradeBuffer[gradeLength];
                int charIndex = 0;
                for (int j = 0; j < bufSize; j++) {
                    if (tempBuffer[j] == '"') {
                        if (!readingGrade) {
                            readingGrade = true;
                            j++;
                        } else {
                            readingGrade = false;
                            break;
                        }
                    }
                    if (readingGrade) {
                        gradeBuffer[charIndex] = tempBuffer[j];
                        charIndex++;
                    }
                }

                gradeBuffer[gradeLength] = '\0';

                // Checking which string the grade is with strcmp and assigns a Grade with appropiate enum
                Grade new_grade;

                if (strcmp(gradeBuffer, "Bachelor") == 0) {
                    new_grade = Grade_Bachelor;
                }
                else if (strcmp(gradeBuffer, "Master") == 0) {
                    new_grade = Grade_Master;
                }
                else if (strcmp(gradeBuffer, "PhD") == 0) {
                    new_grade = Grade_PhD;
                }
                else if (strcmp(gradeBuffer, "None") == 0) {
                    new_grade = Grade_None;
                }
                else {
                    continue;
                }

                setGrade(rec, new_grade);
                fprintf(stderr, "<grade=\"%s\" />\n", gradeBuffer);
            }

            // Handling courses values
            else if (strstr(tempBuffer, "courses") != NULL) {
                // If tempBuffer contains courses we start looking for course
                char coursesBuffer[bufSize];
                memset(coursesBuffer, 0, bufSize);
                fprintf(stderr, "<courses>\n");

                do {
                    // Goes to start of next XML parameter
                    while (buffer[i] != '<') {
                        i++;
                    }
                
                    // Reading the course inside of courses
                    memset(coursesBuffer, 0, bufSize);
                    for (int j = 0; j < bufSize; j++) {
                        coursesBuffer[j] = buffer[i];
                        if (buffer[i] == '>') {
                            break;
                        }
                        i++;
                    }

                    // Checks if course is found and we're not at the end of courses
                    if (strstr(coursesBuffer, "course") != NULL &&
                        strstr(coursesBuffer, "courses") == NULL) {                    
                        bool readingCourse = false;

                        // Reading the course to a new buffer that is only within the length of the course
                        char courseBuffer[5];
                        int charIndex = 0;
                        for (int j = 0; j < bufSize; j++) {
                            if (readingCourse && coursesBuffer[j] != '"') {
                                courseBuffer[charIndex] = coursesBuffer[j];
                                charIndex++;
                            }
                            if (coursesBuffer[j] == 'N') {
                                readingCourse = true;
                            }
                            if (coursesBuffer[j] == '"') {
                                readingCourse = false;
                            }
                        }
                        courseBuffer[4] = '\0';

                        // Checking which string the course is with strcmp and assigns the course with appropiate flag
                        int course;

                        if (strcmp(courseBuffer, "1000") == 0) {
                            course = Course_IN1000;
                        } 

                        else if (strcmp(courseBuffer, "1010") == 0) {
                            course = Course_IN1010;
                        }

                        else if (strcmp(courseBuffer, "1020") == 0) {
                            course = Course_IN1020;
                        } 
                        
                        else if (strcmp(courseBuffer, "1030") == 0) {
                            course = Course_IN1030;
                        }

                        else if (strcmp(courseBuffer, "1050") == 0) {
                            course = Course_IN1050;
                        }

                        else if (strcmp(courseBuffer, "1060") == 0) {
                            course = Course_IN1060;
                        }

                        else if (strcmp(courseBuffer, "1080") == 0) {
                            course = Course_IN1080;
                        }

                        else if (strcmp(courseBuffer, "1140") == 0) {
                            course = Course_IN1140;
                        }

                        else if (strcmp(courseBuffer, "1150") == 0) {
                            course = Course_IN1150;
                        }

                        else if (strcmp(courseBuffer, "1900") == 0) {
                            course = Course_IN1900;
                        }

                        else if (strcmp(courseBuffer, "1910") == 0) {
                            course = Course_IN1910;
                        }

                        else {
                            continue;
                        }

                        setCourse(rec, course);
                        fprintf(stderr, "<course=\"%s\" />\n", courseBuffer);
                    }
                }
                // While the courses is not completely read run the do code^^
                while (strstr(coursesBuffer, "courses") == NULL);

                fprintf(stderr, "</courses>\n");
            }
        }

        if (recordEndFound) {
            fprintf(stderr, "</record>\n");
            break;
        }
    }

    /*
    *   If record contains both source and dest
    *   Including at least one of the other record parameters
    *   Return the record
    */
    if ((rec->has_source &&
        rec->has_dest) &&
        (rec->has_username ||
        rec->has_id ||
        rec->has_group ||
        rec->has_semester ||
        rec->has_grade ||
        rec->courses)) {
            return rec;
    }

    // Else, clear record memory and return NULL
    else {
        if (rec) {
            if (rec->has_username) {
                clearRecord(rec);
            }
            deleteRecord(rec);
        }
        return NULL;
    }
}

int is_set(unsigned char flags, int pos) {
    return flags & (1 << pos);
}

Record* BinaryToRecord( char* buffer, int bufSize, int* bytesread )
{
    Record* rec = newRecord();
    int readCounter = 0;        // For tracking which byte in the buffer we're reading

    fprintf(stderr, "In BinaryToRecord, with rc being %d\n", *bytesread);
    fprintf(stderr, "And bufsize being %d\n", bufSize);
    fprintf(stderr, "And buffer being \n");

    for (int i = 0; i < bufSize; i++) {
        for (int j = 7; j >= 0; j--) {
            fprintf(stderr, "%d", (buffer[i] >> j) & 1);
        }
        fprintf(stderr, " ");
    }
    fprintf(stderr, "\n");

    // Checks flags
    if (*bytesread > 0) {
        unsigned char flags;
        flags = (unsigned char)buffer[readCounter];
   
        for (int i = 7; i >= 0; i--) {
            if (is_set(flags, i)) {
                switch (i) {
                    case 7:
                        rec->has_source = true;
                        fprintf(stderr, "Source is set\n");
                        break;
                    case 6:
                        rec->has_dest = true;
                        fprintf(stderr, "Dest is set\n");
                        break;
                    case 5:
                        rec->has_username = true;
                        fprintf(stderr, "Username is set\n");
                        break;
                    case 4:
                        rec->has_id = true;
                        fprintf(stderr, "ID is set\n");
                        break;
                    case 3:
                        rec->has_group = true;
                        fprintf(stderr, "Group is set\n");
                        break;
                    case 2:
                        rec->has_semester = true;
                        fprintf(stderr, "Semester is set\n");
                        break;
                    case 1:
                        rec->has_grade = true;
                        fprintf(stderr, "Grade is set\n");
                        break;
                    case 0:
                        rec->has_courses = true;
                        fprintf(stderr, "Courses is set\n");
                        break;
                }
            }
        }
        readCounter += 1;
    }

    /* Every following if statements for each record parameter will be handled accordingly if their has_ value is true
       Every read from the buffer will be handled with the expected amount of bytes to read
       After every byte read, readCounter increases by one
    */

    // Handling source value
    if (rec->has_source) {
        char source;
        source = buffer[readCounter];

        setSource(rec, source);
        fprintf(stderr, "Source is: %c\n", rec->source);
        readCounter += 1;
    }

    // Handling dest value
    if (rec->has_dest) {
        char dest;
        dest = buffer[readCounter];

        setDest(rec, dest);
        fprintf(stderr, "Dest is: %c\n", rec->dest);
        readCounter += 1;
    }

    // Handling username value
    if (rec->has_username) {
        int username_length = 0;

        // Extracting the length of the username from the buffer using bitwise or and left shift
        username_length |= (buffer[readCounter] & 0xFF) << 24;
        readCounter += 1;

        username_length |= (buffer[readCounter] & 0xFF) << 16;
        readCounter += 1;

        username_length |= (buffer[readCounter] & 0xFF) << 8;
        readCounter += 1;

        username_length |= (buffer[readCounter] & 0xFF);
        readCounter += 1;

        fprintf(stderr, "Username length is: %d\n", username_length);

        // Extracting the username in 'username_length' bytes from the buffer to a char array to store the username
        char username[username_length];
        for (int i = 0; i < username_length; i++) {
            username[i] = buffer[readCounter];
            readCounter++;
        }

        username[username_length] = 0;
        setUsername(rec, username);
    }

    // Handling id value
    if (rec->has_id) {
        uint32_t id = 0;

        // Extracting the id from the buffer using bitwise or and left shift
        id |= (buffer[readCounter] & 0xFF) << 24;
        readCounter += 1;

        id |= (buffer[readCounter] & 0xFF) << 16;
        readCounter += 1;

        id |= (buffer[readCounter] & 0xFF) << 8;
        readCounter += 1;

        id |= (buffer[readCounter] & 0xFF);
        readCounter += 1;

        setId(rec, id);

        fprintf(stderr, "ID is: %d\n", rec->id);
    }

    // Handling group value
    if (rec->has_group) {
        uint32_t group = 0;

        // Extracting the group from the buffer using bitwise or and left shift
        group |= (buffer[readCounter] & 0xFF) << 24;
        readCounter += 1;

        group |= (buffer[readCounter] & 0xFF) << 16;
        readCounter += 1;

        group |= (buffer[readCounter] & 0xFF) << 8;
        readCounter += 1;

        group |= (buffer[readCounter] & 0xFF);
        readCounter += 1;

        setGroup(rec, group);

        fprintf(stderr, "Group is: %d\n", rec->group);
    }

    // Handling semester value
    if (rec->has_semester) {
        uint8_t semester = 0;

        semester = buffer[readCounter];
        readCounter++;

        setSemester(rec, semester);

        fprintf(stderr, "Semester is: %d\n", rec->semester);
    }

    // Handling grade value
    if (rec->has_grade) {
        uint8_t grade = 0;

        // Extracting the grade value from the buffer
        grade = buffer[readCounter];

        readCounter++;

        Grade new_grade;

        if (grade & (1 << 0)) {
            new_grade = Grade_Bachelor;
            fprintf(stderr, "Grade: Bachelor\n");
        }
        else if (grade & (1 << 1)) {
            new_grade = Grade_Master;
            fprintf(stderr, "Grade: Master\n");
        }
        else if (grade & (1 << 2)) {
            new_grade = Grade_PhD;
            fprintf(stderr, "Grade: PhD\n");
        } 
        else {
            new_grade = Grade_None;
            fprintf(stderr, "Grade: None\n");
        }

        setGrade(rec, new_grade);
    }

    // Handling courses values
    if (rec->has_courses) {
        short courses = 0;

        // Extracting the course bytes from the buffer using bitwise or and left shift
        courses |= (buffer[readCounter] & 0xFF) << 8;
        readCounter += 1;

        courses |= (buffer[readCounter] & 0xFF);
        readCounter += 1;

        // Iterating through all 10 possible courses, for each course found they are added to the rec with setCourse()
        for (int i = 10; i >= 0; i--) {
            if (courses & (1 << i)) {
                switch (i) {
                    case 10:
                        setCourse(rec, Course_IN1910);
                        courses &= ~(1 << 10);
                        fprintf(stderr, "Course: IN1910\n");
                        break;
                    case 9:
                        setCourse(rec, Course_IN1900);
                        courses &= ~(1 << 9);
                        fprintf(stderr, "Course: IN1900\n");
                        break;
                    case 8:
                        setCourse(rec, Course_IN1150);
                        courses &= ~(1 << 8);
                        fprintf(stderr, "Course: IN1150\n");
                        break;
                    case 7:
                        setCourse(rec, Course_IN1140);
                        courses &= ~(1 << 7);
                        fprintf(stderr, "Course: IN1140\n");
                        break;
                    case 6:
                        setCourse(rec, Course_IN1080);
                        courses &= ~(1 << 6);
                        fprintf(stderr, "Course: IN1080\n");
                        break;
                    case 5:
                        setCourse(rec, Course_IN1060);
                        courses &= ~(1 << 5);
                        fprintf(stderr, "Course: IN1060\n");
                        break;
                    case 4:
                        setCourse(rec, Course_IN1050);
                        courses &= ~(1 << 4);
                        fprintf(stderr, "Course: IN1050\n");
                        break;
                    case 3:
                        setCourse(rec, Course_IN1030);
                        courses &= ~(1 << 3);
                        fprintf(stderr, "Course: IN1030\n");
                        break;
                    case 2:
                        setCourse(rec, Course_IN1020);
                        courses &= ~(1 << 2);
                        fprintf(stderr, "Course: IN1020\n");
                        break;
                    case 1:
                        setCourse(rec, Course_IN1010);
                        courses &= ~(1 << 1);
                        fprintf(stderr, "Course: IN1010\n");
                        break;
                    case 0:
                        setCourse(rec, Course_IN1000);
                        courses &= ~(1 << 0);
                        fprintf(stderr, "Course: IN1000\n");
                        break;
                }
            }
        }
    }

    /*
    *   If record contains both source and dest
    *   Including at least one of the other record parameters
    *   Return the record
    */
    if ((rec->has_source &&
        rec->has_dest) &&
        (rec->has_username ||
        rec->has_id ||
        rec->has_group ||
        rec->has_semester ||
        rec->has_grade ||
        rec->has_courses)) {
            return rec;
    }

    // Else, clear record memory and return NULL
    else {
        if (rec) {
            if (rec->has_username) {
                clearRecord(rec);
            }
            deleteRecord(rec);
        }
        fprintf(stderr, "Record not complete");
        return NULL;
    }
}