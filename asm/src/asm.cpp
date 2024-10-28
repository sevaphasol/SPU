#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <getopt.h>

#include "asm.h"

//------------------------------------------------//

AsmReturnCode OpenCode (AsmInfo* asm_info, int argc, const char* argv[])
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (ParseArgv(asm_info, argc, argv) != ASM_SUCCESS)
    {
        fprintf(stderr, "PARSE ARGV ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_PARSE_ARGV_ERROR;
    }

    asm_info->input.ptr  = fopen(asm_info->input.name,  "r");

    if (!(asm_info->input.ptr))
    {
        fprintf(stderr, "FILE OPEN ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_OPEN_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseArgv (AsmInfo* asm_info, int argc, const char* argv[])
{
    if (!argv)
    {
        fprintf(stderr, "INVALID ARGV ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_ARGV_ERROR;
    }

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

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ReadCode (AsmInfo_t* asm_info)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    size_t file_size = 0;

    if (GetFileSize(asm_info->input.ptr, &asm_info->input.size) != ASM_SUCCESS)
    {
        fprintf(stderr, "GET FILE SIZE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_GET_FILE_SIZE_ERROR;
    }

    asm_info->input.data    = (char*)        calloc(asm_info->input.size, sizeof(char));

    asm_info->code.code     = (int*)         calloc(asm_info->input.size, sizeof(int));

    asm_info->labels.len    = LabelsSize;
    asm_info->labels.labels = (Label_t*)     calloc(asm_info->labels.len, sizeof(Label_t));

    asm_info->labels.fix_up_table.fix_ups \
                            = (FixUpElem_t*) calloc(asm_info->input.size, sizeof(FixUpElem_t));

    asm_info->code.len   = asm_info->input.size;

    if (fread(asm_info->input.data, sizeof(char), asm_info->input.size, asm_info->input.ptr) != asm_info->input.size)
    {
        fprintf(stderr, "FILE READ ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_READ_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode GetFileSize (FILE* const file, size_t* file_size)
{
    struct stat file_status = {};

    if (fstat(fileno(file), &file_status) < 0)
    {
        fprintf(stderr, "GET FILE SIZE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_GET_FILE_SIZE_ERROR;
    }

    *file_size = file_status.st_size;

    return ASM_SUCCESS;
}

//------------------------------------------------//

RegCode GetRegCode(const char* str)
{
    if (!str)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return INVALID_REG_CODE;
    }

    for (int reg_index = 0; reg_index < RegistersAmount; reg_index++)
    {
            if (strcmp(str, RegistersTabel[reg_index].name) == 0)
            {
                return RegistersTabel[reg_index].code;
            }
    }

    fprintf(stderr, "INVALID REGISTER CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return INVALID_REG_CODE;
}

//------------------------------------------------//

AsmReturnCode ParsePushPopArg (AsmInfo_t* asm_info, const char* arg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    int    imc  = 0;
    char   reg[MaxRegNameSize] = {};

    if (sscanf(arg, "[%d + %[ABCDXSBPSDI]]", &imc, reg) == 2)
    {
        if (ParseImcRegRam(asm_info, imc, reg) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE IMC REG RAM ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "[%[ABCDXSBPSDI] + %d]", reg, &imc) == 2)
    {
        if(ParseRegImcRam(asm_info, imc, reg) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE REG IMC RAM ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }


        return ASM_SUCCESS;
    }

    if (sscanf(arg, "[%d]", &imc) == 1)
    {
        if (ParseImcRam(asm_info, imc) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE IMC RAM ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "[%[ABCDXSBPSDI]]", reg) == 1)
    {
        if (ParseRegRam(asm_info, reg) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE REG RAM ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%[ABCDXSBPSDI]", reg) == 1)
    {
        if (ParseReg(asm_info, reg) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%d + %[ABCDXSBPSDI]", &imc, reg) == 2)
    {
        if (ParseImcReg(asm_info, imc, reg) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE IMC REG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%[ABCDXSBPSDI] + %d", reg, &imc) == 2)
    {
        if (ParseRegImc(asm_info, imc, reg) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE REG IMC ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%d", &imc) == 1)
    {
        if (ParseImc(asm_info, imc) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE IMC ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_PUSH_POP_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return ASM_PARSE_PUSH_POP_ARG_ERROR;
}

//------------------------------------------------//

AsmReturnCode ParseImcRegRam (AsmInfo_t* asm_info, int imc, const char* reg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!reg)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode + RegisterCode;
    asm_info->code.code[++asm_info->code.ip] = imc;

    if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_REGISTER_CODE_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseRegImcRam (AsmInfo_t* asm_info, int imc, const char* reg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!reg)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode + RegisterCode;

    if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_REGISTER_CODE_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = imc;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseImcRam (AsmInfo_t* asm_info, int imc)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode;
    asm_info->code.code[++asm_info->code.ip] = imc;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseRegRam (AsmInfo_t* asm_info, const char* reg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!reg)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = RamCode + RegisterCode;

    if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_REGISTER_CODE_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseReg (AsmInfo_t* asm_info, const char* reg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!reg)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = RegisterCode;

    if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_REGISTER_CODE_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseImcReg (AsmInfo_t* asm_info, int imc, const char* reg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!reg)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = ImmerseConstCode + RegisterCode;
    asm_info->code.code[++asm_info->code.ip] = imc;

    if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_REGISTER_CODE_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseRegImc (AsmInfo_t* asm_info, int imc, const char* reg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!reg)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = RegisterCode + ImmerseConstCode;

    if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
    {
        fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_REGISTER_CODE_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = imc;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseImc (AsmInfo_t* asm_info, int imc)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = ImmerseConstCode;
    asm_info->code.code[++asm_info->code.ip] = imc;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseLabelArg (AsmInfo_t* asm_info, const char* arg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    int  imrc_addr = 0;
    int  label     = 0;
    char label_name[MaxLabelName] = {};

    if (!strchr(arg, ':'))
    {
        if (sscanf(arg, "%d", &imrc_addr) != 1)
        {
            fprintf(stderr, "INVALID LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);;

            return ASM_INVALID_LABEL_ERROR;
        }

        asm_info->code.code[++asm_info->code.ip] = imrc_addr;

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%d:", &label) == 1)
    {
        if (ParseNumLabel(asm_info, label) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE NUM LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);;

            return ASM_PARSE_LABEL_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%s:", label_name) == 1)
    {
        if (ParseNamedLabel(asm_info, label_name) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE NAMED LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);;

            return ASM_PARSE_LABEL_ARG_ERROR;
        }

        return ASM_SUCCESS;
    }

    fprintf(stderr, "PARSE NAMED LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);;

    return ASM_PARSE_LABEL_ARG_ERROR;
}

//------------------------------------------------//

AsmReturnCode ParseNumLabel (AsmInfo_t* asm_info, int label)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if ((label < 1) || (label > LabelsSize))
    {
        fprintf(stderr, "INVALID LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_LABEL_ERROR;
    }

    asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[label - 1].addr;

    if (!asm_info->labels.labels[label - 1].inited)
    {
        size_t fix_up_index = asm_info->labels.fix_up_table.n_fix_ups++;

        asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_ptr = &(asm_info->labels.labels[label - 1]);
        asm_info->labels.fix_up_table.fix_ups[fix_up_index].ip        = asm_info->code.ip;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseNamedLabel (AsmInfo_t* asm_info, const char* label_name)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!label_name)
    {
        fprintf(stderr, "LABEL NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

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

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode AddLabel (AsmInfo* asm_info, const char* cmd)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    int  label              = atoi(cmd);
    bool inited             = false;
    char name[MaxLabelName] = {};

    if (label == 0) // not a number
    {
        sscanf(cmd, "%s:", name);

        for (int i = 0; i < asm_info->labels.len; i++)
        {
            if (strcmp(name, asm_info->labels.labels[i].name) == 0)
            {
                fprintf(stderr, "DOUBLE LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_DOUBLE_LABEL_ERROR;
            }

            if (asm_info->labels.labels[i].inited == false)
            {
                memcpy(asm_info->labels.labels[i].name, name, MaxLabelName);
                asm_info->labels.labels[i].addr    = asm_info->code.ip;
                asm_info->labels.labels[i].inited  = inited = true;
                asm_info->labels.n_labels++;

                return ASM_SUCCESS;
            }
        }

        if (inited == false)
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

        return ASM_SUCCESS;
    }

    if ((label < 1))
    {
        fprintf(stderr, "INVALID LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_LABEL_ERROR;
    }

    if (label > asm_info->labels.len)
    {
        asm_info->labels.labels = (Label_t*) realloc(asm_info->labels.labels, asm_info->labels.len * 2 * sizeof(Label_t));
        asm_info->labels.len = asm_info->labels.len * 2;
    }

    if (asm_info->labels.labels[label - 1].inited)
    {
        fprintf(stderr, "DOUBLE LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_DOUBLE_LABEL_ERROR;
    }

    asm_info->labels.labels[label - 1].addr   = asm_info->code.ip;
    asm_info->labels.labels[label - 1].inited = true;
    asm_info->labels.n_labels++;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode BuildCode (AsmInfo_t* asm_info)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    char* str = asm_info->input.data;
    char cmd[MaxLineSize];
    char arg[MaxLineSize];
    int nchars = 0;

    while (true)
    {
        if (str - asm_info->input.data >= asm_info->input.size)
        {
            break;
        }

        if (ParseLine(asm_info, &str, cmd, arg, &nchars) != ASM_SUCCESS)
        {
            fprintf(stderr, "PARSE LINE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_PARSE_LINE_ERROR;
        }
    }

    asm_info->code.len = asm_info->code.ip;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ParseLine (AsmInfo_t* asm_info, char** str, char* cmd, char* arg, int* nchars)
{
    if (!asm_info || !str || !*str || !cmd || !arg || !nchars)
    {
        fprintf(stderr, "NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_NULL_PTR_ERROR;
    }

    sscanf(*str, "%[^\n]%n", *str, nchars);

    if (strchr(*str, ' '))
    {
        sscanf(*str, "%[^\n ] %[^\n]", cmd, arg);
    }

    else
    {
        sscanf(*str, "%[^\n]", cmd);
    }

    for (int cmd_index = 0; cmd_index < CommandsAmount; cmd_index++)
    {
        Command_t command = CommandsTabel[cmd_index];

        if (strcmp(cmd, command.name) == 0)
        {
            switch (command.arg_type)
            {
                case NO_ARG:
                {
                    asm_info->code.code[asm_info->code.ip++] = command.code;

                    *cmd = '\0';
                    *arg = '\0';

                    *str += *nchars + 1;

                    return ASM_SUCCESS;
                }

                case PUSH_POP_ARG:
                {
                    asm_info->code.code[asm_info->code.ip] = command.code;

                    if (ParsePushPopArg(asm_info, arg) != ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return ASM_PARSE_PUSH_POP_ARG_ERROR;
                    }

                    asm_info->code.ip++;

                    *cmd = '\0';
                    *arg = '\0';

                    *str += *nchars + 1;

                    return ASM_SUCCESS;
                }

                case LABEL_ARG:
                {
                    asm_info->code.code[asm_info->code.ip] = command.code;

                    if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return ASM_PARSE_LABEL_ARG_ERROR;
                    }

                    asm_info->code.ip++;

                    *cmd = '\0';
                    *arg = '\0';

                    *str += *nchars + 1;

                    return ASM_SUCCESS;
                }

                case DRAW_ARG: //TODO ParseDrawArg
                {
                    asm_info->code.code[asm_info->code.ip++] = command.code;

                    int mode, ram_ptr, size = 0;

                    if (sscanf(arg, "%d", &mode) != 1)
                    {
                        fprintf(stderr, "PARSE DRAW ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return ASM_DRAW_ARGS_SCANF_ERROR;
                    }

                    asm_info->code.code[asm_info->code.ip] = mode;

                    if (ParsePushPopArg(asm_info, arg) != ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE DRAW ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return ASM_DRAW_ARGS_SCANF_ERROR;
                    }

                    if (ParsePushPopArg(asm_info, strchr(strchr(arg, ' ') + 1, ' ') + 1) != ASM_SUCCESS)
                    {
                        fprintf(stderr, "PARSE DRAW ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                        return ASM_DRAW_ARGS_SCANF_ERROR;
                    }

                    asm_info->code.ip++;

                    *cmd = '\0';
                    *arg = '\0';

                    *str += *nchars + 1;

                    return ASM_SUCCESS;
                }
            }
        }
    }

    if (strchr(cmd, ':'))
    {
        if (AddLabel(asm_info, cmd) != ASM_SUCCESS)
        {
            fprintf(stderr, "ADD LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_ADD_LABEL_ERROR;
        }

        *cmd = '\0';
        *arg = '\0';

        *str += *nchars + 1;

        return ASM_SUCCESS;
    }

    if (cmd[0] == '\0')
    {
        *arg = '\0';

        *str += 1;

        return ASM_SUCCESS;
    }

    fprintf(stderr, "INVALID COOMMAND ERROR: \"%s\" in %s:%d:%s\n", cmd, __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return ASM_INVALID_COMMAND_ERROR;
}

//------------------------------------------------//

AsmReturnCode FixUpLabels (AsmInfo* asm_info)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }
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
            fprintf(stderr, "NOT INITIALIZED LABEL in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_NOT_INITIALIZED_LABEL_ERROR;
        }
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode WriteCode(AsmInfo* asm_info)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    #ifdef PRINT_WRITTEN_CODE

    fprintf(stdout, "Code:\n");

    for (int i = 0; i < asm_info->code.len; i++)
    {
        fprintf(stdout, "[%d] = %d\n", i, asm_info->code.code[i]);
    }

    #endif

    asm_info->output.ptr = fopen(asm_info->output.name, "wb");

    if (!(asm_info->output.ptr))
    {
        fprintf(stderr, "FILE OPEN ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_OPEN_ERROR;
    }

    fwrite(&asm_info->code.len, sizeof(asm_info->code.len), 1, asm_info->output.ptr);
    fwrite(asm_info->code.code, asm_info->code.elem_size, asm_info->code.len, asm_info->output.ptr);

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode CloseCode(AsmInfo_t* asm_info)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (!asm_info->input.ptr)
    {
        fprintf(stderr, "ASM FILE CLOSE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_CLOSE_ERROR;
    }

    fclose(asm_info->input.ptr);

    if (!asm_info->output.ptr)
    {
        fprintf(stderr, "ASM FILE CLOSE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_CLOSE_ERROR;
    }

    fclose(asm_info->output.ptr);

    free(asm_info->input.data);

    return ASM_SUCCESS;
}

//------------------------------------------------//
