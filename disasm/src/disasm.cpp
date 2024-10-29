#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <colors.h>

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

    if (ParseArgv(dis_asm_info, argc, argv) != DIS_ASM_SUCCESS)
    {
        fprintf(stderr, "DIS ASM INVALID ARGV ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_ARGV_ERROR;
    }

    dis_asm_info->input.ptr  = fopen(dis_asm_info->input.name,  "rb");
    dis_asm_info->output.ptr = fopen(dis_asm_info->output.name, "w");

    if (!(dis_asm_info->input.ptr) || !(dis_asm_info->output.ptr))
    {
        fprintf(stderr, "FILE OPEN ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_FILE_OPEN_ERROR;
    }

    return DIS_ASM_SUCCESS;
}

//------------------------------------------------//

DisAsmReturnCode ParseArgv (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[])
{
    if (!argv)
    {
        fprintf(stderr, "INVALID ARGV ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_ARGV_ERROR;
    }

    int option_index = 1;
    int parsed_files = 0;

    while (option_index < argc)
	{
        if (strcmp(argv[option_index], "--asm") == 0 || strcmp(argv[option_index], "-a") == 0)
		{
            dis_asm_info->asm_option = true;

			option_index++;

            continue;
        }

        if (parsed_files == 0)
        {
            dis_asm_info->input.name  = argv[option_index];

            parsed_files++;

            option_index++;

            continue;
        }

        if (parsed_files == 1)
        {
            dis_asm_info->output.name = argv[option_index];

            parsed_files++;

            option_index++;

            continue;
        }

        fprintf(stderr, "INCORRECT INPUT\n I/O files set as default\n\n");

        return DIS_ASM_INVALID_ARGV_ERROR;
    }

    if (parsed_files > 2)
    {
        fprintf(stderr, "INCORRECT INPUT\n I/O files set as default\n\n");

        return DIS_ASM_INVALID_ARGV_ERROR;
    }
    else if (parsed_files != 2)
    {
        if (parsed_files == 1)
        {
            dis_asm_info->output.name = DefaultOutput;
        }
        else
        {
            dis_asm_info->input.name  = DefaultInput;
            dis_asm_info->output.name = DefaultOutput;
        }
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
                    if (dis_asm_info->asm_option)
                    {
                        fprintf(dis_asm_info->output.ptr, "%s\n", command.name);
                    }

                    color_print (RED_CODE,   "%5x:", dis_asm_info->code.ip);
                    color_print (BLUE_CODE,  "%3s%02x ", "", command.code);
                    color_print (BLUE_CODE,  "%02x", dis_asm_info->code.code[dis_asm_info->code.ip]);
                    color_print (GREEN_CODE, "%10s\n", command.name);

                    dis_asm_info->code.ip++;

                    return DIS_ASM_SUCCESS;
                }

                case PUSH_POP_ARG:
                {
                    if (dis_asm_info->asm_option)
                    {
                        fprintf(dis_asm_info->output.ptr, "%s ", command.name);
                    }

                    color_print (RED_CODE,   "%5x:", dis_asm_info->code.ip);
                    color_print (BLUE_CODE,  "%3s%02x ", "", command.code);
                    color_print (BLUE_CODE,  "%02x", dis_asm_info->code.code[dis_asm_info->code.ip]);
                    color_print (GREEN_CODE, "%10s", command.name);

                    dis_asm_info->code.ip++;

                    printf("%5s", "");

                    if (ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%ss\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
                    }

                    fputc('\n', dis_asm_info->output.ptr);

                    putchar('\n');

                    return DIS_ASM_SUCCESS;
                }

                case LABEL_ARG:
                {
                    if (dis_asm_info->asm_option)
                    {
                        fprintf(dis_asm_info->output.ptr, "%s ", command.name);
                    }
                    color_print (RED_CODE,   "%5x:", dis_asm_info->code.ip);
                    color_print (BLUE_CODE,  "%3s%02x ", "", command.code);
                    color_print (BLUE_CODE,  "%02x", dis_asm_info->code.code[dis_asm_info->code.ip]);
                    color_print (GREEN_CODE, "%10s", command.name);

                    dis_asm_info->code.ip++;

                    if (ParseLabelArg(dis_asm_info) != DIS_ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return DIS_ASM_PARSE_LABEL_ARG_ERROR;
                    }

                    fputc('\n', dis_asm_info->output.ptr);

                    putchar('\n');

                    return DIS_ASM_SUCCESS;
                }

                case DRAW_ARG:
                {
                    if (dis_asm_info->asm_option)
                    {
                        fprintf(dis_asm_info->output.ptr, "%s ", command.name);
                    }
                    color_print (RED_CODE,   "%5x:", dis_asm_info->code.ip);
                    color_print (BLUE_CODE,  "%3s%02x ", "", command.code);
                    color_print (BLUE_CODE,  "%02x", dis_asm_info->code.code[dis_asm_info->code.ip]);
                    color_print (GREEN_CODE, "%10s", command.name);

                    dis_asm_info->code.ip++;

                    if (ParseDrawArg(dis_asm_info) != DIS_ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return DIS_ASM_PARSE_DRAW_ARG_ERROR;
                    }

                    fputc('\n', dis_asm_info->output.ptr);

                    putchar('\n');

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%d ", imc);
    }
    color_print(PURPLE_CODE, "%d", imc);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%s ", reg_name);
    }
    color_print(PURPLE_CODE, "%s", reg_name);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%d + ", imc);
    }
    color_print(PURPLE_CODE, "%d + ", imc);

    int  reg_code                 = dis_asm_info->code.code[dis_asm_info->code.ip++];
    char reg_name[MaxRegNameSize] = {};

    if (GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS || !reg_name)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%s ", reg_name);
    }
    color_print(PURPLE_CODE, "%s", reg_name);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "[%d] ", imc);
    }

    color_print(PURPLE_CODE, "[%d]", imc);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "[%s] ", reg_name);
    }

    color_print(PURPLE_CODE, "[%s]", reg_name);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "[%d + ", imc);
    }

    color_print(PURPLE_CODE, "[%d + ", imc);

    int  reg_code                 = dis_asm_info->code.code[dis_asm_info->code.ip++];
    char reg_name[MaxRegNameSize] = {};

    if (GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS || !reg_name)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_INVALID_REGISTER_CODE_ERROR;
    }

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%s] ", reg_name);
    }

    color_print(PURPLE_CODE, "%s]", reg_name);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%d ", new_ip);
    }
    color_print(PURPLE_CODE, "%5s%02x", "", new_ip);

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

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%d ", mode);
    }

    color_print(PURPLE_CODE, "%5s", "");

    if (ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS)
    {
        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
    }

    color_print(PURPLE_CODE, "%2s", "");

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
