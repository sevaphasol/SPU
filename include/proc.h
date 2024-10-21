#include "stack.h"

#ifndef PROC_H__
#define PROC_H__

#define SPU_INFO_INIT .input = {.name = nullptr, .ptr = nullptr, .size = 0, .n_strings = 0, .data = nullptr},    \
                      .dump  = {.name = nullptr, .ptr = nullptr, .size = 0, .n_strings = 0, .data = nullptr},    \
                      .proc  = {.running = false, .len = 0, .elem_size = sizeof(int), .ip = 0, .code = nullptr,  \
                      .regs  = {.len = RegsSize, .regs = {0}}},                                                  \
                      .stk   = {.len = StackSize, .id = {0}},                                                    \
                      .ram   = {.len = RamSize, .ram = {0}}                                                      \

#define REG_CTOR(reg_code) spu_info->proc.regs.regs[reg_code - 1].name = #reg_code; \
                           spu_info->proc.regs.regs[reg_code - 1].code = reg_code;  \

// #define PRINT_READ_CODE

const size_t StackSize     = 8;
const size_t RamSize       = 256;
const size_t MaxRegName    = 2;
const size_t RegsSize      = 4;

const char* const DefaultInput = "asm/assembled files/example_programm_code.bin";

const char* const DefaultDump  = "logs/proc_dump.log";

typedef enum SpuReturnCodes
{
    SPU_SUCCESS,
    SPU_FILE_OPEN_ERROR,
    SPU_OPEN_CODE_ERROR,
    SPU_READ_CODE_ERROR,
    SPU_INVALID_INSRUCTION_ERROR,
    SPU_DIVISION_BY_ZERO_ERROR,
    SPU_INFO_NULL_PTR_ERROR,
    SPU_CLOSE_INPUT_FILE_ERROR,
    SPU_CLOSE_DUMP_FILE_ERROR,
} SpuReturnCode;

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

typedef enum RegisterCodes
{
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
} RegisterCode;

typedef struct Stream
{
    const char* name;
    FILE*       ptr;
    size_t      size;
    size_t      n_strings;
    char*       data;
} Stream_t;

typedef struct Register
{
    const char* name;
    int         code;
    int         value;
} Reg_t;

typedef struct Registers
{
    size_t len;
    Reg_t  regs[RegsSize];
} Regs_t;

typedef struct Proc
{
    bool running;
    size_t len;
    size_t elem_size;
    int    ip;
    int*   code;
    Regs_t regs;
} Proc_t;

typedef struct Stk
{
    size_t len;
    StackId_t id;
} Stk_t;

typedef struct Ram
{
    size_t len;
    size_t elem_size;
    int    ram[RamSize];
} Ram_t;

typedef struct SpuInfo
{
    Stream_t input;
    Stream_t dump;
    Proc_t   proc;
    Stk_t    stk;
    Ram_t    ram;
} SpuInfo_t;

SpuReturnCode SpuInfoCtor(SpuInfo_t* spu_info, int argc, const char* argv[]);

SpuReturnCode OpenCode   (SpuInfo_t* spu_info, int argc, const char* argv[]);

SpuReturnCode ReadCode   (SpuInfo_t* spu_info);

SpuReturnCode ExecuteCode(SpuInfo_t* spu_info);

SpuReturnCode SpuInfoDtor(SpuInfo_t* spu_info);

int*          GetArg  (SpuInfo_t* spu_info);

SpuReturnCode CmdHlt  (SpuInfo_t* spu_info);
SpuReturnCode CmdPush (SpuInfo_t* spu_info);
SpuReturnCode CmdPop  (SpuInfo_t* spu_info);
SpuReturnCode CmdAdd  (SpuInfo_t* spu_info);
SpuReturnCode CmdSub  (SpuInfo_t* spu_info);
SpuReturnCode CmdMul  (SpuInfo_t* spu_info);
SpuReturnCode CmdDiv  (SpuInfo_t* spu_info);
SpuReturnCode CmdSqrt (SpuInfo_t* spu_info);
SpuReturnCode CmdSin  (SpuInfo_t* spu_info);
SpuReturnCode CmdCos  (SpuInfo_t* spu_info);
SpuReturnCode CmdIn   (SpuInfo_t* spu_info);
SpuReturnCode CmdOut  (SpuInfo_t* spu_info);
SpuReturnCode CmdDump (SpuInfo_t* spu_info);
SpuReturnCode CmdJmp  (SpuInfo_t* spu_info);
SpuReturnCode CmdJa   (SpuInfo_t* spu_info);
SpuReturnCode CmdJb   (SpuInfo_t* spu_info);
SpuReturnCode CmdJae  (SpuInfo_t* spu_info);
SpuReturnCode CmdJbe  (SpuInfo_t* spu_info);
SpuReturnCode CmdJe   (SpuInfo_t* spu_info);
SpuReturnCode CmdJne  (SpuInfo_t* spu_info);
SpuReturnCode CmdDraw (SpuInfo_t* spu_info);
SpuReturnCode CmdCall (SpuInfo_t* spu_info);
SpuReturnCode CmdRet  (SpuInfo_t* spu_info);

#endif // PROC_H__

