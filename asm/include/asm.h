#include <stdio.h>
#include <assert.h>

#ifndef ASM_H__
#define ASM_H__

#define PRINT_WRITTEN_CODE

//———————————————————————————————————————————————————————————————————//

const int CommandsAmount  = 25;
const int LabelsSize      = 1024;
const int MaxLabelName    = 32;
const int MaxLineSize     = 64;
const int RegistersAmount = 8;
const int MaxRegNameSize  = 2;
const int nLines = 9;

//-------------------------------------------------------------------//

const int ImmerseConstCode = 1;
const int RegisterCode     = 2;
const int RamCode          = 4;

//-------------------------------------------------------------------//

const char* const DefaultInput  = "asm_files/test.asm";
const char* const DefaultOutput = "executable_files/test.bin";

//-------------------------------------------------------------------//

const char* const Delim         = " ";

//———————————————————————————————————————————————————————————————————//

enum AsmReturnCode
{
    ASM_SUCCESS                      = 0,
    ASM_OPEN_CODE_ERROR              = 1,
    ASM_FILE_OPEN_ERROR              = 2,
    ASM_READ_CODE_ERROR              = 3,
    ASM_INVALID_ARGV_ERROR           = 4,
    ASM_FILE_READ_ERROR              = 5,
    ASM_FILE_CLOSE_ERROR             = 6,
    ASM_INFO_NULL_PTR_ERROR          = 7,
    ASM_INFO_VALID                   = 8,
    ASM_INFO_INVALID                 = 9,
    ASM_GET_FILE_SIZE_ERROR          = 10,
    ASM_GET_NUM_OF_STRINGS_ERROR     = 11,
    ASM_MAKE_STRINGS_ERROR           = 12,
    ASM_PARSE_ARG_ERROR              = 13,
    ASM_INVALID_LABEL_ERROR          = 14,
    ASM_LABELS_OBERFLOW_ERROR        = 15,
    ASM_INVALID_LABEL_NAME_ERROR     = 16,
    ASM_DOUBLE_LABEL_ERROR           = 17,
    ASM_ADD_LABEL_ERROR              = 18,
    ASM_NOT_INITIALIZED_LABEL_ERROR  = 19,
    ASM_INVALID_REGISTER_CODE_ERROR  = 20,
    ASM_PARSE_PUSH_POP_ARG_ERROR     = 21,
    ASM_PARSE_LABEL_ARG_ERROR        = 22,
    ASM_DRAW_SIZE_ERROR              = 23,
    ASM_INVALID_COMMAND_ERROR        = 24,
    ASM_DRAW_ARGS_SCANF_ERROR        = 25,
    ASM_PARSE_ARGV_ERROR             = 26,
    ASM_NULL_PTR_ERROR               = 27,
    ASM_PARSE_LINE_ERROR             = 28,
    ASM_PARSE_FUNC_ERROR             = 29,
    ASM_GET_CUR_STRING_ERROR         = 30,
    ASM_INSERT_PUSH_POP_ARG_ERROR    = 31,
    ASM_SKIPPED_LINE_STATUS          = 32,
    ASM_IS_NOT_AN_EMPTY_LINE_STATUS  = 33,
    ASM_ADDED_LABEL_STATUS           = 34,
    ASM_IS_NOT_A_LABEL_STATUS        = 35,
    ASM_PARSED_COMMAND_STATUS        = 36,
    ASM_IS_NOT_A_COMMAND_STATUS      = 37,
};

//-------------------------------------------------------------------//

enum ScanfOrder
{
    IMC,
    REG,
    IMC_REG,
    REG_IMC,
};

//-------------------------------------------------------------------//

enum CmdCode
{
    HLT   = 0,
    PUSH  = 1,
    POP   = 2,
    ADD   = 3,
    SUB   = 4,
    MUL   = 5,
    DIV   = 6,
    SQRT  = 7,
    SIN   = 8,
    COS   = 9,
    IN    = 10,
    OUT   = 11,
    DUMP  = 12,
    JMP   = 13,
    JA    = 14,
    JB    = 15,
    JAE   = 16,
    JBE   = 17,
    JE    = 18,
    JNE   = 19,
    DRAW  = 20,
    CALL  = 21,
    RET   = 22,
    CLEAR = 23,
    SLEEP = 24,
};

//-------------------------------------------------------------------//

enum ArgType
{
    NO_ARG       = 0,
    PUSH_POP_ARG = 1,
    LABEL_ARG    = 2,
    DRAW_ARG     = 3,
};

//-------------------------------------------------------------------//

enum RegCode
{
    INVALID_REG_CODE = -1,
    AX               = 1,
    BX               = 2,
    CX               = 3,
    DX               = 4,
    SP               = 5,
    BP               = 6,
    SI               = 7,
    DI               = 8,
};

//-------------------------------------------------------------------//

struct Stream_t
{
    const char* name;
    FILE*       ptr;
    size_t      size;
    size_t      n_strings;
    char*       data;
};

//-------------------------------------------------------------------//

struct Code_t
{
    size_t len;
    size_t elem_size;
    int    ip;
    int*   code;
};

//-------------------------------------------------------------------//

struct Label_t
{
    bool inited;
    char name[MaxLabelName];
    int  addr;
};

//-------------------------------------------------------------------//

struct FixUpElem_t
{
    bool named;
    int  ip;
    char label_name[MaxLabelName];
    Label_t* label_ptr;
};

//-------------------------------------------------------------------//

struct FixUpTable_t
{
    size_t n_fix_ups;
    FixUpElem_t* fix_ups;
};

//-------------------------------------------------------------------//

struct Labels_t
{
    size_t    len;
    size_t    elem_size;
    size_t    n_labels;
    Label_t*  labels;
    FixUpTable_t fix_up_table;
};

//-------------------------------------------------------------------//

struct AsmInfo_t
{
    Stream_t  input;
    Stream_t  output;
    Code_t    code;
    Labels_t  labels;
};

//-------------------------------------------------------------------//

struct Command_t
{
    const char    name[MaxLineSize];
    CmdCode       code;
    AsmReturnCode (*parse_func)(AsmInfo_t*, char*);
};

//-------------------------------------------------------------------//

struct Register_t
{
    const char name[MaxRegNameSize + 1];
    RegCode    code;
};

//-------------------------------------------------------------------//

struct PushPopLine_t
{
    const char* format_line;
    ScanfOrder  scanf_order;
    int         need_imc;
    int         need_reg;
    int         need_ram;
};

//-------------------------------------------------------------------//

AsmReturnCode ParsePushPopArg   (AsmInfo_t* asm_info, char* arg);
AsmReturnCode ParseLabelArg     (AsmInfo_t* asm_info, char* arg);
AsmReturnCode ParseDrawArg      (AsmInfo_t* asm_info, char* arg);

//———————————————————————————————————————————————————————————————————//

const Command_t CommandsTabel[] = {{.name = "hlt",   .code = HLT,   .parse_func = nullptr},
                                   {.name = "push",  .code = PUSH,  .parse_func = &ParsePushPopArg},
                                   {.name = "pop",   .code = POP,   .parse_func = &ParsePushPopArg},
                                   {.name = "add",   .code = ADD,   .parse_func = nullptr},
                                   {.name = "sub",   .code = SUB,   .parse_func = nullptr},
                                   {.name = "mul",   .code = MUL,   .parse_func = nullptr},
                                   {.name = "div",   .code = DIV,   .parse_func = nullptr},
                                   {.name = "sqrt",  .code = SQRT,  .parse_func = nullptr},
                                   {.name = "sin",   .code = SIN,   .parse_func = nullptr},
                                   {.name = "cos",   .code = COS,   .parse_func = nullptr},
                                   {.name = "in",    .code = IN,    .parse_func = nullptr},
                                   {.name = "out",   .code = OUT,   .parse_func = nullptr},
                                   {.name = "dump",  .code = DUMP,  .parse_func = nullptr},
                                   {.name = "jmp",   .code = JMP,   .parse_func = &ParseLabelArg},
                                   {.name = "ja",    .code = JA,    .parse_func = &ParseLabelArg},
                                   {.name = "jb",    .code = JB,    .parse_func = &ParseLabelArg},
                                   {.name = "jae",   .code = JAE,   .parse_func = &ParseLabelArg},
                                   {.name = "jbe",   .code = JBE,   .parse_func = &ParseLabelArg},
                                   {.name = "je",    .code = JE,    .parse_func = &ParseLabelArg},
                                   {.name = "jne",   .code = JNE,   .parse_func = &ParseLabelArg},
                                   {.name = "draw",  .code = DRAW,  .parse_func = &ParseDrawArg},
                                   {.name = "call",  .code = CALL,  .parse_func = &ParseLabelArg},
                                   {.name = "ret",   .code = RET,   .parse_func = nullptr},
                                   {.name = "clear", .code = CLEAR, .parse_func = nullptr},
                                   {.name = "sleep", .code = SLEEP, .parse_func = nullptr}};

//-------------------------------------------------------------------//

const Register_t RegistersTabel[] = {{.name = "AX", .code = AX},
                                     {.name = "BX", .code = BX},
                                     {.name = "CX", .code = CX},
                                     {.name = "DX", .code = DX},
                                     {.name = "SP", .code = SP},
                                     {.name = "BP", .code = BP},
                                     {.name = "SI", .code = SI},
                                     {.name = "DI", .code = DI}};

//-------------------------------------------------------------------//

const PushPopLine_t PushPopLines[nLines] = \
{{.format_line = "[%d + %[ABCDXSBPSDI]]", .scanf_order = IMC_REG, .need_imc = 1, .need_reg = 1, .need_ram = 1},
 {.format_line = "[%[ABCDXSBPSDI] + %d]", .scanf_order = REG_IMC, .need_imc = 1, .need_reg = 1, .need_ram = 1},
 {.format_line = "[%[ABCDXSBPSDI]]",      .scanf_order = REG,     .need_imc = 0, .need_reg = 1, .need_ram = 1},
 {.format_line = "[%d]",                  .scanf_order = IMC,     .need_imc = 1, .need_reg = 0, .need_ram = 1},
 {.format_line = "%[ABCDXSBPSDI] + %d",   .scanf_order = REG_IMC, .need_imc = 1, .need_reg = 1, .need_ram = 0},
 {.format_line = "%d + %[ABCDXSBPSDI]",   .scanf_order = IMC_REG, .need_imc = 1, .need_reg = 1, .need_ram = 0},
 {.format_line = "%[ABCDXSBPSDI]",        .scanf_order = REG,     .need_imc = 0, .need_reg = 1, .need_ram = 0},
 {.format_line = "%d",                    .scanf_order = IMC,     .need_imc = 1, .need_reg = 0, .need_ram = 0}};


//———————————————————————————————————————————————————————————————————//

AsmReturnCode AsmCtor     (AsmInfo_t* asm_info, int argc, const char* argv[]);
AsmReturnCode BuildCode   (AsmInfo_t* asm_info);
AsmReturnCode FixUpLabels (AsmInfo_t* asm_info);
AsmReturnCode WriteCode   (AsmInfo_t* asm_info);
AsmReturnCode CloseCode   (AsmInfo_t* asm_info);

//———————————————————————————————————————————————————————————————————//

#endif // ASM_H__
