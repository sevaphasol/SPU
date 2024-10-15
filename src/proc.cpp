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

    StackElem_t ax, bx, cx, dx = 0;

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

            case PUSHR:
            {
                RegisterCode n = (RegisterCode) codeArr[++ip];

                if (n == AX)
                {
                    StackPush(stkId, ax);

                    return;
                }

                if (n == BX)
                {
                    StackPush(stkId, bx);

                    return;
                }

                if (n == CX)
                {
                    StackPush(stkId, cx);

                    return;
                }

                if (n == DX)
                {
                    StackPush(stkId, dx);

                    return;
                }

                ip++;

                break;
            }

            case POP:
            {
                RegisterCode n = (RegisterCode) codeArr[++ip];

                if (n == AX)
                {
                    ax = StackPop(stkId);

                    return;
                }

                if (n == BX)
                {
                    bx = StackPop(stkId);

                    return;
                }

                if (n == CX)
                {
                    cx = StackPop(stkId);

                    return;
                }

                if (n == DX)
                {
                    dx = StackPop(stkId);

                    return;
                }

                ip++;

                break;
            }

            case JMP:
            {
                ip = codeArr[ip + 1];

                getchar();

                break;
            }

            case JA:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                if (a > b)
                {
                    ip = codeArr[ip + 1];

                    getchar();

                    break;
                }

                ip++;

                break;
            }

            case JB:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                if (a < b)
                {
                    ip = codeArr[ip + 1];

                    getchar();

                    break;
                }

                ip++;

                break;
            }

            case JAE:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                if (a >= b)
                {
                    ip = codeArr[ip + 1];

                    getchar();

                    break;
                }

                ip++;

                break;
            }

            case JBE:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                if (a <= b)
                {
                    ip = codeArr[ip + 1];

                    getchar();

                    break;
                }

                ip++;

                break;
            }

            case JE:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                if (a == b)
                {
                    ip = codeArr[ip + 1];

                    getchar();

                    break;
                }

                ip++;

                break;
            }

            case JNE:
            {
                StackElem_t a = StackPop(stkId);
                StackElem_t b = StackPop(stkId);

                if (a != b)
                {
                    ip = codeArr[ip + 1];

                    getchar();

                    break;
                }

                ip++;

                break;
            }

            default:
            {
                fprintf(stderr, "Invalid instruction: %d\n", codeArr[ip]);

                running = false;

                break;
            }
        }
    }

    StackDtor(stkId);
}
