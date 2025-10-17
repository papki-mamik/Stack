#ifndef STACK_H
#define STACK_H

#include <stdio.h>
#include <stdlib.h>

enum ERRORS_STACK
{
    ERROR_NO = 0,
    ERROR_POINTER_TO_STACK = (1 << 0),
    ERROR_POINTER_DATA = (1 << 1),
    ERROR_WRONG_SIZE = (1 << 2),
    ERROR_WRONG_CAPACITY = (1 << 3),
    ERROR_CANARY_LEFT = (1 << 4),
    ERROR_CANARY_RIGHT = (1 << 5)
};

typedef int data_t;
typedef int StackErr_t;

typedef struct LIFO
{
    data_t* data;
    long int size;
    long int capacity;
    StackErr_t code_error;
} stack_t;

#define CANARY_BEGINNING -1880000015
#define CANARY_END        1880000015
#define POIZON   ((data_t)0xDEADBEEF)

#define STACK_OK(stk) \
    do { \
            StackVerify(stk); \
            if (stk -> code_error != 0) \
            { \
                if ((stk -> code_error & ERROR_POINTER_TO_STACK) || (stk -> code_error & ERROR_POINTER_DATA)) \
                { \
                    fprintf(stderr, "CRITICAL ERROR: Program terminated. Code: %d\n", stk -> code_error); \
                    Printer_ERRORS(stk); \
                    exit(stk -> code_error); \
                } \
                else \
                { \
                    StackDump(stk, __FILE__, __LINE__, __FUNCTION__); \
                    return stk -> code_error; \
                } \
            } \
        } while(0) 

StackErr_t StackInit(stack_t* stk, long int specified_capacity);

StackErr_t StackDestroy(stack_t* stk);

StackErr_t StackPush(stack_t* stk, data_t variable);

data_t StackPop(stack_t* stk, data_t* variable);

StackErr_t StackVerify(stack_t* stk);

void StackDump(stack_t* stk, const char* file, int line, const char* function);

void Printer_ERRORS(stack_t* stk);

#endif