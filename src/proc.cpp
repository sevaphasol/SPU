#include <stdio.h>
#include <string.h>

#include "proc.h"
#include "stack.h"

unsigned int StackSize = 8;

void Run(int codeArr[ArrSize])
{
    StackId_t stkId = STACK_CTOR(StackSize);

    bool running = true;

    int ip = 0;

    while (running)
    {
        switch (codeArr[ip])
        {
            case HLT:
            {
                running = false;

                break;
            }

            case PUSH:
            {
                StackElem_t arg = codeArr[++ip];

                StackPush(stkId, arg);

                ip++;

                break;
            }

            case ADD:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                StackPush(stkId, a + b);

                ip++;

                break;
            }

            case SUB:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                StackPush(stkId, b - a);

                ip++;

                break;
            }

            case MUL:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                StackPush(stkId, a * b);

                ip++;

                break;
            }


            case DIV:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                StackPush(stkId, b / a);

                ip++;

                continue;
            }


            case OUT:
            {
                StackElem_t a = StackPop(stkId);

                printf("%d\n", a);

                ip++;

                break;
            }


            case DUMP:
            {
                SpecialStackDump(stkId);

                ip++;

                break;
            }

            default:
                fprintf(stderr, "Syntax error: %d\n", codeArr[ip]);

                running = false;

                break;
        }
    }

    StackDtor(stkId);
}
