#include <stdio.h>
#include <assert.h>

#ifndef ASM_H__
#define ASM_H__

#define ASSERT(validation_info) assert(validation_info != ASM_INFO_INVALID)

#define ASM_INFO_INIT .input  = {.name = nullptr, .ptr = nullptr, .size = 0, .data = nullptr}, \
                      .output = {.name = nullptr, .ptr = nullptr, .size = 0, .data = nullptr}, \
                      .code   = {.len = 0, .elem_size = sizeof(int), .ip = 0, .code   = {0}},  \
                      .labels = {.len = LabelsSize, .elem_size = sizeof(int), .labels = {0}},  \

#define asserted && fprintf(stderr, "asserted macro %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__)

const int CodeArrSize   = 64;
const int LabelsSize    = 16;
const int MaxLabelName  = 10;
const int MaxLineSize   = 64;
const int Poison        = -1;

const int ImmerseConstCode = 1;
const int RegisterCode     = 2;
const int RamCode          = 4;

const char* const DefaultInput  = "programm.asm";
const char* const DefaultOutput = "programm_code.bin";

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

typedef struct Labels
{
    size_t  len;
    size_t  elem_size;
    size_t  n_labels;
    Label_t labels[LabelsSize];
} Labels_t;

typedef struct AsmInfo
{
    Stream_t input;
    Stream_t output;
    Code_t   code;
    Labels_t labels;
} AsmInfo_t;

typedef enum AsmReturnCodes
{
    ERROR                    = -1,
    NO_ERROR                 = 0,
    FILE_OPEN_ERROR          = 1,
    FILE_READ_ERROR          = 2,
    NULL_PTR_ERROR           = 3,
    ASM_INFO_VALID           = 4,
    ASM_INFO_INVALID         = 5,
    GET_FILE_SIZE_ERROR      = 6,
    GET_NUM_OF_STRINGS_ERROR = 7,
    MAKE_STRINGS_ERROR       = 8,
    PARSE_ARG_ERROR          = 9,
    INVALID_LABEL_ERROR      = 10,
    INVALID_LABEL_NAME_ERROR = 11,
    DOUBLE_LABEL_ERROR       = 12,
    PARSE_JMP_ARG_ERROR      = 13,
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
} CmdCode;

typedef enum RegCodes
{
    INVALID_CODE = -1,
    AX           = 1,
    BX           = 2,
    CX           = 3,
    DX           = 4,
} RegCode;

AsmReturnCode AsmInfoValid      (const AsmInfo* asm_info);

AsmReturnCode OpenCode          (AsmInfo_t* asm_info, int argc, const char* argv[]);

AsmReturnCode ReadCode          (AsmInfo_t* asm_info);
AsmReturnCode GetFileSize       (FILE* const file, size_t* file_size);
AsmReturnCode GetNumOfStrings   (Stream_t* stream);

AsmReturnCode BuildCode         (AsmInfo_t* asm_info);
RegCode       GetRegCode        (const char* reg_name);
AsmReturnCode ParsePushPopArg   (AsmInfo_t* asm_info, const char* arg);
AsmReturnCode ParseJumpArg      (AsmInfo_t* asm_info, const char* arg);
AsmReturnCode AddLable          (AsmInfo* asm_info, const char* cmd);

AsmReturnCode WriteCode         (const AsmInfo_t* asm_info);

AsmReturnCode CloseCode         (AsmInfo_t* asm_info);

#endif // ASM_H__
