#include <stdio.h>
#include <assert.h>

#ifndef DIS_ASM_H__
#define DIS_ASM_H__

#define DIS_ASM_INFO_INIT .input      = {.name = nullptr, .ptr = nullptr, .size = 0, .data  = nullptr}, \
                          .output     = {.name = nullptr, .ptr = nullptr, .size = 0, .data  = nullptr}, \
                          .code       = {.len = 0, .elem_size = sizeof(int), .ip = 0, .code = nullptr}, \
                          .asm_option = false                                                           \

// #define PRINT_READ_CODE

const int CommandsAmount  = 25;
const int LabelsSize      = 32;
const int MaxLabelName    = 32;
const int MaxLineSize     = 64;
const int RegistersAmount = 8;
const int MaxRegNameSize  = 2;

const int ImmerseConstCode = 1;
const int RegisterCode     = 2;
const int RamCode          = 4;

const char* const DefaultInput  = "executable_files/test.bin";
const char* const DefaultOutput = "disasmed_files/test.asm";

typedef enum DisAsmReturnCodes
{
    DIS_ASM_SUCCESS,
    DIS_ASM_FILE_OPEN_ERROR,
    DIS_ASM_INVALID_ARGV_ERROR,
    DIS_ASM_FILE_READ_ERROR,
    DIS_ASM_FILE_CLOSE_ERROR,
    DIS_ASM_INFO_NULL_PTR_ERROR,
    DIS_ASM_INFO_VALID,
    DIS_ASM_INFO_INVALID,
    DIS_ASM_GET_FILE_SIZE_ERROR,
    DIS_ASM_GET_NUM_OF_STRINGS_ERROR,
    DIS_ASM_MAKE_STRINGS_ERROR,
    DIS_ASM_PARSE_ARG_ERROR,
    DIS_ASM_INVALID_LABEL_ERROR,
    DIS_ASM_INVALID_LABEL_NAME_ERROR,
    DIS_ASM_DOUBLE_LABEL_ERROR,
    DIS_ASM_ADD_LABEL_ERROR,
    DIS_ASM_NOT_INITIALIZED_LABEL_ERROR,
    DIS_ASM_INVALID_REGISTER_CODE_ERROR,
    DIS_ASM_PARSE_PUSH_POP_ARG_ERROR,
    DIS_ASM_PARSE_LABEL_ARG_ERROR,
    DIS_ASM_DRAW_SIZE_ERROR,
    DIS_ASM_INVALID_COMMAND_ERROR,
    DIS_ASM_DRAW_ARGS_SCANF_ERROR,
    DIS_ASM_PARSE_ARGV_ERROR,
    DIS_ASM_NULL_PTR_ERROR,
    DIS_ASM_PARSE_LINE_ERROR,
    DIS_ASM_INVALID_INSTRUCTION_ERROR,
    DIS_ASM_PARSE_INSTRUCTION,
    DIS_ASM_PARSE_DRAW_ARG_ERROR,
} DisAsmReturnCode;

typedef enum CmdCodes
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
} CmdCode;

typedef enum ArgTypeCodes
{
    NO_ARG       = 0,
    PUSH_POP_ARG = 1,
    LABEL_ARG    = 2,
    DRAW_ARG     = 3,
} ArgType;

typedef enum RegCodes
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
} RegCode;

typedef struct Stream
{
    const char* name;
    FILE*       ptr;
    size_t      size;
    size_t      n_strings;
    char*       data;
} Stream_t;

typedef struct Code
{
    size_t len;
    size_t elem_size;
    int    ip;
    int*   code;
} Code_t;

typedef struct DisAsmInfo
{
    Stream_t  input;
    Stream_t  output;
    Code_t    code;
    bool      asm_option;
} DisAsmInfo_t;

typedef struct Command
{
    const char name[MaxLineSize];
    CmdCode    code;
    ArgType    arg_type;
} Command_t;


typedef struct Register
{
    const char name[MaxRegNameSize + 1];
    RegCode    code;
} Register_t;

const Command_t CommandsTabel[]   = {{.name = "hlt",    .code = HLT,  .arg_type = NO_ARG},
                                     {.name = "push",   .code = PUSH, .arg_type = PUSH_POP_ARG},
                                     {.name = "pop",    .code = POP,  .arg_type = PUSH_POP_ARG},
                                     {.name = "add",    .code = ADD,  .arg_type = NO_ARG},
                                     {.name = "sub",    .code = SUB,  .arg_type = NO_ARG},
                                     {.name = "mul",    .code = MUL,  .arg_type = NO_ARG},
                                     {.name = "div",    .code = DIV,  .arg_type = NO_ARG},
                                     {.name = "sqrt",   .code = SQRT, .arg_type = NO_ARG},
                                     {.name = "sin",    .code = SIN,  .arg_type = NO_ARG},
                                     {.name = "cos",    .code = COS,  .arg_type = NO_ARG},
                                     {.name = "in",     .code = IN,   .arg_type = NO_ARG},
                                     {.name = "out",    .code = OUT,  .arg_type = NO_ARG},
                                     {.name = "dump",   .code = DUMP, .arg_type = NO_ARG},
                                     {.name = "jmp",    .code = JMP,  .arg_type = LABEL_ARG},
                                     {.name = "ja",     .code = JA,   .arg_type = LABEL_ARG},
                                     {.name = "jb",     .code = JB,   .arg_type = LABEL_ARG},
                                     {.name = "jae",    .code = JAE,  .arg_type = LABEL_ARG},
                                     {.name = "jbe",    .code = JBE,  .arg_type = LABEL_ARG},
                                     {.name = "jne",    .code = JNE,  .arg_type = LABEL_ARG},
                                     {.name = "je",     .code = JE,   .arg_type = LABEL_ARG},
                                     {.name = "draw",   .code = DRAW, .arg_type = DRAW_ARG},
                                     {.name = "call",   .code = CALL, .arg_type = LABEL_ARG},
                                     {.name = "ret",    .code = RET,  .arg_type = NO_ARG},
                                     {.name = "clear",  .code = CLEAR,  .arg_type = NO_ARG},
                                     {.name = "sleep",  .code = SLEEP,  .arg_type = NO_ARG}};

const Register_t RegistersTabel[] = {{.name = "AX", .code = AX},
                                     {.name = "BX", .code = BX},
                                     {.name = "CX", .code = CX},
                                     {.name = "DX", .code = DX},
                                     {.name = "SP", .code = SP},
                                     {.name = "BP", .code = BP},
                                     {.name = "SI", .code = SI},
                                     {.name = "DI", .code = DI}};

DisAsmReturnCode OpenCode          (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[]);
DisAsmReturnCode ParseArgv         (DisAsmInfo_t* dis_asm_info, int argc, const char* argv[]);

DisAsmReturnCode ReadCode          (DisAsmInfo_t* dis_asm_info);

DisAsmReturnCode BuildCode         (DisAsmInfo_t* dis_asm_info);

DisAsmReturnCode ParseLine         (DisAsmInfo_t* dis_asm_info);

DisAsmReturnCode ParsePushPopArg   (DisAsmInfo_t* dis_asm_info);

DisAsmReturnCode ParseImc          (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseReg          (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseImcReg       (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseImcRam       (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseRegRam       (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseImcRegRam    (DisAsmInfo_t* dis_asm_info);

DisAsmReturnCode GetRegName (int reg_code, char reg_name[MaxRegNameSize]);

DisAsmReturnCode ParseLabelArg     (DisAsmInfo_t* dis_asm_info);
DisAsmReturnCode ParseDrawArg      (DisAsmInfo_t* dis_asm_info);


DisAsmReturnCode CloseCode         (DisAsmInfo_t*dis_asm_info);

#endif // DIS_ASM_H__
