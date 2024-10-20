#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <ctype.h>

#include "asm.h"

AsmReturnCode AsmInfoValid(const AsmInfo* asm_info)
{
    if (!(asm_info && asm_info->code.code && asm_info->input.ptr && asm_info->output.ptr))
    {
        return ASM_INFO_INVALID;
    }

    return ASM_INFO_VALID;
}

AsmReturnCode OpenCode(AsmInfo* asm_info, int argc, const char* argv[])
{
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
        fprintf(stderr, "FILE DOESN'T EXIST\n");

        return FILE_OPEN_ERROR;
    }

    return NO_ERROR;
}

AsmReturnCode ReadCode(AsmInfo_t* asm_info)
{
    ASSERT(AsmInfoValid(asm_info));

    size_t file_size = 0;

    if (GetFileSize(asm_info->input.ptr, &asm_info->input.size) == GET_FILE_SIZE_ERROR)
    {
        return GET_FILE_SIZE_ERROR;
    }

    asm_info->input.data = (char*) calloc(asm_info->input.size, sizeof(char));

    if (fread(asm_info->input.data, sizeof(char), asm_info->input.size, asm_info->input.ptr) != asm_info->input.size)
    {
        return FILE_READ_ERROR;
    }

    if (GetNumOfStrings(&asm_info->input) == GET_NUM_OF_STRINGS_ERROR)
    {
        return GET_NUM_OF_STRINGS_ERROR;
    }

    return NO_ERROR;
}

AsmReturnCode GetFileSize(FILE* const file, size_t* file_size)
{
    struct stat file_status = {};

    if (fstat(fileno(file), &file_status) < 0)
    {
        return GET_FILE_SIZE_ERROR;
    }

    *file_size = file_status.st_size;

    return NO_ERROR;
}

AsmReturnCode GetNumOfStrings(Stream_t* stream)
{
    if (!stream)
    {
        return GET_NUM_OF_STRINGS_ERROR;
    }

    char* ptr = nullptr;
    char* str = stream->data;
    int   cnt = 0;

    while ((ptr = strchr(str, '\n')) != nullptr)
    {
        str = ptr + 1;
        cnt++;
    }

    stream->n_strings = cnt;

    return NO_ERROR;
}

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

    return INVALID_CODE;
}

AsmReturnCode ParsePushPopArg(AsmInfo_t* asm_info, const char* arg)
{
    int    imc  = 0;
    char reg[2] = {};

    if (sscanf(arg, "[%d + %[ABCDX]]", &imc, reg) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode + RegisterCode;
        asm_info->code.code[++asm_info->code.ip] = imc;
        asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg); //TODO check for INVALID_CODE

        return NO_ERROR;
    }

    if (sscanf(arg, "[%[ABCDX] + %d]", reg, &imc) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode + RegisterCode;
        asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg); //TODO check for INVALID_CODE
        asm_info->code.code[++asm_info->code.ip] = imc;

        return NO_ERROR;
    }

    if (sscanf(arg, "[%d]", &imc) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode + ImmerseConstCode;
        asm_info->code.code[++asm_info->code.ip] = imc;

        return NO_ERROR;
    }

    if (sscanf(arg, "[%[ABCDX]]", reg) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = RamCode;
        asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg); //TODO check for INVALID_CODE

        return NO_ERROR;
    }

    if (sscanf(arg, "%[ABCDX]", reg) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = RegisterCode;
        asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg); //TODO check for INVALID_CODE

        return NO_ERROR;
    }

    if (sscanf(arg, "%d + %[ABCDX]", &imc, reg) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] = ImmerseConstCode + RegisterCode;
        asm_info->code.code[++asm_info->code.ip] = imc,
        asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg); //TODO check for INVALID_CODE

        return NO_ERROR;
    }

    if (sscanf(arg, "%[ABCDX] + %d", reg, &imc) == 2)
    {
        asm_info->code.code[++asm_info->code.ip] =  RegisterCode + ImmerseConstCode;
        asm_info->code.code[++asm_info->code.ip] = GetRegCode(reg); //TODO check for INVALID_CODE
        asm_info->code.code[++asm_info->code.ip] = imc;

        return NO_ERROR;
    }

    if (sscanf(arg, "%d", &imc) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = ImmerseConstCode;
        asm_info->code.code[++asm_info->code.ip] = imc;

        return NO_ERROR;
    }

    return PARSE_ARG_ERROR;
}

AsmReturnCode ParseJumpArg (AsmInfo_t* asm_info, const char* arg)
{
    int imrc_addr = 0;
    int label     = 0;
    char label_name[MaxLabelName] = {};

    if (!strchr(arg, ':'))
    {
        if(sscanf(arg, "%d", &imrc_addr) != 1)
        {
            return INVALID_LABEL_ERROR;
        }

        asm_info->code.code[++asm_info->code.ip] = imrc_addr;

        return NO_ERROR;
    }

    if (sscanf(arg, "%d:", &label) == 1)
    {
        asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[label - 1].addr;

        return NO_ERROR;
    }

    if (sscanf(arg, "%s:", label_name) == 1) //TODO ERROR CHECK
    {
        for (int i = 0; i < LabelsSize; i++)
        {
            if (!asm_info->labels.labels[i].name)
            {
                continue;
            }

fprintf(stderr, "%d\n", __LINE__);
            if (strcmp(label_name, asm_info->labels.labels[i].name) == 0)
            {
                asm_info->code.code[++asm_info->code.ip] = asm_info->labels.labels[i].addr;

                return NO_ERROR;
            }
        }

        return NO_ERROR;
    }

    return PARSE_JMP_ARG_ERROR;
}

AsmReturnCode AddLable (AsmInfo* asm_info, const char* cmd) //TODO named labels
{
    int  label              = atoi(cmd);
    char name[MaxLabelName] = {};

    if (label == 0)
    {
        sscanf(cmd, "%s:", name);

        int i = 0;

        for ( ; i < asm_info->labels.len; i++)
        {
fprintf(stderr, "%d\n", __LINE__);
            if (!asm_info->labels.labels[i].name)
            {
                if (asm_info->labels.labels[i].addr == 0)
                {
                    memcpy((void*) asm_info->labels.labels[i].name, (void*) name, MaxLabelName);
                    asm_info->labels.labels[i].addr    = asm_info->code.ip + 1;
                    asm_info->labels.labels[i].inited  = true;
                    asm_info->labels.n_labels++;

                    return NO_ERROR;
                }

                continue;
            }

            if (strcmp(name, asm_info->labels.labels[i].name) == 0)
            {
                return DOUBLE_LABEL_ERROR;
            }
        }

        return NO_ERROR;
    }

    if ((label < 1) || (label > LabelsSize))
    {
        return INVALID_LABEL_ERROR;
    }

    if (asm_info->labels.labels[label - 1].inited)
    {
        return DOUBLE_LABEL_ERROR;
    }

    asm_info->labels.labels[label - 1].addr    = asm_info->code.ip + 1;
    asm_info->labels.labels[label - 1].inited = true;
    asm_info->labels.n_labels++;

    return NO_ERROR;
}

AsmReturnCode BuildCode (AsmInfo_t* asm_info)
{
    char* str = asm_info->input.data;
    char cmd[MaxLineSize];
    char arg[MaxLineSize];
    int nchars = 0;

    while (sscanf(str, "%[^\n]%n", str, &nchars) != -1)
    {
        if (strchr(str, ' '))
        {
            sscanf(str, "%[^ ] %[^\n]", cmd, arg);
        }

        else
        {
            sscanf(str, "%[^\n]", cmd);
        }

        if (strcmp(cmd, "hlt") == 0)
        {
            asm_info->code.code[asm_info->code.ip++] = HLT;

            break;
        }

        if (strcmp(cmd, "push") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = PUSH;

            ParsePushPopArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "pop") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = POP;

            ParsePushPopArg(asm_info, arg);

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
            AddLable(asm_info, cmd);

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jmp") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JMP;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "ja") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JA;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jb") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JB;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jae") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JAE;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jbe") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JBE;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "je") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JE;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "jne") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = JNE;

            ParseJumpArg(asm_info, arg);

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }

        if (strcmp(cmd, "draw") == 0)
        {
            asm_info->code.code[asm_info->code.ip] = DRAW;

            asm_info->code.ip++;

            *cmd = '\0';
            *arg = '\0';

            str += nchars + 1;

            continue;
        }
    }

    asm_info->code.len = asm_info->code.ip;

    return NO_ERROR;
}

AsmReturnCode WriteCode(const AsmInfo* asm_info)
{
    ASSERT(AsmInfoValid(asm_info));

    fprintf(stdout, "Code:\n");

    for (int i = 0; i < asm_info->code.len; i++)
    {
        fprintf(stdout, "[%d] = %d\n", i, asm_info->code.code[i]);
    }

    fwrite(&asm_info->code.len, sizeof(asm_info->code.len), 1, asm_info->output.ptr);
    fwrite(asm_info->code.code, asm_info->code.elem_size, asm_info->code.len, asm_info->output.ptr);

    return NO_ERROR;
}

AsmReturnCode CloseCode(AsmInfo_t* asm_info)
{
    fclose(asm_info->input.ptr);
    fclose(asm_info->output.ptr);

    free(asm_info->input.data);

    return NO_ERROR;
}
