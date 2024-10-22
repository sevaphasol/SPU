#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>
#include <getopt.h>

#include "asm.h"

//------------------------------------------------//

AsmReturnCode OpenCode(AsmInfo* asm_info, int argc, char* argv[])
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    if (argc != 3)
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
        asm_info->output.name = argv[2];
    }

    asm_info->input.ptr  = fopen(asm_info->input.name,  "r");
    asm_info->output.ptr = fopen(asm_info->output.name, "wb");

    if (!(asm_info->input.ptr) || !(asm_info->output.ptr))
    {
        fprintf(stderr, "FILE OPEN ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_OPEN_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode ReadCode(AsmInfo_t* asm_info)
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

    asm_info->input.data = (char*) calloc(asm_info->input.size, sizeof(char));

    if (fread(asm_info->input.data, sizeof(char), asm_info->input.size, asm_info->input.ptr) != asm_info->input.size)
    {
        fprintf(stderr, "FILE READ ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_FILE_READ_ERROR;
    }

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode GetFileSize(FILE* const file, size_t* file_size)
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

RegCode GetRegCode(const char* reg_name)
{
    if (strcmp(reg_name, "AX") == 0)
    {
        return AX;
    }

    if (strcmp(reg_name, "BX") == 0)
    {
        return BX;
    }

    if (strcmp(reg_name, "CX") == 0)
    {
        return CX;
    }

    if (strcmp(reg_name, "DX") == 0)
    {
        return DX;
    }

    fprintf(stderr, "INVALID REGISTER CODE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

    return INVALID_REG_CODE;
}

//------------------------------------------------//

AsmReturnCode ParsePushPopArg(AsmInfo_t* asm_info, const char* arg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    int    imc  = 0;
    char reg[2] = {};

    if (sscanf(arg, "[%d + %[ABCDX]]", &imc, reg) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode + RegisterCode;
        asm_info->code.code[++asm_info->code.ip] = imc;

        if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
        {
            fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_REGISTER_CODE_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "[%[ABCDX] + %d]", reg, &imc) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode + RegisterCode;

        if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
        {
            fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_REGISTER_CODE_ERROR;
        }

        asm_info->code.code[++asm_info->code.ip] = imc;

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "[%d]", &imc) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode;
        asm_info->code.code[++asm_info->code.ip] = imc;

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "[%[ABCDX]]", reg) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + RegisterCode;

        if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
        {
            fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_REGISTER_CODE_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%[ABCDX]", reg) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = RegisterCode;

        if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
        {
            fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_REGISTER_CODE_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%d + %[ABCDX]", &imc, reg) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = ImmerseConstCode + RegisterCode;
        asm_info->code.code[++asm_info->code.ip] = imc;

        if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
        {
            fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_REGISTER_CODE_ERROR;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%[ABCDX] + %d", reg, &imc) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = RegisterCode + ImmerseConstCode;

        if ((asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg)) == INVALID_REG_CODE)
        {
            fprintf(stderr, "INVALID REGISTER CODE in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_REGISTER_CODE_ERROR;
        }

        asm_info->code.code[++asm_info->code.ip] = imc;

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%d", &imc) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = ImmerseConstCode;
        asm_info->code.code[++asm_info->code.ip] = imc;

        return ASM_SUCCESS;
    }

    return ASM_PARSE_ARG_ERROR;
}

//------------------------------------------------//

AsmReturnCode ParseLabelArg (AsmInfo_t* asm_info, const char* arg)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    int imrc_addr = 0;
    int label     = 0;
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
        if ((label < 1) || (label > LabelsSize))
        {
            fprintf(stderr, "INVALID LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

            return ASM_INVALID_LABEL_ERROR;
        }

        asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[label - 1].addr;

        if (!asm_info->labels.labels[label - 1].inited)
        {
            size_t fix_up_index = asm_info->labels.fix_up_table.n_labels++;

            asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_ptr = &(asm_info->labels.labels[label - 1]);
            asm_info->labels.fix_up_table.fix_ups[fix_up_index].ip        = asm_info->code.ip;
        }

        return ASM_SUCCESS;
    }

    if (sscanf(arg, "%s:", label_name) == 1)
    {
        for (int i = 0; i < LabelsSize; i++)
        {
            if (strcmp(label_name, asm_info->labels.labels[i].name) == 0)
            {
                asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[i].addr;

                return ASM_SUCCESS;
            }
        }

        asm_info->code.code[++asm_info->code.ip] = -1;

        size_t fix_up_index = asm_info->labels.fix_up_table.n_labels++;

        memcpy((void*) asm_info->labels.fix_up_table.fix_ups[fix_up_index].label_name,
               (void*) label_name, MaxLabelName);

        asm_info->labels.fix_up_table.fix_ups[fix_up_index].ip = asm_info->code.ip;

        asm_info->labels.fix_up_table.fix_ups[fix_up_index].named = true;

        return ASM_SUCCESS;
    }

    fprintf(stderr, "PARSE LABLE ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);;

    return ASM_PARSE_LABEL_ARG_ERROR;
}

//------------------------------------------------//

AsmReturnCode AddLable (AsmInfo* asm_info, const char* cmd)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    int  label              = atoi(cmd);
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
                memcpy((void*) asm_info->labels.labels[i].name, (void*) name, MaxLabelName);
                asm_info->labels.labels[i].addr    = asm_info->code.ip;
                asm_info->labels.labels[i].inited  = true;
                asm_info->labels.n_labels++;

                return ASM_SUCCESS;
            }
        }

        return ASM_SUCCESS;
    }

    if ((label < 1) || (label > LabelsSize))
    {
        fprintf(stderr, "INVALID LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INVALID_LABEL_ERROR;
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
        sscanf(str, "%[^\n]%n", str, &nchars);

        if (str - asm_info->input.data == asm_info->input.size)
        {
            break;
        }

        if (strchr(str, ' '))
        {
            sscanf(str, "%[^\n ] %[^\n]", cmd, arg);
        }

        else
        {
            sscanf(str, "%[^\n]", cmd);
        }

        if (cmd[0] == '\0')
        {
            *arg = '\0';

            str += 1;

            continue;
        }

        if (strcmp(cmd, "hlt") == 0)
        {
            asm_info->code.code[asm_info->code.ip++] = HLT;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "push") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = PUSH;

            if (ParsePushPopArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "pop") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = POP;

            if (ParsePushPopArg(asm_info, arg))
            {
                fprintf(stderr, "PARSE PUSH POP ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_PUSH_POP_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "add") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = ADD;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "sub") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = SUB;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "mul") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = MUL;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "div") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = DIV;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "sqrt") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = SQRT;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "sin") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = SIN;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "cos") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = COS;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "in") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = IN;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "out") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = OUT;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "dump") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = DUMP;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strchr(cmd, ':'))
        {
            if (AddLable(asm_info, cmd) != ASM_SUCCESS)
            {
                fprintf(stderr, "ADD LABEL ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_ADD_LABEL_ERROR;
            }

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jmp") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JMP;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "ja") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JA;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jb") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JB;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jae") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JAE;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jbe") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JBE;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "je") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JE;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jne") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JNE;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                fprintf(stderr, "PARSE LABEL ARG ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_PARSE_LABEL_ARG_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "draw") == 0)
        {
            asm_info->code.code[asm_info->code.ip++] = DRAW;

            int ram_ptr, size = 0;

            if (size < 0)
            {
                fprintf(stderr, "DRAW SIZE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

                return ASM_DRAW_SIZE_ERROR;
            }

            sscanf(arg, "%d %d", &ram_ptr, &size);

            asm_info->code.code[asm_info->code.ip++] = ram_ptr;
            asm_info->code.code[asm_info->code.ip]   = size;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "call") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = CALL;

            if (ParseLabelArg(asm_info, arg) != ASM_SUCCESS)
            {
                return ASM_ADD_LABEL_ERROR;
            }

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "ret") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = RET;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        fprintf(stderr, "INVALID COOMMAND ERROR: \"%s\" in %s:%d:%s\n", cmd, __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return INVALID_COMMAND_ERROR;
    }

    asm_info->code.len = asm_info->code.ip;

    return ASM_SUCCESS;
}

//------------------------------------------------//

AsmReturnCode FixUpLabes(AsmInfo* asm_info)
{
    if (!asm_info)
    {
        fprintf(stderr, "ASM INFO NULL PTR ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return ASM_INFO_NULL_PTR_ERROR;
    }

    for (size_t i = 0; i < asm_info->labels.fix_up_table.n_labels; i++)
    {
        int      insert_ip        = asm_info->labels.fix_up_table.fix_ups[i].ip;
        Label_t* insert_label_ptr = nullptr;

        if (!asm_info->labels.fix_up_table.fix_ups->named)
        {
            insert_label_ptr = asm_info->labels.fix_up_table.fix_ups[i].label_ptr;
        }
        else
        {
            const char* fix_up_label_name = asm_info->labels.fix_up_table.fix_ups[i].label_name;

            for (int label_index = 0; label_index < asm_info->labels.len; i++)
            {
                if (strcmp(fix_up_label_name, asm_info->labels.labels[i].name) == 0)
                {
                    insert_label_ptr = &asm_info->labels.labels[i];

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

AsmReturnCode WriteCode(const AsmInfo* asm_info)
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
