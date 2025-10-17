#include "SPU.h"
#include "stack.h"

int Reader(FILE* byte_code, SPU_t* SPU1)
{
    int code_size = 0;
    fscanf(byte_code, "%d", &code_size);

    // printf("code_size = %d\n", code_size);

    SPU1 -> code = (int*)calloc((size_t)code_size + 2, sizeof(int));

    // printf("SPU1 calloc - %p", SPU1 -> code);

    SPU1 -> code[0] = code_size;

    for(int i = 1; i < code_size + 1; i++)
    {
        fscanf(byte_code, "%d", &(SPU1 -> code[i]));
    }

    return code_size;
}

void Runner(stack_t* stk)
{
    SPU_t SPU1;
    stack_t stk_ret_arr;

    StackInit(&stk_ret_arr, 10);

    SPU1.stack = stk;
    SPU1.IP = 1;

    FILE* byte_code = fopen("byte_code.txt", "r");
    if (!byte_code) {
        printf("Error: Cannot open byte_code.txt\n");
        return;
    }

    int code_size = Reader(byte_code, &SPU1);
    fclose(byte_code);

    if (code_size <= 0) {
        printf("Error: Invalid code size\n");
        return;
    }

    while (SPU1.IP < code_size + 1)
    {
        int command = SPU1.code[SPU1.IP];
        
        switch(command)
        {
            case PUSH:
            {  
                StackPush(stk, SPU1.code[++SPU1.IP]);
            }
            break;
            
            case ADD:
            {
                int first_value = 0, second_value = 0;
                StackPop(stk, &second_value);
                StackPop(stk, &first_value);
                StackPush(stk, first_value + second_value);
            }
            break;
            
            case SUB:
            {
                int first_value = 0, second_value = 0;
                StackPop(stk, &second_value);
                StackPop(stk, &first_value);
                StackPush(stk, first_value - second_value);
            }
            break;
            
            case MUL:
            {
                int first_value = 0, second_value = 0;
                StackPop(stk, &second_value);
                StackPop(stk, &first_value);
                StackPush(stk, first_value * second_value);
            }
            break;
            
            case DIV:
            {
                int first_value = 0, second_value = 0;
                StackPop(stk, &second_value);
                StackPop(stk, &first_value);
                StackPush(stk, first_value / second_value);
            }
            break;
            
            case SQvRT:
            {
                int value = 0;
                StackPop(stk, &value);
                StackPush(stk, (data_t)sqrt(value));
            }
            break;
            
            case OUT:
            {
                int value = 0;
                StackPop(stk, &value);
                printf("Output: %d\n", value);
            }
            break;
            
            case HLT:
                printf("Program halted\n");
                StackDestroy(&stk_ret_arr);
                SPUDestroy(&SPU1);
                return;

            case PUSHR:
            {
                int first_value = 0;
                first_value = SPU1.regs[SPU1.code[++SPU1.IP] - 1];
                StackPush(stk, first_value);
            }
            break;

            case POPR:
            {
                int value = 0;
                StackPop(stk, &value);
                SPU1.regs[SPU1.code[++SPU1.IP] - 1] = value;
            }
            break;

            case IN:
            {
                int value = 0;
                scanf("%d", &value);
                StackPush(stk, value);
            }
            break;
             
            case JB:
            {
                int first_value = 0, second_value = 0;
                StackPop(stk, &second_value);
                StackPop(stk, &first_value);
                if(first_value < second_value) {
                    SPU1.IP = SPU1.code[SPU1.IP + 1];
                    continue; // Пропускаем инкремент в конце
                } else {
                    SPU1.IP++; // Пропускаем адрес перехода
                }
            }
            break;

            case CALL:
            {
                StackPush(&stk_ret_arr, SPU1.IP + 2); // Сохраняем адрес возврата
                SPU1.IP = SPU1.code[SPU1.IP + 1];
                continue; // Пропускаем инкремент
            }
            break;

            case RET:
            {
                int return_addr = 0;
                StackPop(&stk_ret_arr, &return_addr);
                SPU1.IP = return_addr;
                continue; // Пропускаем инкремент
            }
            break;

            default:
                printf("Error: Unknown command %d at position %d\n", command, SPU1.IP);
                break;
        }
        SPU1.IP++;
    }

    StackDestroy(&stk_ret_arr);
    SPUDestroy(&SPU1);
}

void SPUDestroy(SPU_t* SPU)
{
    if (SPU->code) {
        free(SPU->code);
        SPU->code = NULL;
    }
    
    // Предполагая, что StackDestroy принимает stack_t*
    if (SPU->stack) {
        StackDestroy(SPU->stack);
    }

    SPU->IP = -1;
    for(int i = 0; i < 8; i++) {
        SPU->regs[i] = -1;
    }
}

void PriterArrays(data_t* pointer, int size)
{
    for(int i = 0; i < size; i++)
    {
        printf("%d ", pointer[i]);
    }
}


SPUErr_t SPUDump(SPU_t* SPU)
{
    StackDump(SPU -> stack, __FILE__, __LINE__, __FUNCTION__);

    for(int i = 0; i < 8; i++)
    {
        printf("[%d]", SPU -> regs[i]);
    }

    return 0;
}