#ifndef ASSEMBLER_SECOND_GENERATION_H
#define ASSEMBLER_SECOND_GENERATION_H

#include "functions.h"
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>

typedef int AsmErr_t;

struct lines
{
    size_t length;
    char* line_ptr;
};

struct source_buffer 
{
    char* data;
    long int size;
};

struct byte_code 
{
    int* code_array;
    int code_size; 
    int labels[10];
};

struct command
{
    int opcode;
    const char* name;
    int operand_type;
};

struct regs
{
    int num_reg;
    const char* r_name;
};

typedef struct assembler 
{
    const struct command* commands_list;
    long int num_commands;
    const struct regs* regs_list;
    long int num_regs;
    struct lines* line_array;
    long int count;
    struct source_buffer src_buf;
    struct byte_code output;
} asm_t;

struct fix_up
{
    int line;
    int label_ptr;
};

struct data_fix_up
{
    struct fix_up* ptr;
    int count_fix_ups;
    size_t capacity_fix_ups;
};

void AsmInit(asm_t* asmc);

char* MakeBuffer(FILE* pointer_to_file, struct source_buffer* src_buf);

struct lines* MakeLineArray(struct source_buffer* buf, long int* count);

void FillByteCode(asm_t* asmc, struct data_fix_up* fix_up);

void SscanfNumReg(char* stk, int* reg, const struct regs* regs_list, long int num_regs);

void PrintByteCode(struct byte_code, FILE* output);

void CheckLabels(asm_t* asmc, struct data_fix_up fix_up);

#endif