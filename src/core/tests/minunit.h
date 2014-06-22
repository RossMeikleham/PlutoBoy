/* http://c.learncodethehardway.org/book/ex30.html */

#undef NDEBUG
#ifndef _minunit_h
#define _minunit_h

#include <stdio.h>
#include "dbg.h"
#include <stdlib.h>

typedef void (*voidFunc)(void);

voidFunc BEFORE = NULL;
voidFunc AFTER = NULL;

#define mu_suite_start() const char *message = NULL

#define mu_assert(test, message)if (!(test)) { log_err(message); return message; } ;

// Before and After made by Ross Meikleham
#define SET_BEFORE(name) BEFORE = name
#define SET_AFTER(name) AFTER = name


#define mu_run_test(test)  if(BEFORE != NULL) BEFORE(); \
    debug("\n-----%s", " " #test); \
    message = test(); tests_run++; if (message) return message;\
    if(AFTER != NULL) AFTER(); 



#define RUN_TESTS(name) int main(int argc, char *argv[]) {\
    argc = 1; \
    debug("----- RUNNING: %s", argv[0]);\
        printf("----\nRUNNING: %s\n", argv[0]);\
        const char *result = name();\
        if (result != 0) {\
            printf("FAILED: %s\n", result);\
        }\
        else {\
            printf("ALL TESTS PASSED\n");\
        }\
    printf("Tests run: %d\n", tests_run);\
        exit(result != 0);\
}


int tests_run;

#endif
