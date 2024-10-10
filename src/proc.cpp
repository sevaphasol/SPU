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
        char cmd[50] = "";

        scanf("%s", cmd);

        if (strcmp(cmd, "push") == 0)
        {
            StackElem_t arg = 0;

            scanf("%d", &arg);

            StackPush(stkId, arg);

            continue;
        }

        if (strcmp(cmd, "sub") == 0)
        {
            StackElem_t a = StackPop(stkId);

            StackElem_t b = StackPop(stkId);

            StackPush(stkId, b - a);

            continue;
        }

        if (strcmp(cmd, "add") == 0)
        {
            StackElem_t a = StackPop(stkId);

            StackElem_t b = StackPop(stkId);

            StackPush(stkId, a + b);

            continue;
        }

        if (strcmp(cmd, "mul") == 0)
        {
            StackElem_t a = StackPop(stkId);

            StackElem_t b = StackPop(stkId);

            StackPush(stkId, a * b);

            continue;
        }

        if (strcmp(cmd, "div") == 0)
        {
            StackElem_t a = StackPop(stkId);

            StackElem_t b = StackPop(stkId);

            StackPush(stkId, b / a);

            continue;
        }

        if (strcmp(cmd, "out") == 0)
        {
            StackElem_t a = StackPop(stkId);

            printf("%d\n", a);

            continue;
        }

        if (strcmp(cmd , "dump") == 0)
        {
            SpecialStackDump(stkId);

            continue;
        }

        if (strcmp(cmd, "hlt") == 0)
        {
            running = false;

            continue;
        }

        fprintf(stderr, "Syntax error: %s\n", cmd);
    }

    StackDtor(stkId);
}
