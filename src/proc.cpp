#include <stdio.h>
#include <string.h>
#include <time.h>

#include "proc.h"
#include "stack.h"


void ProcCtor(Proc_t* proc, const int argc, const char* argv[])
{
    if (argc != 2 && argc != 1)
   {
        fprintf(stderr, "INCORRECT INPUT\n");

        return;
    }

    if (argc == 1)
    {
        proc->input_file_name = DefaultInput;
    }
    else
    {
        proc->input_file_name = argv[1];
    }

    proc->input_file      = fopen(proc->input_file_name, "r");

    proc->dump_file_name  = DmpName;
    proc->dump_file       = fopen(proc->dump_file_name,  "w");

    proc->arr_size = CodeArrSize;

    size_t size = 0;

    int arg = 0;

    for ( ; fscanf(proc->input_file, "%d", &arg) != EOF; size++)
    {
        proc->code[size] = arg;
    }

    proc->len_code = size;

    proc->ip = 0;

    proc->stk_size = StackSize;
    proc->stk_id   = STACK_CTOR(proc->stk_size);

    proc->regs_size = RegsSize;
    memset((void*) proc->regs, 0, proc->regs_size);

    proc->ram_size = RamSize;
    memset((void*) proc->ram,  0, proc->ram_size);

    proc->running  = false;
}

void Run(Proc_t* proc)
{
    proc->running = true;

    while (proc->running)
    {
        switch (proc->code[proc->ip++])
        {
            case HLT:
            {
                CmdHlt(proc);

                break;
            }

            case PUSH:
            {
                CmdPush(proc);

                break;
            }

            case ADD:
            {
                CmdAdd(proc);

                break;
            }

            case SUB:
            {
                CmdSub(proc);

                break;
            }

            case MUL:
            {
                CmdMul(proc);

                break;
            }

            case DIV:
            {
                CmdDiv(proc);

                continue;
            }

            case OUT:
            {
                CmdOut(proc);

                break;
            }

            case DUMP:
            {
                CmdDump(proc);

                break;
            }

            case POP:
            {
                CmdPop(proc);

                break;
            }

            case JMP:
            {
                CmdJmp(proc);

                break;
            }

            case JA:
            {
                CmdJa(proc);

                break;
            }

            case JB:
            {
                CmdJb(proc);

                break;
            }

            case JAE:
            {
                CmdJae(proc);

                break;
            }

            case JBE:
            {
                CmdJbe(proc);

                break;
            }

            case JE:
            {
                CmdJe(proc);

                break;
            }

            case JNE:
            {
                CmdJne(proc);

                break;
            }

            default:
            {
                fprintf(stderr, "Invalid instruction: %d\n", proc->code[proc->ip]);

                proc->running = false;

                break;
            }
        }
    }
}

void ProcDtor(Proc_t* proc)
{
    fclose(proc->input_file);
    fclose(proc->dump_file);

    StackDtor(proc->stk_id);
}

int* GetArg(Proc_t* proc)
{
    int        arg_type  = proc->code[proc->ip++];
    static int arg_value;

    arg_value = 0;

    int*       arg_addr  = nullptr;

    if (arg_type & 1)
    {
        arg_value +=  proc->code[proc->ip];
        arg_addr   = &proc->code[proc->ip];

fprintf(stderr, "1: arg value = %d\n", arg_value);

        proc->ip++;
    }

    if (arg_type & 2)
    {
        int reg_number = proc->code[proc->ip] - 1;

        arg_value +=  proc->regs[reg_number];
        arg_addr   = &proc->regs[reg_number];

fprintf(stderr, "2: arg value = %d\n", arg_value);

        proc->ip++;
    }

    if (arg_type & 1 && arg_type & 2)
    {
        arg_addr = &arg_value;
    }

    if (arg_type & 4)
    {
        int ram_addres = arg_value;

fprintf(stderr, "2: ram addres = %d\n", ram_addres);

        arg_addr = &proc->ram[ram_addres];
    }

    return arg_addr;
}

void CmdHlt  (Proc_t* proc)
{
    proc->running = false;
}

void CmdPush (Proc_t* proc)
{
    int arg = *GetArg(proc);

    StackPush(proc->stk_id, arg);
}

void CmdPop  (Proc_t* proc)
{
    int* arg = GetArg(proc);

    int val = StackPop(proc->stk_id);

    *arg = val;
}

void CmdAdd  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    StackPush(proc->stk_id, a + b);
}

void CmdSub  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    StackPush(proc->stk_id, b - a);
}

void CmdMul  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    StackPush(proc->stk_id, a * b);
}

void CmdDiv  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    StackPush(proc->stk_id, b / a);
}

void CmdSqrt (Proc_t* proc)
{

}

void CmdSin  (Proc_t* proc)
{

}

void CmdCos  (Proc_t* proc)
{

}

void CmdIn   (Proc_t* proc)
{
    int a = 0;
    scanf("Enter value: %d", &a);

    StackPush(proc->stk_id, a);
}

void CmdOut  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);

    printf("%d\n", a);
}

void CmdDump (Proc_t* proc)
{
    SpecialStackDump(proc->stk_id);

    FILE* dump_file = proc->dump_file;

    time_t RawTime;
    struct tm* TimeInfo;
    time(&RawTime);
    TimeInfo = localtime(&RawTime);

    fprintf(dump_file, "Local time and date: %s\n", asctime(TimeInfo));

    fprintf(dump_file, "input file = %s\n", proc->input_file_name);
    fprintf(dump_file, "input file pointer = %p\n\n", proc->input_file);

    fprintf(dump_file, "running = %s\n\n", proc->running ? "true" : "false");

    fprintf(dump_file, "arr_size = %ld\n",  proc->arr_size);
    fprintf(dump_file, "regs_size = %ld\n", proc->regs_size);
    fprintf(dump_file, "ram_size = %ld\n",  proc->ram_size);

    fprintf(dump_file, "\nlen_code = %ld\n",  proc->len_code);
    fprintf(dump_file, "ip       = %d\n\n", proc->ip);

    fprintf(dump_file, "CODE ARRAY\n\n");

    for (int i = 0; i < proc->len_code; i++)
    {
        fprintf(dump_file, "[%d] = %d\n", i, proc->code[i]);
        printf("%d\n", proc->code[i]);
    }


    fprintf(dump_file, "\nREGISTERS\n\n");

    fprintf(dump_file, "ax = %d\n", proc->regs[0]);
    fprintf(dump_file, "bx = %d\n", proc->regs[1]);
    fprintf(dump_file, "cx = %d\n", proc->regs[2]);
    fprintf(dump_file, "dx = %d\n", proc->regs[3]);

    fprintf(dump_file, "\nRAM\n\n");

    for (int i = 0; i < proc->ram_size; i++)
    {
        fprintf(dump_file, "[%d] = %d\n", i, proc->ram[i]);
    }
}

void CmdJmp  (Proc_t* proc)
{
    proc->ip = proc->code[proc->ip + 1];

    getchar();
}

void CmdJa   (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    if (a > b)
    {
        proc->ip = proc->code[proc->ip + 1];

        getchar();
    }

    StackPush(proc->stk_id, b);
    StackPush(proc->stk_id, a);
}

void CmdJb   (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    if (a < b)
    {
        proc->ip = proc->code[proc->ip + 1];

        getchar();
    }

    StackPush(proc->stk_id, b);
    StackPush(proc->stk_id, a);
}

void CmdJae  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    if (a >= b)
    {
        proc->ip = proc->code[proc->ip + 1];

        getchar();
    }

    StackPush(proc->stk_id, b);
    StackPush(proc->stk_id, a);
}

void CmdJbe  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    if (a <= b)
    {
        proc->ip = proc->code[proc->ip + 1];

        getchar();
    }

    StackPush(proc->stk_id, b);
    StackPush(proc->stk_id, a);
}

void CmdJe   (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    if (a == b)
    {
        proc->ip = proc->code[proc->ip + 1];

        getchar();
    }

    StackPush(proc->stk_id, b);
    StackPush(proc->stk_id, a);
}

void CmdJne  (Proc_t* proc)
{
    int a = StackPop(proc->stk_id);
    int b = StackPop(proc->stk_id);

    if (a != b)
    {
        proc->ip = proc->code[proc->ip + 1];

        getchar();
    }

    StackPush(proc->stk_id, b);
    StackPush(proc->stk_id, a);
}

void CmdDraw (Proc_t* proc)
{

}
