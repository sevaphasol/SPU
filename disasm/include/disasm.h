#ifndef DIS_ASM_H__
#define DIS_ASM_H__

#include <stdio.h>
#include <assert.h>

//———————————————————————————————————————————————————————————————————//

#define INSERT(str) str

// #define PRINT_READ_CODE

//———————————————————————————————————————————————————————————————————//

const int CommandsAmount  = 25;
const int LabelsSize      = 32;
const int MaxLabelName    = 32;
const int MaxLineSize     = 64;
const int RegistersAmount = 8;
const int MaxRegNameSize  = 2;

//-------------------------------------------------------------------//

const int ImmerseConstCode = 1;
const int RegisterCode     = 2;
const int RamCode          = 4;
const int nLines           = 6;

//-------------------------------------------------------------------//

const char* const DefaultInput  = "executable_files/test.bin";
const char* const DefaultOutput = "disasmed_files/test.asm";

//———————————————————————————————————————————————————————————————————//

enum DisAsmReturnCode
{
    DIS_ASM_SUCCESS                     = 0,
    DIS_ASM_FILE_OPEN_ERROR             = 1,
    DIS_ASM_INVALID_ARGV_ERROR          = 2,
    DIS_ASM_FILE_READ_ERROR             = 3,
    DIS_ASM_FILE_CLOSE_ERROR            = 4,
    DIS_ASM_INFO_NULL_PTR_ERROR         = 5,
    DIS_ASM_INFO_VALID                  = 6,
    DIS_ASM_INFO_INVALID                = 7,
    DIS_ASM_GET_FILE_SIZE_ERROR         = 8,
    DIS_ASM_GET_NUM_OF_STRINGS_ERROR    = 9,
    DIS_ASM_MAKE_STRINGS_ERROR          = 10,
    DIS_ASM_PARSE_ARG_ERROR             = 11,
    DIS_ASM_INVALID_LABEL_ERROR         = 12,
    DIS_ASM_INVALID_LABEL_NAME_ERROR    = 13,
    DIS_ASM_DOUBLE_LABEL_ERROR          = 14,
    DIS_ASM_ADD_LABEL_ERROR             = 15,
    DIS_ASM_NOT_INITIALIZED_LABEL_ERROR = 16,
    DIS_ASM_INVALID_REGISTER_CODE_ERROR = 17,
    DIS_ASM_PARSE_PUSH_POP_ARG_ERROR    = 18,
    DIS_ASM_PARSE_LABEL_ARG_ERROR       = 19,
    DIS_ASM_DRAW_SIZE_ERROR             = 20,
    DIS_ASM_INVALID_COMMAND_ERROR       = 21,
    DIS_ASM_DRAW_ARGS_SCANF_ERROR       = 22,
    DIS_ASM_PARSE_ARGV_ERROR            = 23,
    DIS_ASM_NULL_PTR_ERROR              = 24,
    DIS_ASM_PARSE_LINE_ERROR            = 25,
    DIS_ASM_INVALID_INSTRUCTION_ERROR   = 26,
    DIS_ASM_PARSE_INSTRUCTION           = 27,
    DIS_ASM_PARSE_DRAW_ARG_ERROR        = 28,
    DIS_ASM_OPEN_CODE_ERROR             = 29,
    DIS_ASM_READ_CODE_ERROR             = 30,
    DIS_ASM_PARSE_FUNC_ERROR            = 31,
    DIS_ASM_PRINT_PUSH_POP_ARG_ERROR    = 32,
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

//———————————————————————————————————————————————————————————————————//

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

struct DisAsmInfo_t
{
    Stream_t  input;
    Stream_t  output;
    Code_t    code;
    bool      asm_option;
};

//-------------------------------------------------------------------//

struct Command_t
{
    const char       name[MaxLineSize];
    CmdCode          code;
    DisAsmReturnCode (*parse_func)(DisAsmInfo_t*);
};

//-------------------------------------------------------------------//

struct Register_t
{
    const char name[MaxRegNameSize + 1];
    RegCode    code;
};

//———————————————————————————————————————————————————————————————————//

DisAsmReturnCode ParsePushPopArg   (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseLabelArg     (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseDrawArg      (DisAsmInfo_t* dis_asm_info);

//-------------------------------------------------------------------//

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

//———————————————————————————————————————————————————————————————————//

enum PrintfOrder
{
    IMC     = ImmerseConstCode,
    REG     = RegisterCode,
    IMC_REG = ImmerseConstCode + RegisterCode,
};

//-------------------------------------------------------------------//

struct PushPopLine_t
{
    const char* format_line;
    int         arg_type;
    PrintfOrder printf_order;
    int         need_imc;
    int         need_reg;
    int         need_ram;
};

//-------------------------------------------------------------------//

const PushPopLine_t PushPopLines[nLines] = \
{{.format_line   = "[%d + %s]", .arg_type = ImmerseConstCode + RegisterCode + RamCode, \
  .printf_order  = IMC_REG, .need_imc = 1, .need_reg = 1, .need_ram = 1},

 {.format_line   = "[%s]",      .arg_type =                    RegisterCode + RamCode, \
   .printf_order = REG,     .need_imc = 0, .need_reg = 1, .need_ram = 1},

 {.format_line   = "[%d]",      .arg_type = ImmerseConstCode  +    RamCode           , \
   .printf_order = IMC,     .need_imc = 1, .need_reg = 0, .need_ram = 1},

 {.format_line   = "%d + %s",   .arg_type = ImmerseConstCode + RegisterCode          , \
   .printf_order = IMC_REG, .need_imc = 1, .need_reg = 1, .need_ram = 0},

 {.format_line   = "%s",        .arg_type =                    RegisterCode          , \
   .printf_order = REG,     .need_imc = 0, .need_reg = 1, .need_ram = 0},

 {.format_line   = "%d",        .arg_type = ImmerseConstCode                         , \
   .printf_order = IMC,     .need_imc = 1, .need_reg = 0, .need_ram = 0}};

//———————————————————————————————————————————————————————————————————//

const Register_t RegistersTabel[] = {{.name = "AX", .code = AX},
                                     {.name = "BX", .code = BX},
                                     {.name = "CX", .code = CX},
                                     {.name = "DX", .code = DX},
                                     {.name = "SP", .code = SP},
                                     {.name = "BP", .code = BP},
                                     {.name = "SI", .code = SI},
                                     {.name = "DI", .code = DI}};

//———————————————————————————————————————————————————————————————————//

DisAsmReturnCode DisAsmCtor (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[]);
DisAsmReturnCode BuildCode  (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode CloseCode  (DisAsmInfo_t*dis_asm_info);

#endif // DIS_ASM_H__
