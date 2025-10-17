#include "assembler_second_generation.h"

const struct command commands_list[] = 
{
    {PUSH,  "PUSH",  ARG_NUMBER},
        {ADD,   "ADD",   ARG_NONE},
        {SUB,   "SUB",   ARG_NONE},
        {MUL,   "MUL",   ARG_NONE},
        {SQvRT, "SQvRT", ARG_NONE},
        {DIV,   "DIV",   ARG_NONE},
        {OUT,   "OUT",   ARG_NONE},
        {HLT,   "HLT",   ARG_NONE},
        {RET,   "RET",   ARG_NONE},
        {IN,    "IN",    ARG_NONE},
            {JUMP,  "JUMP",  ARG_LABEL},
            {JE,    "JE",    ARG_LABEL},
            {JNE,   "JNE",   ARG_LABEL},
            {JA,    "JA",    ARG_LABEL},
            {JAE,   "JAE",   ARG_LABEL},
            {JB,    "JB",    ARG_LABEL},
            {JBE,   "JBE",   ARG_LABEL},
            {CALL,  "CALL",  ARG_LABEL},
                {PUSHR, "PUSHR", ARG_REGISTER},
                {POPR,  "POPR",  ARG_REGISTER}
};

const struct regs regs_list[] = 
{
    {RAX, "AX"}, {RBX, "BX"}, 
    {RCX, "CX"}, {RDX, "DX"}, 
    {REX, "EX"}, {RFX, "FX"}, 
    {RGX, "GX"}, {RHX, "HX"}
};

//****************************************************************************************************************************************************************************** */

int main()
{
    asm_t asm1;

    AsmInit(&asm1);

    FILE* pointer_to_file = fopen("quadratka.asm", "r");

    asm1.src_buf.data = MakeBuffer(pointer_to_file, &(asm1.src_buf));

    asm1.line_array = MakeLineArray(&asm1.src_buf, &asm1.count);

    asm1.output.code_array = (int*)calloc(asm1.count + 1 ,sizeof(int));

    struct fix_up* fix_up1 = (struct fix_up*)calloc(1, sizeof(struct fix_up));

    struct data_fix_up fx_data = 
    {
        .ptr = fix_up1,
        .count_fix_ups = 0,
        .capacity_fix_ups = 1
    };

    FillByteCode(&asm1, &fx_data);

    CheckLabels(&asm1, fx_data);

    FILE* pointer_output = fopen("byte_code.txt", "a");

    // PrintByteCode(asm1.output, stderr);

    PrintByteCode(asm1.output, pointer_output);

    return 0;
}

//*************************************************************************************** 

void AsmInit(asm_t* asmc)
{
    asmc->commands_list = commands_list;
    asmc->num_commands = (sizeof(commands_list))/(sizeof(struct command));

    asmc->regs_list = regs_list;
    asmc->num_regs = (sizeof(regs_list))/(sizeof(struct regs));

    //source_buffer init
    asmc->src_buf.data = NULL;
    asmc->src_buf.size = 1;
    
    //line_arrary Init
    asmc->line_array = NULL;
    asmc->count = 0;
    
    //byte_code init
    asmc->output.code_array = NULL;
    asmc->output.code_size = 1;
    for(int i = 0; i < 9; i++) (asmc->output).labels[i] = -1;
}

//*************************************************************************************** 

char* MakeBuffer(FILE* pointer_to_file, struct source_buffer* src_buf)
{
    struct stat info;
    
    if(fstat(fileno(pointer_to_file), &info) != 0)
    {
        return NULL;
    }
    
    src_buf->size = info.st_size;
    
    src_buf->data = (char*)calloc(size_t(src_buf->size + 1), sizeof(char));
    
    assert(src_buf->data != NULL);
    
    fread(src_buf->data, sizeof(char), size_t(src_buf->size), pointer_to_file);
    src_buf->data[src_buf->size] = '\0';
    
    for(int i = 0; i < src_buf->size; i++)
    {
        if(src_buf->data[i] == ';')
            while(src_buf->data[i] != '\n')
            {
                src_buf->data[i] = '\n';
                i++;
            }
    }

// puts(src_buf->data);

for(int i = 0; i < src_buf->size; i++)
{
    if((src_buf->data[i] == '\n') || (src_buf->data[i] == ' ') || (src_buf->data[i] == '\r')) src_buf->data[i] = '\0';
}

return src_buf->data;
}

//*************************************************************************************************************************************************************** */

struct lines* MakeLineArray(struct source_buffer* buf, long int* count)
{
    size_t line_number = 0;

    for(long int i = 0; i < buf->size; i++)
    {
        if(buf->data[i] == '\0')
        {
            (*count)++;
            
            while((buf->data[i] == '\0') && i < buf->size) i++;
        }
    }

    (*count)++;
    
    struct lines* line_array = (struct lines*)calloc(*count, sizeof(struct lines));
    
    line_array[0].line_ptr = buf->data;
    line_array[0].length = strlen(buf->data);
    
    for(long int i = 0; i < buf->size; i++)
    {
        if(buf->data[i] == '\0')
        {
            line_number++;
            
            while(buf->data[i] == '\0') i++;

            line_array[line_number].line_ptr = &buf->data[i];
            line_array[line_number].length = strlen(line_array[line_number].line_ptr);
        }
    }

    return line_array;
}

//****************************************************************************************************** */

void FillByteCode(asm_t* asmc, struct data_fix_up* fix_up)
{
    for(int current_line = 0; current_line < asmc->count; current_line++)
    {
        if(*(asmc->line_array[current_line].line_ptr) == ':')
        {
            int label_ptr;
            sscanf((asmc->line_array[current_line].line_ptr), ":%d", &label_ptr);
                    
            asmc->output.labels[label_ptr] = asmc->output.code_size;
        }
        else
        {
            int num_func = -1;
    
            for(int j = 0; j < asmc->num_commands; j++)
            {
                if(strcasecmp(asmc->line_array[current_line].line_ptr, asmc->commands_list[j].name) == 0)
                {
                    num_func = asmc->commands_list[j].opcode;
                    break;
                }
            }
    
            asmc->output.code_array[asmc->output.code_size++] = num_func;

            if(asmc->commands_list[num_func].operand_type != ARG_NONE)
            {
                current_line++;
    
                if(asmc->commands_list[num_func].operand_type == ARG_NUMBER)
                {
                    int value = 0;
                    sscanf(asmc->line_array[current_line].line_ptr, "%d", &value);
                    asmc->output.code_array[asmc->output.code_size] = value;
                }
                else if(asmc->commands_list[num_func].operand_type == ARG_REGISTER)
                {
                    int reg = 0;
                    SscanfNumReg(asmc->line_array[current_line].line_ptr, &reg, asmc->regs_list, asmc->num_regs);
                    asmc->output.code_array[asmc->output.code_size] = reg;
                }
                else if(asmc->commands_list[num_func].operand_type == ARG_LABEL)
                {
                    int label = 0;
                    
                    if(*(asmc->line_array[current_line].line_ptr) == ':')
                    {
                        int label_ptr = 0;
                        sscanf((asmc->line_array[current_line].line_ptr), ":%d", &label_ptr);
                        
                        if(asmc->output.labels[label_ptr] != -1) 
                        {
                            label = asmc->output.labels[label_ptr];
                        }
                        else 
                        {
                            if(fix_up->count_fix_ups == fix_up->capacity_fix_ups)
                            {
                                fix_up->capacity_fix_ups *= 2;
                                fix_up->ptr = (struct fix_up*)realloc(fix_up->ptr, fix_up->capacity_fix_ups * sizeof(struct fix_up));
                            }

                            fix_up->ptr[fix_up->count_fix_ups].line = asmc->output.code_size;
                            fix_up->ptr[fix_up->count_fix_ups].label_ptr = label_ptr;
                            fix_up->count_fix_ups++;
                            
                            label = -1;
                        }
                    }
                    else
                    {
                        sscanf((asmc->line_array[current_line].line_ptr), "%d", &label);
                    }
                    asmc->output.code_array[asmc->output.code_size] = label;
                }

                asmc->output.code_size++;
            }
        }
    }

    asmc->output.code_array[0] = asmc->output.code_size;
}

//*********************************************************************************************************************************************************** */

void SscanfNumReg(char* stk, int* reg, const struct regs* regs_list, long int num_regs)
{
    for(int j = 0; j < num_regs; j++)
    {
        if(strcasecmp(stk, regs_list[j].r_name) == 0)
        {
                *reg = regs_list[j].num_reg;
                break;
        }
    }
}

//**************************************************************************************************************************************************** */

void CheckLabels(asm_t* asmc, struct data_fix_up fix_up)
{
    for(int i = 0; i < fix_up.count_fix_ups; i++)
    {
        int label_pos = asmc->output.labels[fix_up.ptr[i].label_ptr];
        if(label_pos != -1) 
        {
            asmc->output.code_array[fix_up.ptr[i].line] = label_pos;
        }
        else 
        {
            fprintf(stderr, "Error: undefined label :%d\n", fix_up.ptr[i].label_ptr);
        }
    }
}

//****************************************************************************************************************************************************** */

void PrintByteCode(struct byte_code output, FILE* pointer_output)
{
    fprintf(pointer_output,"Byte_code:\n");
    for(int i = 0; i < output.code_size; i++)
    {
        fprintf(pointer_output, "%d ", output.code_array[i]);
    }        
    fprintf(pointer_output,"\n");
}