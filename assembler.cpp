#include "functions.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>

typedef struct data
{
    char* buffer;
    long int  buffer_size;
    int   current_index;
    int func_index;
} data_t;

typedef struct byte_code
{
    int*  array_num;
    int   code_size;
    int  labels[10];
} code_t;

char* MakeBuffer (FILE* pointer_to_file, data_t* data);
 
int ReadFunction (data_t* data);

int FillByteCode (data_t* data, code_t* byte_code);

int ScanfNumReg (data_t* data, int* num_reg);

void  SkipBufToNext (data_t* data);

void  PrintByteCode (code_t* code);

int main()
{
    data_t data1;
    data1.buffer = NULL;
    data1.buffer_size   = 0;
    data1.current_index = 0;
    data1.func_index = 1;

    FILE* input = fopen("quadratka.asm", "r");
    data1.buffer = MakeBuffer(input, &data1);
    fclose(input);

    code_t byte_code1;
    byte_code1.code_size = 1;
    byte_code1.array_num = (int*)calloc(size_t((data1.buffer_size / 2) + 1), sizeof(int));
    for(int i = 0; i < 9; i++)
    {
        byte_code1.labels[i] = -1;
    }

    byte_code1.code_size = FillByteCode(&data1, &byte_code1);
    if(byte_code1.code_size == 0) 
    {
        free(data1.buffer);
        free(byte_code1.array_num);
        
        return 1;
    }

    data1.current_index = 0;
    data1.func_index = 1;
    byte_code1.code_size = 1;

    byte_code1.code_size = FillByteCode(&data1, &byte_code1);
    if(byte_code1.code_size == 0) 
    {
        free(data1.buffer);
        free(byte_code1.array_num);
        
        return 1;
    }

    PrintByteCode(&byte_code1);

    for(int i = 1; i < byte_code1.code_size; i++)
    {
        if((byte_code1.array_num[i] == JB || byte_code1.array_num[i] == CALL) && byte_code1.array_num[i+1] == -1) 
        {
            free(data1.buffer);
            free(byte_code1.array_num);

            return 1;
        }
    }

    printf("Program successfully compiled\n");

    FILE* output = fopen("byte_code.txt", "w");

    for(int i = 0; i < byte_code1.code_size; i++)
    {
        fprintf(output, "%d ", byte_code1.array_num[i]);
    }

    free(data1.buffer);
    free(byte_code1.array_num);
    
    return 0;
}

char* MakeBuffer(FILE* pointer_to_file, data_t* data)
{
    struct stat info;
    
    if(fstat(fileno(pointer_to_file), &info) != 0)
    {
        return NULL;
    }
    
    data->buffer_size = info.st_size;
    
    data->buffer = (char*)calloc(size_t(data->buffer_size + 1), sizeof(char));
    
    assert(data->buffer != NULL);
    
    fread(data->buffer, sizeof(char), size_t(data->buffer_size), pointer_to_file);
    data->buffer[data->buffer_size] = '\0';

    for(int i = 0; i < data->buffer_size; i++)
    {
        if(data->buffer[i] == ';')
            while(data->buffer[i] != '\n')
            {
                data->buffer[i] = '\n';
                i++;
            }
    }

    // puts(data->buffer);

    for(int i = 0; i < data->buffer_size; i++)
    {
        if((data->buffer[i] == '\n') || (data->buffer[i] == ' ') || (data->buffer[i] == '\r')) data->buffer[i] = '\0';
    }
    
    return data->buffer;
}

int FillByteCode(data_t* data, code_t* byte_code)
{    
    int line = 1;

    while(1)
    {   
        int number_function = ReadFunction(data);
        
        if(number_function == PUSH) 
        {
            int variable = 0;
            
            byte_code->array_num[byte_code->code_size++] = PUSH;
            
            sscanf((data->buffer + data->current_index), "%d", &variable);
            
            byte_code->array_num[byte_code->code_size++] = variable;
            
            SkipBufToNext(data);
        }
        else if(number_function == PUSHR) 
        {
            int num_reg = 0;
            
            byte_code->array_num[byte_code->code_size++] = PUSHR;

            ScanfNumReg(data, &num_reg);
            
            byte_code->array_num[byte_code->code_size++] = num_reg;
            
            SkipBufToNext(data);
        }
        else if(number_function == POPR) 
        {
            int num_reg = 0;
            
            byte_code->array_num[byte_code->code_size++] = POPR;
            
            ScanfNumReg(data, &num_reg);
            
            byte_code->array_num[byte_code->code_size++] = num_reg;
            
            SkipBufToNext(data);
        }
        else if(number_function == LABLE) 
        {
            int num_lab = 0;

            sscanf((data->buffer + data->current_index + 1), "%d", &num_lab);

            byte_code->labels[num_lab] = data->func_index;

            SkipBufToNext(data);
            data->func_index--;
        }
        else if(number_function == JB || number_function == CALL) 
        {
            int lable = 0;
            
            byte_code->array_num[byte_code->code_size++] = number_function;
            
            if(data->buffer[data->current_index] == ':')
            {
                sscanf((data->buffer + data->current_index), ":%d", &lable);
                byte_code->array_num[byte_code->code_size++] = byte_code->labels[lable];
            }
            else
            {
                sscanf((data->buffer + data->current_index), "%d", &lable);
                byte_code->array_num[byte_code->code_size++] = lable;
            }

            SkipBufToNext(data);
        }
        else if(number_function == HLT)
        {
            byte_code->array_num[byte_code->code_size++] =  HLT;

            break;
        }
        else if(number_function <= IN && number_function >= PUSH)
        {
            byte_code->array_num[byte_code->code_size++] = number_function;
        }
        else
        {
            printf("Error: unvaluable function in line %d\n", line);
            
            return 0;
        }

        line++;
    }

    // for(int i = 0; i < 9 ;i++)
    // {
    //     printf("[%d]", byte_code->labels[i]);
    // }
    // printf("\n");


    *byte_code->array_num = byte_code->code_size;

    return byte_code->code_size;
}
        
int ReadFunction(data_t* data)
{
    int number_function = 0;
    const int max_function_lenght = 40;
    char extracted_function[max_function_lenght];
    
    sscanf((data->buffer + (data->current_index)), "%39s", extracted_function);

    if     (strcasecmp(extracted_function, "PUSH")  == 0) number_function = PUSH;
    else if(strcasecmp(extracted_function, "ADD")   == 0) number_function = ADD;
    else if(strcasecmp(extracted_function, "SUB")   == 0) number_function = SUB;
    else if(strcasecmp(extracted_function, "MUL")   == 0) number_function = MUL;
    else if(strcasecmp(extracted_function, "SQvRT") == 0) number_function = SQvRT;
    else if(strcasecmp(extracted_function, "DIV")   == 0) number_function = DIV;
    else if(strcasecmp(extracted_function, "OUT")   == 0) number_function = OUT;
    else if(strcasecmp(extracted_function, "IN")    == 0) number_function = IN;
    else if(strcasecmp(extracted_function, "POPR")  == 0) number_function = POPR;
    else if(strcasecmp(extracted_function, "PUSHR") == 0) number_function = PUSHR;
    else if(strcasecmp(extracted_function, "JB")    == 0) number_function = JB;
    else if(strcasecmp(extracted_function, "CALL")    == 0) number_function = CALL;
    else if(strcasecmp(extracted_function, "RET")    == 0) number_function = RET;
    else if(data->buffer[data->current_index] == ':')
    {
        number_function = LABLE;
        return number_function;
    }
    else if(strcasecmp(extracted_function, "HLT")   == 0) 
    {
        number_function = HLT;
        return number_function;
    }

    SkipBufToNext(data);
    
    return number_function;
}

void PrintByteCode(code_t* byte_code)
{
    for(int i = 0; i < byte_code->code_size; i++)
    {
        printf("%d ", byte_code->array_num[i]);
    }        
    printf("\n");
}

int ScanfNumReg(data_t* data, int* num_reg)
{
    char extracted_reg[3];

    sscanf((data->buffer + (data->current_index)), "%2s", extracted_reg);

    //fprintf(stderr, "extracted_reg = %s\n", extracted_reg);

    if(strcasecmp(extracted_reg, "AX") == 0) *num_reg = RAX;
    else if(strcasecmp(extracted_reg, "BX") == 0) *num_reg = RBX;
    else if(strcasecmp(extracted_reg, "CX") == 0) *num_reg = RCX;
    else if(strcasecmp(extracted_reg, "DX") == 0) *num_reg = RDX;
    else if(strcasecmp(extracted_reg, "EX") == 0) *num_reg = REX;
    else if(strcasecmp(extracted_reg, "FX") == 0) *num_reg = RFX;
    else if(strcasecmp(extracted_reg, "GX") == 0) *num_reg = RGX;
    else if(strcasecmp(extracted_reg, "HX") == 0) *num_reg = RHX;

    return *num_reg;
}

void SkipBufToNext(data_t* data)
{
    while(data->buffer[data->current_index] != '\0') (data->current_index) += 1;
    while(data->buffer[data->current_index] == '\0') (data->current_index) += 1;
    
    data->func_index++;
}