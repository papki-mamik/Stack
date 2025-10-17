#include "stack.h"

StackErr_t StackInit(stack_t* stk, long int specified_capacity)
{
    stk -> data = (data_t*)calloc((size_t)(specified_capacity + 2), sizeof(data_t));
    stk -> data[0] = CANARY_BEGINNING;
    stk -> data[specified_capacity + 1] = CANARY_END;
    stk -> size = 1;
    stk -> capacity = specified_capacity + 2;

    for(long int i = 1; i < specified_capacity + 1; i++)
    {
        stk -> data[i] = POIZON;
    }

    STACK_OK(stk);
    
    return 0;
}

StackErr_t StackPush(stack_t* stk, data_t variable)
{
    STACK_OK(stk);

    if(stk -> size + 1 > stk -> capacity)
    {
        stk -> capacity = stk -> capacity * 2 - 2;
        stk -> data = (data_t*)realloc(stk -> data, (size_t)stk -> capacity * sizeof(data_t));

        stk->data[stk->capacity - 1] = CANARY_END;
        for(long int i = stk->size; i < stk->capacity - 1; i++) 
        {
            stk->data[i] = POIZON;
        }
    }
    
    //StackDump(StackVerify(stk), stk, __FILE__, __LINE__, __FUNCTION__);
    
    stk -> data[stk -> size++] = variable;

    STACK_OK(stk);
    
    return 0;
}

StackErr_t StackPop(stack_t* stk, data_t* variable)
{
    STACK_OK(stk);

    if(stk -> size == 1)
    {
        stk -> code_error |= ERROR_WRONG_SIZE;
        return ERROR_WRONG_SIZE;
    }

    *variable = stk -> data[--(stk -> size)];

    stk->data[stk->size] = POIZON;  
    
    STACK_OK(stk);

    return 0;
}

StackErr_t StackDestroy(stack_t* stk)
{
    free(stk -> data);

    stk -> data = NULL;
    stk -> capacity = 0;
    stk -> size = 0;
    
    return 0;
}

StackErr_t StackVerify(stack_t* stk)
{
    stk -> code_error = ERROR_NO;

    if(stk == NULL)
    {
        stk -> code_error |= ERROR_POINTER_TO_STACK;
        return stk -> code_error;
    }
    if(stk -> data == NULL)
    {
        stk -> code_error |= ERROR_POINTER_DATA;
    }
    if(stk -> size < 0 || stk -> size > stk -> capacity)
    {
        stk -> code_error |= ERROR_WRONG_SIZE;
    }
    if(stk -> capacity <= 0)
    {
        stk -> code_error |= ERROR_WRONG_CAPACITY;
    }
    if(stk -> code_error == 0)
    {
        if(stk -> data[0] != CANARY_BEGINNING)
        {
        stk -> code_error |= ERROR_CANARY_LEFT;
        }
        if(stk -> data[stk -> capacity - 1] != CANARY_END)
        {
            stk -> code_error |= ERROR_CANARY_RIGHT;
        }
    }

    return stk -> code_error;
}

void StackDump(stack_t* stk, const char* file, int line, const char* function)
{
    Printer_ERRORS(stk);
    
    FILE* errors = fopen("errors.txt", "a");

    fprintf(errors, 
    "StackDump called from %s: in function %s\n%s:%d\n"
    "stack [%p]\n"
    "{\n"
    "    size = %ld\n"
    "    capacity = %ld\n"
    "    data \n        {\n",
    file, function, file, line, stk, stk -> size, stk -> capacity);

    fprintf(errors, "            [0] = %d   canary_beginning", stk -> data[0]);
    if(stk->data[0] == CANARY_BEGINNING)
    {
        fprintf(errors, " (alive)\n");
    }
    else
    {
        fprintf(errors, " (dead)((\n");
    }

    for(int i = 1; i < stk -> capacity - 1; i++) 
    {
        fprintf(errors, "            [%d] = %d", i, stk -> data[i]);

        if(stk -> data[i] == POIZON)
        {
            fprintf(errors, "    poizoned");
        }

        fprintf(errors, "\n");
    }

    fprintf(errors, "            [%ld] = %d    canary_end", stk -> capacity - 1, stk -> data[stk -> capacity - 1]);
    if(stk->data[stk -> capacity - 1] == CANARY_END)
    {
        fprintf(errors, " (alive)\n");
    }
    else
    {
        fprintf(errors, " (dead)((\n");
    }
    
    fprintf(errors, "        }\n}\n");

    fclose(errors);
}

void Printer_ERRORS(stack_t* stk)
{
    StackErr_t err = stk -> code_error;

    FILE* errors = fopen("errors.txt", "a");
    
    if(err == ERROR_NO) 
    {
        fprintf(errors, "No errors detected\n");

        return;
    }
    
    fprintf(errors, "Stack errors detected (error code: %d):\n", err);
    
    if(err & ERROR_POINTER_TO_STACK) fprintf(errors, "ERROR_POINTER_TO_STACK: Stack pointer is NULL\n");
    if(err & ERROR_POINTER_DATA)     fprintf(errors, "ERROR_POINTER_DATA: Data array is NULL\n");
    if(err & ERROR_WRONG_SIZE)       fprintf(errors, "ERROR_WRONG_SIZE: Invalid size value\n");
    if(err & ERROR_WRONG_CAPACITY)   fprintf(errors, "ERROR_WRONG_CAPACITY: Invalid capacity value\n");
    if(err & ERROR_CANARY_LEFT)      fprintf(errors, "ERROR_CANARY_LEFT: Left Canary is dead\n");
    if(err & ERROR_CANARY_RIGHT)     fprintf(errors, "ERROR_CANARY_RIGHT: Right Canary is dead\n");

    fprintf(errors, "\n");

    fclose(errors);
}
