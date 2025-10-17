#ifndef SPU_H
#define SPU_H

#include "stack.h"
#include "functions.h"
#include <string.h>
#include <math.h>

typedef int SPUErr_t;

typedef struct SPU
{
    stack_t* stack;
    int* code;
    int IP;
    data_t regs[8];
} SPU_t;

int Reader(FILE* byte_code, SPU_t* SPU);

void Runner(stack_t* stk);

void PriterArrays(data_t* pointer, int size);

void SPUDestroy(SPU_t* SPU);

SPUErr_t SPUDump(SPU_t* SPU);

#endif 