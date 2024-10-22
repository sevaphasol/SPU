#include <stdio.h>
#include <assert.h>

#ifndef ASM_H__
#define ASM_H__

#define ASSERT(validation_info) assert(validation_info != ASM_INFO_INVALID)

#define ASM_INFO_INIT .input        = {.name = nullptr, .ptr = nullptr, .size = 0, .data = nullptr},                     \
                      .output       = {.name = nullptr, .ptr = nullptr, .size = 0, .data = nullptr},                     \
                      .code         = {.len = 0, .elem_size = sizeof(int), .ip = 0, .code   = {0}},                      \
                      .labels       = {.len = LabelsSize, .elem_size = sizeof(int), .labels = {0}, .fix_up_table = {0}}  \

#define PRINT_WRITTEN_CODE

const int CodeArrSize   = 128;
const int LabelsSize    = 16;
const int MaxLabelName  = 64;
const int MaxLineSize   = 64;
const int Poison        = -1;

const int ImmerseConstCode = 1;
const int RegisterCode     = 2;
const int RamCode          = 4;

const char* const DefaultInput  = "asm_files/example_programm.asm";
const char* const DefaultOutput = "assembled_files/example_programm_code.bin";

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
    int    code[CodeArrSize];
} Code_t;

typedef struct Label
{
    bool inited;
    char name[MaxLabelName];
    int  addr;
} Label_t;

typedef struct FixUpElem
{
    bool named;
    int ip;
    char label_name[MaxLabelName];
    Label_t* label_ptr;
} FixUpElem_t;

typedef struct FixUpTable
{
    size_t n_labels;
    FixUpElem fix_ups[CodeArrSize];
} FixUpTable_t;

typedef struct Labels
{
    size_t    len;
    size_t    elem_size;
    size_t    n_labels;
    Label_t   labels[LabelsSize];
    FixUpTable_t fix_up_table;
} Labels_t;

typedef struct AsmInfo
{
    Stream_t  input;
    Stream_t  output;
    Code_t    code;
    Labels_t  labels;
} AsmInfo_t;

typedef enum AsmReturnCodes
{
    ASM_SUCCESS,
    ASM_FILE_OPEN_ERROR,
    ASM_FILE_READ_ERROR,
    ASM_FILE_CLOSE_ERROR,
    ASM_INFO_NULL_PTR_ERROR,
    ASM_INFO_VALID,
    ASM_INFO_INVALID,
    ASM_GET_FILE_SIZE_ERROR,
    ASM_GET_NUM_OF_STRINGS_ERROR,
    ASM_MAKE_STRINGS_ERROR,
    ASM_PARSE_ARG_ERROR,
    ASM_INVALID_LABEL_ERROR,
    ASM_INVALID_LABEL_NAME_ERROR,
    ASM_DOUBLE_LABEL_ERROR,
    ASM_ADD_LABEL_ERROR,
    ASM_NOT_INITIALIZED_LABEL_ERROR,
    ASM_INVALID_REGISTER_CODE_ERROR,
    ASM_PARSE_PUSH_POP_ARG_ERROR,
    ASM_PARSE_LABEL_ARG_ERROR,
    ASM_DRAW_SIZE_ERROR,
    INVALID_COMMAND_ERROR,
} AsmReturnCode;

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
} CmdCode;

typedef enum RegCodes
{
    INVALID_REG_CODE = -1,
    AX               = 1,
    BX               = 2,
    CX               = 3,
    DX               = 4,
} RegCode;

AsmReturnCode OpenCode          (AsmInfo_t* asm_info, int argc, char* argv[]);

AsmReturnCode ReadCode          (AsmInfo_t* asm_info);
AsmReturnCode GetFileSize       (FILE* const file, size_t* file_size);

AsmReturnCode BuildCode         (AsmInfo_t* asm_info);
RegCode       GetRegCode        (const char* reg_name);
AsmReturnCode ParsePushPopArg   (AsmInfo_t* asm_info, const char* arg);
AsmReturnCode ParseLabelArg     (AsmInfo_t* asm_info, const char* arg);
AsmReturnCode AddLable          (AsmInfo* asm_info, const char* cmd);

AsmReturnCode FixUpLabes        (AsmInfo_t* asm_info);

AsmReturnCode WriteCode         (const AsmInfo_t* asm_info);

AsmReturnCode CloseCode         (AsmInfo_t* asm_info);

#endif // ASM_H__
