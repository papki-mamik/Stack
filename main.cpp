#include "stack.h"
#include "SPU.h"

int main()
{
    stack_t stk1 = {};
    
    StackInit(&stk1, 10);

    Runner(&stk1);

    StackDestroy(&stk1);

    return 0;
}