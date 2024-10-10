#include <stdio.h>
#include <string.h>

#include "proc.h"
#include "stack.h"

unsigned int StackSize = 8;

void Run()
{
    StackId_t stkId = STACK_CTOR(StackSize);

    bool running = true;

    while (running)
    {
        int cmd = -1;

        scanf("%d", &cmd);

        switch (cmd)
        {
            case HLT:
            {
                running = false;

                break;
            }

            case PUSH:
            {
                StackElem_t arg = 0;

                scanf("%d", &arg);

                StackPush(stkId, arg);

                break;
            }

            case ADD:
            {
                StackElem_t a = StackPop(stkId);

                StackElem_t b = StackPop(stkId);

                StackPush(stkId, a + b);

                break;
            }

            case SUB:
            {
                StackElem_t a = StackPop(stkId);

                StackElem_t b = StackPop(stkId);

                StackPush(stkId, b - a);

                break;
            }

            case MUL:
            {
                StackElem_t a = StackPop(stkId);

                StackElem_t b = StackPop(stkId);

                StackPush(stkId, a * b);

                break;
            }


            case DIV:
            {
                StackElem_t a = StackPop(stkId);

                StackElem_t b = StackPop(stkId);

                StackPush(stkId, b / a);

                continue;
            }


            case OUT:
            {
                StackElem_t a = StackPop(stkId);

                printf("%d\n", a);

                break;
            }


            case DUMP:
            {
                SpecialStackDump(stkId);

                break;
            }

            default:
                fprintf(stderr, "Syntax error: %d\n", cmd);
                break;
        }
    }

    StackDtor(stkId);
}
