#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <getopt.h>

//-------------------------------------------------------------------//

#include "asm.h"
#include "custom_assert.h"

//———————————————————————————————————————————————————————————————————//

static AsmReturnCode OpenCode           (AsmInfo_t* asm_info, int argc, const char* argv[]);
static AsmReturnCode ParseArgv          (AsmInfo_t* asm_info, int argc, const char* argv[]);
static AsmReturnCode ReadCode           (AsmInfo_t* asm_info);
static AsmReturnCode GetFileSize        (FILE* const file, size_t* file_size);
static AsmReturnCode ParseLine          (AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars);
static AsmReturnCode GetCurString       (AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars);
static AsmReturnCode SkipIfEmpty        (char** str, char* cmd, char* arg, int* nchars);
static AsmReturnCode AddIfLabel         (AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars);
static AsmReturnCode ParseIfCommand     (AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars);
static RegCode       GetRegCode         (const char* str);
bool                 ScanedSuccessfully (PushPopLine_t* line, char* arg, int* imc, char* reg);
static AsmReturnCode ParseNumLabel      (AsmInfo_t* asm_info, int label);
static AsmReturnCode ParseNamedLabel    (AsmInfo_t* asm_info, const char* label);
static AsmReturnCode AddLabel           (AsmInfo_t* asm_info,   const char* cmd);
static AsmReturnCode InsertPushPopArg   (AsmInfo_t* asm_info, int imc, const char* reg,
                                         int need_imc, int need_reg, int need_ram);

//———————————————————————————————————————————————————————————————————//

AsmReturnCode AsmCtor(AsmInfo_t* asm_info, int argc, const char* argv[])
{
    ASSERT(asm_info);

    //-------------------------------------------------------------------//

    VERIFY(OpenCode(asm_info, argc, argv) != ASM_SUCCESS,
           return ASM_OPEN_CODE_ERROR);

    VERIFY(ReadCode(asm_info) != ASM_SUCCESS,
           return ASM_READ_CODE_ERROR);

    //-------------------------------------------------------------------//

    asm_info->code.code      = (int*) calloc(asm_info->input.size, sizeof(int));
    asm_info->code.elem_size = sizeof(int);

    //-------------------------------------------------------------------//

    asm_info->labels.len    = LabelsSize;
    asm_info->labels.labels = (Label_t*) calloc(asm_info->labels.len, sizeof(Label_t));

    //-------------------------------------------------------------------//

    asm_info->labels.fix_up_table.fix_ups
                               = (FixUpElem_t*) calloc(asm_info->input.size, sizeof(FixUpElem_t));
    asm_info->labels.elem_size = sizeof(int);

    //-------------------------------------------------------------------//

    asm_info->code.len = asm_info->input.size;

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode OpenCode (AsmInfo_t* asm_info, int argc, const char* argv[])
{
    ASSERT(asm_info);

    //-------------------------------------------------------------------//

    VERIFY(ParseArgv(asm_info, argc, argv) != ASM_SUCCESS,
           return ASM_PARSE_ARGV_ERROR);

    //-------------------------------------------------------------------//

    asm_info->input.ptr = fopen(asm_info->input.name,  "r");
    VERIFY(!(asm_info->input.ptr), return ASM_FILE_OPEN_ERROR);

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode ParseArgv (AsmInfo_t* asm_info, int argc, const char* argv[])
{
    ASSERT(argv);

    //-------------------------------------------------------------------//

    if (argc != 3)
    {
        if (argc != 2)
        {
            if (argc != 1)
            {
                fprintf(stderr, "INCORRECT INPUT\n I/O files set as default\n\n");
            }

            asm_info->input.name  = DefaultInput;
            asm_info->output.name = DefaultOutput;
        }

        else
        {
            asm_info->input.name  = argv[1];
            asm_info->output.name = DefaultOutput;
        }
    }

    else
    {
        asm_info->input.name  = argv[1];
        asm_info->output.name = argv[2];
    }

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode ReadCode (AsmInfo_t* asm_info)
{
    ASSERT(asm_info);

    //-------------------------------------------------------------------//

    size_t file_size = 0;

    VERIFY(GetFileSize(asm_info->input.ptr, &asm_info->input.size) != ASM_SUCCESS,
           return ASM_GET_FILE_SIZE_ERROR);

    asm_info->input.data = (char*) calloc(asm_info->input.size, sizeof(char));

    //-------------------------------------------------------------------//

    if (fread(asm_info->input.data, sizeof(char), asm_info->input.size, asm_info->input.ptr) != asm_info->input.size)
    {
        fprintf(stderr, "FILE READ ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_READ_ERROR;
    }

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode GetFileSize (FILE* const file, size_t* file_size)
{
    ASSERT(file);
    ASSERT(file_size);

    //-------------------------------------------------------------------//

    struct stat file_status = {};

    if (fstat(fileno(file), &file_status) < 0)
    {
        fprintf(stderr, "GET FILE SIZE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_GET_FILE_SIZE_ERROR;
    }

    *file_size = file_status.st_size;

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode BuildCode (AsmInfo_t* asm_info)
{
    ASSERT(asm_info);

    //-------------------------------------------------------------------//

    char* str = asm_info->input.data;
    char cmd[MaxLineSize];
    char arg[MaxLineSize];
    int nchars = 0;

    //-------------------------------------------------------------------//

    while (true)
    {
        if (str - asm_info->input.data >= asm_info->input.size)
        {
            break;
        }

        VERIFY(ParseLine(asm_info, &str, cmd, arg, &nchars) != ASM_SUCCESS,
               return ASM_PARSE_LINE_ERROR);
    }

    asm_info->code.len = asm_info->code.ip;

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode GetCurString(AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars)
{
    ASSERT(asm_info);
    ASSERT(str);
    ASSERT(*str);
    ASSERT(cmd)
    ASSERT(arg);
    ASSERT(nchars);

    sscanf(*str, "%[^\n]%n", *str, nchars);

    if (strchr(*str, ' '))
    {
        sscanf(*str, "%[^\n ] %[^;\n]", cmd, arg);
    }

    else
    {
        sscanf(*str, "%[^\n]", cmd);
    }

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode ParseLine (AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars)
{
    ASSERT(asm_info);
    ASSERT(str);
    ASSERT(*str);
    ASSERT(cmd)
    ASSERT(arg);
    ASSERT(nchars);

    //-------------------------------------------------------------------//

    VERIFY(GetCurString(asm_info, str, cmd, arg, nchars) != ASM_SUCCESS,
           return ASM_GET_CUR_STRING_ERROR);

    //-------------------------------------------------------------------//

    if (SkipIfEmpty(str, cmd, arg, nchars) == ASM_SKIPPED_LINE_STATUS)
        return ASM_SUCCESS;

    //-------------------------------------------------------------------//

    if (AddIfLabel(asm_info, str, cmd, arg, nchars) == ASM_ADDED_LABEL_STATUS)
        return ASM_SUCCESS;

    //-------------------------------------------------------------------//

    if (ParseIfCommand(asm_info, str, cmd, arg, nchars) == ASM_PARSED_COMMAND_STATUS)
        return ASM_SUCCESS;

    //-------------------------------------------------------------------//

    fprintf(stderr, "INVALID COOMMAND ERROR: \"%s\" in %s:%d:%s\n",
            cmd, __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return ASM_INVALID_COMMAND_ERROR;
}

//===================================================================//

AsmReturnCode SkipIfEmpty(char** str, char* cmd, char* arg, int* nchars)
{
    ASSERT(str);
    ASSERT(*str);
    ASSERT(cmd)
    ASSERT(arg);
    ASSERT(nchars);

    //-------------------------------------------------------------------//

    if (cmd[0] == '\0')
    {
        *arg = '\0';
        *str += 1;

        return ASM_SKIPPED_LINE_STATUS;
    }

    //-------------------------------------------------------------------//

    return ASM_IS_NOT_AN_EMPTY_LINE_STATUS;
}

//===================================================================//

AsmReturnCode AddIfLabel(AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars)
{
    ASSERT(asm_info);
    ASSERT(str);
    ASSERT(*str);
    ASSERT(cmd)
    ASSERT(arg);
    ASSERT(nchars);

    //-------------------------------------------------------------------//

    if (strchr(cmd, ':'))
    {
        VERIFY(AddLabel(asm_info, cmd) != ASM_SUCCESS,
               return ASM_ADD_LABEL_ERROR);

        *cmd = '\0';
        *arg = '\0';

        *str += *nchars + 1;

        return ASM_ADDED_LABEL_STATUS;
    }

    //-------------------------------------------------------------------//

    return ASM_IS_NOT_A_LABEL_STATUS;
}

//===================================================================//

AsmReturnCode ParseIfCommand(AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars)
{
    ASSERT(asm_info);
    ASSERT(str);
    ASSERT(*str);
    ASSERT(cmd)
    ASSERT(arg);
    ASSERT(nchars);

    //-------------------------------------------------------------------//

    for (int cmd_index = 0; cmd_index < CommandsAmount; cmd_index++)
    {
        Command_t command = CommandsTabel[cmd_index];

        if (strcmp(cmd, command.name) == 0)
        {
            asm_info->code.code[asm_info->code.ip] = command.code;

            if (command.parse_func)
            {
                VERIFY(command.parse_func(asm_info, arg) != ASM_SUCCESS,
                       return ASM_PARSE_FUNC_ERROR);
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            *str += *nchars + 1;

            return ASM_PARSED_COMMAND_STATUS;
        }
    }

    //-------------------------------------------------------------------//

    return ASM_IS_NOT_A_COMMAND_STATUS;
}

//===================================================================//

AsmReturnCode FixUpLabels (AsmInfo_t* asm_info)
{
    ASSERT(asm_info);

    //-------------------------------------------------------------------//

    for (int fix_up_index = 0; fix_up_index < asm_info->labels.fix_up_table.n_fix_ups; fix_up_index++)
    {
        int      insert_ip        = asm_info->labels.fix_up_table.fix_ups[fix_up_index].ip;
        Label_t* insert_label_ptr = nullptr;

        if (!asm_info->labels.fix_up_table.fix_ups->named)
        {
            insert_label_ptr = asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_ptr;
        }
        else
        {
            const char* fix_up_label_name = asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_name;

            for (int label_index = 0; label_index < asm_info->labels.len; label_index++)
            {
                if (strcmp(fix_up_label_name, asm_info->labels.labels[label_index].name) == 0)
                {
                    insert_label_ptr = &asm_info->labels.labels[label_index];

                    break;
                }
            }
        }

        if (insert_label_ptr)
        {
            asm_info->code.code[insert_ip] = insert_label_ptr->addr;
        }
        else
        {
            fprintf(stderr, "NOT INITIALIZED LABEL in %s:%d:%s\n",
                    __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_NOT_INITIALIZED_LABEL_ERROR;
        }
    }

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

RegCode GetRegCode (const char* str)
{
    ASSERT(str);

    //-------------------------------------------------------------------//

    for (int reg_index = 0; reg_index < RegistersAmount; reg_index++)
    {
        if (strcmp(str, RegistersTabel[reg_index].name) == 0)
        {
            return RegistersTabel[reg_index].code;
        }
    }

    fprintf(stderr, "INVALID REGISTER CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    //-------------------------------------------------------------------//

    return INVALID_REG_CODE;
}

//===================================================================//

AsmReturnCode ParsePushPopArg (AsmInfo_t* asm_info, char* arg)
{
    ASSERT(asm_info);
    ASSERT(arg);

    //-------------------------------------------------------------------//

    int  imc  = 0;
    char reg[MaxRegNameSize] = {};
    PushPopLine_t line = {};

    //-------------------------------------------------------------------//

    for (int i = 0; i < nLines; i++)
    {
        line = PushPopLines[i];

        if (ScanedSuccessfully(&line, arg, &imc, reg))
        {
            VERIFY(InsertPushPopArg(asm_info, imc, reg,
                                    line.need_imc,
                                    line.need_reg,
                                    line.need_ram),
                   return ASM_INSERT_PUSH_POP_ARG_ERROR);

            return ASM_SUCCESS;
        }
    }

    //-------------------------------------------------------------------//

    return ASM_PARSE_PUSH_POP_ARG_ERROR;
}

//===================================================================//

 AsmReturnCode InsertPushPopArg(AsmInfo_t*  asm_info, int imc, const char* reg,
                                int need_imc, int need_reg, int need_ram)
 {
     ASSERT(asm_info);
     ASSERT(reg);

    //-------------------------------------------------------------------//

     asm_info->code.code[++asm_info->code.ip] = need_imc * ImmerseConstCode +
                                                need_reg * RegisterCode     +
                                                need_ram * RamCode;

    //-------------------------------------------------------------------//

     if (need_imc)
     {
         asm_info->code.code[++asm_info->code.ip] = imc;
     }

     //-------------------------------------------------------------------//

     if (need_reg)
     {
         VERIFY((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE,
                return ASM_INVALID_REGISTER_CODE_ERROR);
     }

     //-------------------------------------------------------------------//

     return ASM_SUCCESS;
}

//===================================================================//

bool ScanedSuccessfully(PushPopLine_t* line, char* arg, int* imc, char* reg)
{
    ASSERT(line);
    ASSERT(arg);
    ASSERT(imc);
    ASSERT(reg);

    //-------------------------------------------------------------------//

    switch (line->scanf_order)
    {
        case IMC:     return (sscanf(arg, line->format_line, imc)      == 1);
        case REG:     return (sscanf(arg, line->format_line, reg)      == 1);
        case IMC_REG: return (sscanf(arg, line->format_line, imc, reg) == 2);
        case REG_IMC: return (sscanf(arg, line->format_line, reg, imc) == 2);
        default:      return false;
    }

    //-------------------------------------------------------------------//

    return false;
}

//===================================================================//

AsmReturnCode ParseDrawArg (AsmInfo_t* asm_info, char* arg)
{
    ASSERT(asm_info);
    ASSERT(arg);

    //-------------------------------------------------------------------//

    int mode   = 0;
    int nchars = 0;

    VERIFY(sscanf(arg, "%d%n", &mode, &nchars) != 1,
           return ASM_DRAW_ARGS_SCANF_ERROR);

    asm_info->code.code[++asm_info->code.ip] = mode;

    char* ram_ptr = strtok(arg + nchars,  Delim);
    char* length  = strtok(NULL, Delim);
    char* width   = strtok(NULL, Delim);

    VERIFY(!ram_ptr || !length || !width,
           return ASM_DRAW_ARGS_SCANF_ERROR);

    VERIFY(ParsePushPopArg(asm_info, ram_ptr) != ASM_SUCCESS,
           return ASM_DRAW_ARGS_SCANF_ERROR);

    VERIFY(ParsePushPopArg(asm_info, length) != ASM_SUCCESS,
           return ASM_DRAW_ARGS_SCANF_ERROR);

    VERIFY(ParsePushPopArg(asm_info, width) != ASM_SUCCESS,
           return ASM_DRAW_ARGS_SCANF_ERROR);

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode ParseLabelArg (AsmInfo_t* asm_info, char* const_arg)
{
    ASSERT(asm_info);

    //-------------------------------------------------------------------//

    const char* arg = (const char*) const_arg;

    //-------------------------------------------------------------------//

    int  imrc_addr = 0;
    int  label     = 0;
    char label_name[MaxLabelName] = {};

    //-------------------------------------------------------------------//

    if (!strchr(arg, ':'))
    {
        VERIFY(sscanf(arg, "%d", &imrc_addr) != 1,
               return ASM_INVALID_LABEL_ERROR);

        asm_info->code.code[++asm_info->code.ip] = imrc_addr;

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%d:", &label) == 1)
    {
        VERIFY(ParseNumLabel(asm_info, label) != ASM_SUCCESS,
               return ASM_PARSE_LABEL_ARG_ERROR);

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%s:", label_name) == 1)
    {
        VERIFY(ParseNamedLabel(asm_info, label_name) != ASM_SUCCESS,
               return ASM_PARSE_LABEL_ARG_ERROR);

        return ASM_SUCCESS;
    }

    //-------------------------------------------------------------------//

    fprintf(stderr, "PARSE NAMED LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);;

    return ASM_PARSE_LABEL_ARG_ERROR;
}

//===================================================================//

AsmReturnCode ParseNumLabel (AsmInfo_t* asm_info, int label)
{
    ASSERT(asm_info);
    ASSERT((label < 1) || (label > LabelsSize));

    //-------------------------------------------------------------------//

    asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[label - 1].addr;

    if (!asm_info->labels.labels[label - 1].inited)
    {
        size_t fix_up_index = asm_info->labels.fix_up_table.n_fix_ups++;

        asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_ptr = &(asm_info->labels.labels[label - 1]);
        asm_info->labels.fix_up_table.fix_ups[fix_up_index].ip        = asm_info->code.ip;
    }

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode ParseNamedLabel (AsmInfo_t* asm_info, const char* label_name)
{
    ASSERT(asm_info);
    ASSERT(label_name);

    //-------------------------------------------------------------------//

    for (int i = 0; i < LabelsSize; i++)
    {
        if (strcmp(label_name, asm_info->labels.labels[i].name) == 0)
        {
            asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[i].addr;

            return ASM_SUCCESS;
        }
    }

    asm_info->code.code[++asm_info->code.ip] = -1;

    size_t fix_up_index = asm_info->labels.fix_up_table.n_fix_ups++;

    memcpy(asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_name, label_name, MaxLabelName);

    asm_info->labels.fix_up_table.fix_ups[fix_up_index].ip = asm_info->code.ip;

    asm_info->labels.fix_up_table.fix_ups[fix_up_index].named = true;

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode AddLabel (AsmInfo_t* asm_info, const char* cmd)
{
    ASSERT(asm_info);
    ASSERT(cmd);

    //-------------------------------------------------------------------//

    int  label              = atoi(cmd);
    bool inited             = false;
    char name[MaxLabelName] = {};

    if (label == 0) // not a number
    {
        sscanf(cmd, "%s:", name);

        for (int i = 0; i < asm_info->labels.len; i++)
        {
            VERIFY(strcmp(name, asm_info->labels.labels[i].name) == 0,
                   return ASM_DOUBLE_LABEL_ERROR);

            if (!asm_info->labels.labels[i].inited)
            {
                memcpy(asm_info->labels.labels[i].name, name, MaxLabelName);
                asm_info->labels.labels[i].addr    = asm_info->code.ip;
                asm_info->labels.labels[i].inited  = inited = true;
                asm_info->labels.n_labels++;

                return ASM_SUCCESS;
            }
        }

        if (!inited)
        {
            size_t new_label_index = asm_info->labels.len;

            asm_info->labels.labels = (Label_t*) realloc(asm_info->labels.labels, asm_info->labels.len * 2 * sizeof(Label_t));
            asm_info->labels.len = asm_info->labels.len * 2;

            memcpy(asm_info->labels.labels[new_label_index].name, name, MaxLabelName);
            asm_info->labels.labels[new_label_index].addr    = asm_info->code.ip;
            asm_info->labels.labels[new_label_index].inited  = inited = true;
            asm_info->labels.n_labels++;

            return ASM_SUCCESS;
        }

        //-------------------------------------------------------------------//

        return ASM_SUCCESS;
    }

    VERIFY(label < 1, return ASM_INVALID_LABEL_ERROR);

    if (label > asm_info->labels.len)
    {
        asm_info->labels.labels = (Label_t*) realloc(asm_info->labels.labels, asm_info->labels.len * 2 * sizeof(Label_t));
        asm_info->labels.len = asm_info->labels.len * 2;
    }

    VERIFY(asm_info->labels.labels[label - 1].inited,
           return ASM_DOUBLE_LABEL_ERROR);

    asm_info->labels.labels[label - 1].addr   = asm_info->code.ip;
    asm_info->labels.labels[label - 1].inited = true;
    asm_info->labels.n_labels++;

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode WriteCode (AsmInfo_t* asm_info)
{
    ASSERT(asm_info);

    #ifdef PRINT_WRITTEN_CODE

    fprintf(stdout, "Code:\n");

    for (int i = 0; i < asm_info->code.len; i++)
    {
        fprintf(stdout, "[%d] = %d\n", i, asm_info->code.code[i]);
    }

    #endif

    asm_info->output.ptr = fopen(asm_info->output.name, "wb");

    VERIFY(!(asm_info->output.ptr),
           return ASM_FILE_OPEN_ERROR);

    fwrite(&asm_info->code.len, sizeof(asm_info->code.len), 1, asm_info->output.ptr);
    fwrite(asm_info->code.code, asm_info->code.elem_size, asm_info->code.len, asm_info->output.ptr);

    //-------------------------------------------------------------------//

    return ASM_SUCCESS;
}

//===================================================================//

AsmReturnCode CloseCode (AsmInfo_t* asm_info)
{
    ASSERT(asm_info);
    ASSERT(asm_info->input.ptr);
    ASSERT(asm_info->output.ptr);

    //-------------------------------------------------------------------//

    fclose(asm_info->input.ptr);

    //-------------------------------------------------------------------//

    fclose(asm_info->output.ptr);

    free(asm_info->input.data);

    return ASM_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
