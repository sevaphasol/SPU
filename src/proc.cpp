#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

#include "proc.h"
#include "stack.h"

//------------------------------------------------//

SpuReturnCode SpuInfoCtor(SpuInfo_t* spu_info, int argc, const char* argv[])
{
    if (OpenCode(spu_info, argc, argv) != SPU_SUCCESS)
    {
        fprintf(stderr, "OPEN CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_OPEN_CODE_ERROR;
    }

    if (ReadCode(spu_info) != SPU_SUCCESS)
    {
        fprintf(stderr, "READ CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_READ_CODE_ERROR;
    }

    spu_info->stk.id = STACK_CTOR(StackSize);

    REG_CTOR(AX);
    REG_CTOR(BX);
    REG_CTOR(CX);
    REG_CTOR(DX);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode OpenCode(SpuInfo_t* spu_info, int argc, const char* argv[])
{
    if (argc != 3)
    {
        if (argc != 1)
        {
            fprintf(stderr, "INCORRECT INPUT\n input/dump files set as default\n\n");
        }

        spu_info->input.name  = DefaultInput;
        spu_info->dump.name   = DefaultDump;
    }

    else
    {
        spu_info->input.name  = argv[1];
        spu_info->dump.name   = argv[2];
    }

    spu_info->input.ptr  = fopen(spu_info->input.name,  "rb");
    spu_info->dump.ptr   = fopen(spu_info->dump.name,   "w");

    if (!(spu_info->input.ptr) || !(spu_info->dump.ptr))
    {
        fprintf(stderr, "FILE OPEN ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_FILE_OPEN_ERROR;
    }

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode ReadCode(SpuInfo_t* spu_info)
{
    if (fread(&spu_info->proc.len, sizeof(size_t), 1, spu_info->input.ptr) != 1)
    {
        fprintf(stderr, "READ CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_READ_CODE_ERROR;
    }

    spu_info->proc.elem_size = sizeof(int);

    spu_info->proc.code = (int*) calloc(spu_info->proc.len, spu_info->proc.elem_size);

    if (fread(spu_info->proc.code, spu_info->proc.elem_size, spu_info->proc.len, spu_info->input.ptr) !=spu_info->proc.len)                                             \
    {
        fprintf(stderr, "READ CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_READ_CODE_ERROR;
    }

    #ifdef PRINT_READ_CODE

    printf("ReadCode:\n");

    for (int i = 0; i < spu_info->proc.len; i++)
    {
        printf("[%d] = %d\n", i, spu_info->proc.code[i]);
    }

    #endif

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode ExecuteCode(SpuInfo_t* spu_info)
{
    spu_info->proc.running = true;

    while (spu_info->proc.running)
    {
        switch (spu_info->proc.code[spu_info->proc.ip++])
        {
            case HLT:
            {
                CmdHlt(spu_info);

                break;
            }

            case PUSH:
            {
                CmdPush(spu_info);

                break;
            }

            case ADD:
            {
                CmdAdd(spu_info);

                break;
            }

            case SUB:
            {
                CmdSub(spu_info);

                break;
            }

            case MUL:
            {
                CmdMul(spu_info);

                break;
            }

            case DIV:
            {
                CmdDiv(spu_info);

                continue;
            }

            case OUT:
            {
                CmdOut(spu_info);

                break;
            }

            case DUMP:
            {
                CmdDump(spu_info);

                break;
            }

            case POP:
            {
                CmdPop(spu_info);

                break;
            }

            case JMP:
            {
                CmdJmp(spu_info);

                break;
            }

            case JA:
            {
                CmdJa(spu_info);

                break;
            }

            case JB:
            {
                CmdJb(spu_info);

                break;
            }

            case JAE:
            {
                CmdJae(spu_info);

                break;
            }

            case JBE:
            {
                CmdJbe(spu_info);

                break;
            }

            case JE:
            {
                CmdJe(spu_info);

                break;
            }

            case JNE:
            {
                CmdJne(spu_info);

                break;
            }

            case DRAW:
            {
                CmdDraw(spu_info);

                break;
            }

            case CALL:
            {

                CmdCall(spu_info);

                break;
            }

            case RET:
            {
                CmdRet(spu_info);

                break;
            }

            default:
            {
                fprintf(stderr, "INVALID INSTRUCTION ERROR: %d in %s:%d:%s\n", \
                                spu_info->proc.code[spu_info->proc.ip], __FILE__, __LINE__, __PRETTY_FUNCTION__);

                spu_info->proc.running = false;

                return SPU_INVALID_INSRUCTION_ERROR;
            }
        }
    }

    return SPU_SUCCESS;
}

//------------------------------------------------//

int* GetArg(SpuInfo_t* spu_info)
{
    int        arg_type  = spu_info->proc.code[spu_info->proc.ip++];
    static int arg_value;

    arg_value = 0;

    int*       arg_addr  = nullptr;

    if (arg_type & 1)
    {
        arg_value +=  spu_info->proc.code[spu_info->proc.ip];
        arg_addr   = &spu_info->proc.code[spu_info->proc.ip];

        spu_info->proc.ip++;
    }

    if (arg_type & 2)
    {
        int reg_number = spu_info->proc.code[spu_info->proc.ip] - 1;

        arg_value +=  spu_info->proc.regs.regs[reg_number].value;
        arg_addr   = &spu_info->proc.regs.regs[reg_number].value;

        spu_info->proc.ip++;
    }

    if (arg_type & 1 && arg_type & 2)
    {
        arg_addr = &arg_value;
    }

    if (arg_type & 4)
    {
        int ram_addres = arg_value;

        arg_addr = &spu_info->ram.ram[ram_addres];
    }

    return arg_addr;
}

//------------------------------------------------//

SpuReturnCode CmdHlt  (SpuInfo_t* spu_info)
{
    spu_info->proc.running = false;

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdPush (SpuInfo_t* spu_info)
{
    int arg = *GetArg(spu_info);

    StackPush(spu_info->stk.id, arg);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdPop  (SpuInfo_t* spu_info)
{
    int* arg = GetArg(spu_info);

    int val = StackPop(spu_info->stk.id);

    *arg = val;

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdAdd  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    StackPush(spu_info->stk.id, a + b);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdSub  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    StackPush(spu_info->stk.id, b - a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdMul  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    StackPush(spu_info->stk.id, a * b);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdDiv  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (a == 0)
    {
        fprintf(stderr, "DIVISION BY ZERO ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_DIVISION_BY_ZERO_ERROR;
    }

    StackPush(spu_info->stk.id, b / a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdSqrt (SpuInfo_t* spu_info)
{
    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdSin  (SpuInfo_t* spu_info)
{
    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdCos  (SpuInfo_t* spu_info)
{
    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdIn   (SpuInfo_t* spu_info)
{
    int a = 0;
    scanf("Enter value: %d", &a);

    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdOut  (SpuInfo_t* spu_info)
//------------------------------------------------//

{
    int a = StackPop(spu_info->stk.id);

    printf("OUT: %d\n", a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdDump (SpuInfo_t* spu_info)
{
    // fprintf(stderr, "Dumping...\n");

    SpecialStackDump(spu_info->stk.id);

    FILE* dump_file = spu_info->dump.ptr;

    time_t RawTime;
    struct tm* TimeInfo;
    time(&RawTime);
    TimeInfo = localtime(&RawTime);

    fprintf(dump_file, "Local time and date: %s\n", asctime(TimeInfo));

//     Stack_t* stack = Stacks[spu_info->stk.id - 1];
//
//     if (!stack)
//     {
//         fprintf(stderr, "HUY\n");
//     }
//
//     for (int i = 0; i < 16; i++)
//     {
//     fprintf(stderr, "Stacks[%d]: %p\n", i, Stacks[i]);
//     }
//
// fprintf(stderr, "huy\n");
//
// fprintf(stderr, "huy\n");
//
//     for (int i = 0; i < stack->capacity; i++)
//     {
// fprintf(stderr, "huy\n");
//
//         if (i < stack->size)
//         {
//             fprintf(dump_file, "[%d] = %d\n", i, stack->data[i]);
//         }
//         else
//         {
//             fprintf(dump_file, "[%d] = %d (POISON) \n", i, stack->data[i]);
//         }
//     }
//
//     fprintf(dump_file, "\n\n---------------------------------------------------------------------\n\n");

    fprintf(dump_file, "Local time and date: %s\n", asctime(TimeInfo));

    fprintf(dump_file, "input file = %s\n", spu_info->input.name);
    fprintf(dump_file, "input file pointer = %p\n\n", spu_info->input.ptr);

    fprintf(dump_file, "running = %s\n\n", spu_info->proc.running ? "true" : "false");

    fprintf(dump_file, "arr_size = %ld\n",  spu_info->proc.len);
    fprintf(dump_file, "regs_size = %ld\n", spu_info->proc.regs.len);
    fprintf(dump_file, "ram_size = %ld\n",  spu_info->ram.len);

    fprintf(dump_file, "\nlen_code = %ld\n",  spu_info->proc.len);
    fprintf(dump_file, "ip       = %d\n\n", spu_info->proc.ip);

    fprintf(dump_file, "CODE ARRAY\n\n");

    for (int i = 0; i < spu_info->proc.len; i++)
    {
        fprintf(dump_file, "[%d] = %d\n", i, spu_info->proc.code[i]);
    }


    fprintf(dump_file, "\nREGISTERS\n\n");

    for (int i = 0; i < spu_info->proc.regs.len; i++)
    {
        fprintf(dump_file, "%s [ak %d] = %d\n",
                spu_info->proc.regs.regs[i].name, spu_info->proc.regs.regs[i].code, spu_info->proc.regs.regs[i].value);
    }

    fprintf(dump_file, "\nRAM\n\n");

    for (int i = 0; i < 300; i++) // TODO 8 -> spu_info->ram.len
    {
        fprintf(dump_file, "[%d] = %d\n", i, spu_info->ram.ram[i]);
    }

    fflush(dump_file);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJmp  (SpuInfo_t* spu_info)
{
    spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

    // getchar();

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJa   (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b > a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

        // getchar();
    }
    else
    {
        spu_info->proc.ip++;
    }

    StackPush(spu_info->stk.id, b);
    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJb   (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b < a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

        // getchar();
    }
    else
    {
        spu_info->proc.ip++;
    }

    StackPush(spu_info->stk.id, b);
    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJae  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b >= a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

        // getchar();
    }
    else
    {
        spu_info->proc.ip++;
    }

    StackPush(spu_info->stk.id, b);
    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJbe  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b <= a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

        // getchar();
    }
    else
    {
        spu_info->proc.ip++;
    }

    StackPush(spu_info->stk.id, b);
    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJe   (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b == a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

        // getchar();
    }
    else
    {
        spu_info->proc.ip++;
    }

    StackPush(spu_info->stk.id, b);
    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdJne  (SpuInfo_t* spu_info)
{
    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b != a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

        // getchar();
    }

    StackPush(spu_info->stk.id, b);
    StackPush(spu_info->stk.id, a);

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdDraw (SpuInfo_t* spu_info)
{
    int ram_index = spu_info->proc.code[spu_info->proc.ip++];
    int size      = spu_info->proc.code[spu_info->proc.ip++];

    for (int i = 0; i < size; i++)
    {
        for (int j = i * size; j < (i + 1) * size; j++)
        {
            if (spu_info->ram.ram[ram_index + j] == 0)
            {
                printf("\033[37;47m           \033[0m");

                continue;
            }

            if (spu_info->ram.ram[ram_index + j] == 1)
            {
                printf("\033[37;44m           \033[0m");

                continue;
            }

            if (spu_info->ram.ram[ram_index + j] == 2)
            {
                printf("\033[37;41m           \033[0m");

                continue;
            }
        }

        printf("\n");
    }

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdCall(SpuInfo_t* spu_info)
{
    int func_ip = spu_info->proc.code[spu_info->proc.ip];
    int ret_ip  = spu_info->proc.ip + 1;

    StackPush(spu_info->stk.id, ret_ip);

    spu_info->proc.ip = func_ip;

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode CmdRet(SpuInfo_t* spu_info)
{
    int ret_ip = StackPop(spu_info->stk.id);

    spu_info->proc.ip = ret_ip;

    return SPU_SUCCESS;
}

//------------------------------------------------//

SpuReturnCode SpuInfoDtor(SpuInfo_t* spu_info)
{
    if (!spu_info)
    {
        fprintf(stderr, "SPU INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_INFO_NULL_PTR_ERROR;
    }

    if (!spu_info->input.ptr)
    {
        fprintf(stderr, "SPU CLOSE INPUT FILE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_CLOSE_INPUT_FILE_ERROR;
    }

    fclose(spu_info->input.ptr);

    if (!spu_info->dump.ptr)
    {
        fprintf(stderr, "SPU CLOSE DUMP FILE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_CLOSE_DUMP_FILE_ERROR;
    }

    fclose(spu_info->dump.ptr);

    free(spu_info->proc.code);

    StackDtor(spu_info->stk.id);

    return SPU_SUCCESS;
}

//------------------------------------------------//
