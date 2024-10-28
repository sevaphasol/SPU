#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm.h"

//------------------------------------------------//

DisAsmReturnCode OpenCode(DisAsmInfo_t* dis_asm_info, int argc, const char* argv[])
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_NULL_PTR_ERROR;
    }

    if (!argv)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_NULL_PTR_ERROR;
    }

    ParseArgv(dis_asm_info, argc, argv);

    dis_asm_info->input.ptr    = fopen(dis_asm_info->input.name,  "rb");
    dis_asm_info->output.ptr   = fopen(dis_asm_info->output.name, "w");

    if (!(dis_asm_info->input.ptr) || !(dis_asm_info->output.ptr))
    {
        fprintf(stderr, "FILE OPEN ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_FILE_OPEN_ERROR;
    }

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseArgv (DisAsmInfo_t* spu_info, int argc, const char* argv[])
{
    if (!argv)
    {
        fprintf(stderr, "INVALID ARGV ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_ARGV_ERROR;
    }

    if (argc != 3)
    {
        if (argc != 2)
        {
            if (argc != 1)
            {
                fprintf(stderr, "INCORRECT INPUT\n I/O files set as default\n\n");
            }

            spu_info->input.name  = DefaultInput;
            spu_info->output.name = DefaultOutput;
        }
        else
        {
            spu_info->input.name  = argv[1];
            spu_info->output.name = DefaultOutput;
        }
    }

    else
    {
        spu_info->input.name  = argv[1];
        spu_info->output.name = argv[2];
    }

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ReadCode(DisAsmInfo_t* dis_asm_info)
{
    if (fread(&dis_asm_info->code.len, sizeof(size_t), 1, dis_asm_info->input.ptr) != 1)
    {
        fprintf(stderr, "READ CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_FILE_READ_ERROR;
    }

    dis_asm_info->code.code = (int*) calloc(dis_asm_info->code.len, dis_asm_info->code.elem_size);

    if (fread(dis_asm_info->code.code, dis_asm_info->code.elem_size, dis_asm_info->code.len, dis_asm_info->input.ptr) != dis_asm_info->code.len)                                             \
    {
        fprintf(stderr, "READ CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_FILE_READ_ERROR;
    }

    #ifdef PRINT_READ_CODE

    printf("ReadCode:\n");

    for (int i = 0; i < dis_asm_info->code.len; i++)
    {
        printf("[%d] = %d\n", i, dis_asm_info->code.code[i]);
    }

    #endif

    return DIS_ASM_SUCCESS;
}

DisAsmReturnCode BuildCode (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    while (dis_asm_info->code.ip < dis_asm_info->code.len)
    {
        if (ParseLine(dis_asm_info) != DIS_ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE LINE INSTURCTION in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return DIS_ASM_PARSE_INSTRUCTION;
        }
    }

    return DIS_ASM_SUCCESS;
}

DisAsmReturnCode ParseLine (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_NULL_PTR_ERROR;
    }

    for (int cmd_index = 0; cmd_index < CommandsAmount; cmd_index++)
    {
        Command_t command = CommandsTabel[cmd_index];

        if (dis_asm_info->code.code[dis_asm_info->code.ip] == command.code)
        {
            switch (command.arg_type)
            {
                case NO_ARG:
                {
                    fprintf(dis_asm_info->output.ptr, "%s\n", command.name);

                    dis_asm_info->code.ip++;

                    return DIS_ASM_SUCCESS;
                }

                case PUSH_POP_ARG:
                {
                    fprintf(dis_asm_info->output.ptr, "%s ", command.name);

                    dis_asm_info->code.ip++;

                    if (ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
                    }

                    fputc('\n', dis_asm_info->output.ptr);

                    return DIS_ASM_SUCCESS;
                }

                case LABEL_ARG:
                {
                    fprintf(dis_asm_info->output.ptr, "%s ", command.name);

                    dis_asm_info->code.ip++;

                    if (ParseLabelArg(dis_asm_info) != DIS_ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return DIS_ASM_PARSE_LABEL_ARG_ERROR;
                    }

                    fputc('\n', dis_asm_info->output.ptr);

                    return DIS_ASM_SUCCESS;
                }

                case DRAW_ARG:
                {
                    fprintf(dis_asm_info->output.ptr, "%s ", command.name);

                    dis_asm_info->code.ip++;

                    if (ParseDrawArg(dis_asm_info) != DIS_ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return DIS_ASM_PARSE_DRAW_ARG_ERROR;
                    }

                    fputc('\n', dis_asm_info->output.ptr);

                    return DIS_ASM_SUCCESS;
                }
            }
        }
    }

    fprintf(stderr, "INVALID INSTRUCTION ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return DIS_ASM_INVALID_INSTRUCTION_ERROR;
}

//------------------------------------------------//

DisAsmReturnCode ParsePushPopArg   (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int arg_type = dis_asm_info->code.code[dis_asm_info->code.ip++];

    switch (arg_type)
    {
        case ImmerseConstCode:
        {
            if (ParseImc(dis_asm_info) != DIS_ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            return DIS_ASM_SUCCESS;
        }

        case RegisterCode:
        {
            if (ParseReg(dis_asm_info) != DIS_ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            return DIS_ASM_SUCCESS;
        }

        case ImmerseConstCode + RegisterCode:
        {
            if (ParseImcReg(dis_asm_info) != DIS_ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            return DIS_ASM_SUCCESS;
        }

        case RamCode + ImmerseConstCode:
        {
            if (ParseImcRam(dis_asm_info) != DIS_ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            return DIS_ASM_SUCCESS;
        }

        case RamCode + RegisterCode:
        {
            if (ParseRegRam(dis_asm_info) != DIS_ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            return DIS_ASM_SUCCESS;
        }

        case RamCode + ImmerseConstCode + RegisterCode:
        {
            if (ParseImcRegRam(dis_asm_info) != DIS_ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            return DIS_ASM_SUCCESS;
        }
    }

    fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
}

//------------------------------------------------//

DisAsmReturnCode ParseImc (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS_ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int imc = dis_asm_info->code.code[dis_asm_info->code.ip++];

    fprintf(dis_asm_info->output.ptr, "%d ", imc);

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseReg (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS_ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int  reg_code                 = dis_asm_info->code.code[dis_asm_info->code.ip++];
    char reg_name[MaxRegNameSize] = {};

    if (GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS || !reg_name)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    fprintf(dis_asm_info->output.ptr, "%s ", reg_name);

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseImcReg (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS_ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int imc = dis_asm_info->code.code[dis_asm_info->code.ip++];

    fprintf(dis_asm_info->output.ptr, "%d + ", imc);

    int  reg_code                 = dis_asm_info->code.code[dis_asm_info->code.ip++];
    char reg_name[MaxRegNameSize] = {};

    if (GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS || !reg_name)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    fprintf(dis_asm_info->output.ptr, "%s ", reg_name);

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseImcRam       (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS_ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int imc = dis_asm_info->code.code[dis_asm_info->code.ip++];

    fprintf(dis_asm_info->output.ptr, "[%d] ", imc);

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseRegRam       (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS_ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int  reg_code                 = dis_asm_info->code.code[dis_asm_info->code.ip++];
    char reg_name[MaxRegNameSize] = {};

    if (GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS || !reg_name)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    fprintf(dis_asm_info->output.ptr, "[%s] ", reg_name);

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseImcRegRam (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS_ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int imc = dis_asm_info->code.code[dis_asm_info->code.ip++];

    fprintf(dis_asm_info->output.ptr, "[%d + ", imc);

    int  reg_code                 = dis_asm_info->code.code[dis_asm_info->code.ip++];
    char reg_name[MaxRegNameSize] = {};

    if (GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS || !reg_name)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    fprintf(dis_asm_info->output.ptr, "%s] ", reg_name);

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode GetRegName (int reg_code, char reg_name[MaxRegNameSize])
{
    if (reg_code < 1 || reg_code > RegistersAmount)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    memcpy(reg_name, RegistersTabel[reg_code - 1].name, MaxRegNameSize);

    return DIS_ASM_SUCCESS;
}

//---------------------------------------------//

DisAsmReturnCode ParseLabelArg (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    int new_ip = dis_asm_info->code.code[dis_asm_info->code.ip++];

    fprintf(dis_asm_info->output.ptr, "%d ", new_ip);

    return DIS_ASM_SUCCESS;
}

//---------------------------------------------//

DisAsmReturnCode ParseDrawArg      (DisAsmInfo_t* dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
   }

   int mode = dis_asm_info->code.code[dis_asm_info->code.ip++];

    fprintf(dis_asm_info->output.ptr, "%d ", mode);

    if (ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS)
    {
        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
    }

    if (ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS)
    {
        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
    }

    return DIS_ASM_SUCCESS;
}

//---------------------------------------------//

DisAsmReturnCode CloseCode         (DisAsmInfo_t*dis_asm_info)
{
    if (!dis_asm_info)
    {
        fprintf(stderr, "DIS ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INFO_NULL_PTR_ERROR;
    }

    if (!dis_asm_info->input.ptr)
    {
        fprintf(stderr, "DIS ASM FILE CLOSE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_FILE_CLOSE_ERROR;
    }

    fclose(dis_asm_info->input.ptr);

    if (!dis_asm_info->output.ptr)
    {
        fprintf(stderr, "DIS ASM FILE CLOSE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_FILE_CLOSE_ERROR;
    }

    fclose(dis_asm_info->output.ptr);

    free(dis_asm_info->code.code);

    return DIS_ASM_SUCCESS;
}
