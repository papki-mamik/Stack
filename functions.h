#ifndef FUNCTIONS_H
#define FUNCTIONS_H

enum functions
{
    PUSH,
    ADD,
    SUB,
    MUL,
    SQvRT,
    DIV,
    OUT,
    HLT,
    RET,
    IN,
    JUMP,
    JE,
    JNE,
    JA,
    JAE,
    JB,
    JBE,
    CALL,
    PUSHR,
    POPR
};

enum reg
{
    RAX = 1, 
    RBX = 2, 
    RCX = 3,
    RDX = 4,
    REX = 5,
    RFX = 6,
    RGX = 7, 
    RHX = 8
};

enum arguments
{
    ARG_NONE = 0,
    ARG_NUMBER,
    ARG_REGISTER,
    ARG_LABEL
};

#endif 