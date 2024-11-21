#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <colors.h>

//-------------------------------------------------------------------//

#include "disasm.h"
#include "custom_assert.h"

//———————————————————————————————————————————————————————————————————//

static DisAsmReturnCode OpenCode          (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[]);
static DisAsmReturnCode ParseArgv         (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[]);
static DisAsmReturnCode ReadCode          (DisAsmInfo_t* dis_asm_info);
static DisAsmReturnCode ParseLine         (DisAsmInfo_t* dis_asm_info);
static DisAsmReturnCode GetRegName        (int reg_code, char* reg_name);
static DisAsmReturnCode PrintPushPopArg   (DisAsmInfo_t* dis_asm_info, const PushPopLine_t* line);

//———————————————————————————————————————————————————————————————————//

DisAsmReturnCode DisAsmCtor(DisAsmInfo_t* dis_asm_info, int argc, const char* argv[])
{
    ASSERT(dis_asm_info);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    dis_asm_info->code.elem_size = sizeof(int);

    //-------------------------------------------------------------------//

    VERIFY(OpenCode(dis_asm_info, argc, argv) != DIS_ASM_SUCCESS,
           return DIS_ASM_OPEN_CODE_ERROR);

    VERIFY(ReadCode(dis_asm_info) != DIS_ASM_SUCCESS,
           return DIS_ASM_READ_CODE_ERROR);

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode OpenCode(DisAsmInfo_t* dis_asm_info, int argc, const char* argv[])
{
    ASSERT(dis_asm_info);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    VERIFY(ParseArgv(dis_asm_info, argc, argv) != DIS_ASM_SUCCESS,
           return DIS_ASM_INVALID_ARGV_ERROR);

    //-------------------------------------------------------------------//

    dis_asm_info->input.ptr  = fopen(dis_asm_info->input.name,  "rb");
    dis_asm_info->output.ptr = fopen(dis_asm_info->output.name, "w");

    VERIFY(!(dis_asm_info->input.ptr) || !(dis_asm_info->output.ptr),
           DIS_ASM_FILE_OPEN_ERROR);

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode ParseArgv (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[])
{
    ASSERT(argv);

    //-------------------------------------------------------------------//

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

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode ReadCode (DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);

    VERIFY(fread(&dis_asm_info->code.len, sizeof(size_t), 1, dis_asm_info->input.ptr) != 1,
           return DIS_ASM_FILE_READ_ERROR);

    //-------------------------------------------------------------------//

    dis_asm_info->code.code = (int*) calloc(dis_asm_info->code.len, dis_asm_info->code.elem_size);

    VERIFY(fread(dis_asm_info->code.code,
                 dis_asm_info->code.elem_size,
                 dis_asm_info->code.len,
                 dis_asm_info->input.ptr) != dis_asm_info->code.len,
           return DIS_ASM_FILE_READ_ERROR);

    //-------------------------------------------------------------------//

    #ifdef PRINT_READ_CODE

    printf("ReadCode:\n");

    for (int i = 0; i < dis_asm_info->code.len; i++)
    {
        printf("[%d] = %d\n", i, dis_asm_info->code.code[i]);
    }

    #endif

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode BuildCode (DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);

    //-------------------------------------------------------------------//

    while (dis_asm_info->code.ip < dis_asm_info->code.len)
    {
        VERIFY(ParseLine(dis_asm_info) != DIS_ASM_SUCCESS,
               return DIS_ASM_PARSE_INSTRUCTION);
    }

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode ParseLine (DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);

    //-------------------------------------------------------------------//

    for (int cmd_index = 0; cmd_index < CommandsAmount; cmd_index++)
    {
        Command_t command = CommandsTabel[cmd_index];

        if (dis_asm_info->code.code[dis_asm_info->code.ip] == command.code)
        {
            if (dis_asm_info->asm_option)
                fprintf(dis_asm_info->output.ptr, "%s ", command.name);

            color_print(RED_CODE,   "%5x:", dis_asm_info->code.ip);
            color_print(BLUE_CODE,  "%3s%02x ", "", command.code);
            color_print(BLUE_CODE,  "%02x", dis_asm_info->code.code[dis_asm_info->code.ip]);
            color_print(GREEN_CODE, "%10s ", command.name);

            dis_asm_info->code.ip++;

            if (command.parse_func)
            {
                VERIFY(command.parse_func(dis_asm_info),
                       return DIS_ASM_PARSE_FUNC_ERROR);
            }

            if (dis_asm_info->asm_option)
                fputc('\n', dis_asm_info->output.ptr);

            putchar('\n');

            return DIS_ASM_SUCCESS;
        }
    }

    //-------------------------------------------------------------------//

    fprintf(stderr, "INVALID INSTRUCTION ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return DIS_ASM_INVALID_INSTRUCTION_ERROR;
}

//===================================================================//

DisAsmReturnCode ParsePushPopArg(DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);

    //-------------------------------------------------------------------//

    printf("%5s", "");

    int arg_type = dis_asm_info->code.code[dis_asm_info->code.ip++];

    for (int i = 0; i < nLines; i++)
    {
        if (PushPopLines[i].arg_type == arg_type)
        {
            VERIFY(PrintPushPopArg(dis_asm_info, &PushPopLines[i]),
                   return DIS_ASM_PRINT_PUSH_POP_ARG_ERROR);

            return DIS_ASM_SUCCESS;
        }
    }

    fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR;
}

//===================================================================//

DisAsmReturnCode PrintPushPopArg(DisAsmInfo_t* dis_asm_info, const PushPopLine_t* line)
{
    ASSERT(dis_asm_info);
    ASSERT(line);

    //-------------------------------------------------------------------//

    int  imc;
    int  reg_code;
    char reg_name[MaxRegNameSize] = {};

    if (line->need_imc)
    {
        imc = dis_asm_info->code.code[dis_asm_info->code.ip++];
    }

    if (line->need_reg)
    {
        reg_code = dis_asm_info->code.code[dis_asm_info->code.ip++];

        VERIFY(GetRegName(reg_code, reg_name) != DIS_ASM_SUCCESS,
               return DIS_ASM_INVALID_REGISTER_CODE_ERROR);
    }

    //-------------------------------------------------------------------//

    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ////////////////WARNING! COPYPAST///////////////////
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////


    switch (line->printf_order)
    {
        case IMC:
        {
            if (dis_asm_info->asm_option)
            {
                fprintf(dis_asm_info->output.ptr, line->format_line, imc);
            }

            color_print(PURPLE_CODE, line->format_line, imc);

            break;imc
        }
        case REG:
        {
            if (dis_asm_info->asm_option)
            {
                fprintf(dis_asm_info->output.ptr, line->format_line, reg_name);
            }

            color_print(PURPLE_CODE, line->format_line, reg_name);

            break;
        }
        case IMC_REG:
        {
            if (dis_asm_info->asm_option)
            {
                fprintf(dis_asm_info->output.ptr, line->format_line, imc, reg_name);
            }

            color_print(PURPLE_CODE, line->format_line, imc, reg_name);

            break;
        }
    }

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode GetRegName(int reg_code, char reg_name[MaxRegNameSize])
{
    ASSERT(reg_name);

    //-------------------------------------------------------------------//

    VERIFY(reg_code < 1 || reg_code > RegistersAmount,
           return DIS_ASM_INVALID_REGISTER_CODE_ERROR);

    memcpy(reg_name, RegistersTabel[reg_code - 1].name, MaxRegNameSize);

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode ParseLabelArg(DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);

    //-------------------------------------------------------------------//

    int new_ip = dis_asm_info->code.code[dis_asm_info->code.ip++];

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%d ", new_ip);
    }

    color_print(PURPLE_CODE, "%5s%02x", "", new_ip);

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode ParseDrawArg(DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);

    //-------------------------------------------------------------------//

    int mode = dis_asm_info->code.code[dis_asm_info->code.ip++];

    if (dis_asm_info->asm_option)
    {
        fprintf(dis_asm_info->output.ptr, "%d ", mode);
    }

    //-------------------------------------------------------------------//

    VERIFY(ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS,
           return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR);

    VERIFY(ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS,
           return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR);

    VERIFY(ParsePushPopArg(dis_asm_info) != DIS_ASM_SUCCESS,
           return DIS_ASM_PARSE_PUSH_POP_ARG_ERROR);

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//===================================================================//

DisAsmReturnCode CloseCode (DisAsmInfo_t* dis_asm_info)
{
    ASSERT(dis_asm_info);
    ASSERT(dis_asm_info->input.ptr);
    ASSERT(dis_asm_info->output.ptr);

    //-------------------------------------------------------------------//

    fclose(dis_asm_info->input.ptr);
    fclose(dis_asm_info->output.ptr);

    //-------------------------------------------------------------------//

    free(dis_asm_info->code.code);

    //-------------------------------------------------------------------//

    return DIS_ASM_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
