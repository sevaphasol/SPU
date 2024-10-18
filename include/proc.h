#include "stack.h"

#ifndef PROC_H__
#define PROC_H__

const size_t CodeArrSize   = 20;

const size_t StackSize     = 8;

const size_t RegsSize      = 4;

const size_t RamSize       = 16;

const char* const DefaultInput = "programm_code.txt";

const char* const DmpName      = "logs/proc_dump.log";

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

typedef enum RegisterCodes
{
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
} RegisterCode;

typedef struct Proc
{
    const char* input_file_name;
    FILE*       input_file;

    const char* dump_file_name;
    FILE*       dump_file;

    bool      running;

    size_t    arr_size;
    size_t    len_code;
    int       ip;
    int       code[CodeArrSize];

    size_t    stk_size;
    StackId_t stk_id;

    size_t    regs_size;
    int       regs[RegsSize];

    size_t    ram_size;
    int       ram[RamSize];
} Proc_t;

void ProcCtor(Proc_t* proc, const int argc, const char* argv[]);

void Run     (Proc_t* proc);

void ProcDtor(Proc_t* proc);

int* GetArg  (Proc_t* proc);

void CmdHlt  (Proc_t* proc);
void CmdPush (Proc_t* proc);
void CmdPop  (Proc_t* proc);
void CmdAdd  (Proc_t* proc);
void CmdSub  (Proc_t* proc);
void CmdMul  (Proc_t* proc);
void CmdDiv  (Proc_t* proc);
void CmdSqrt (Proc_t* proc);
void CmdSin  (Proc_t* proc);
void CmdCos  (Proc_t* proc);
void CmdIn   (Proc_t* proc);
void CmdOut  (Proc_t* proc);
void CmdDump (Proc_t* proc);
void CmdJmp  (Proc_t* proc);
void CmdJa   (Proc_t* proc);
void CmdJb   (Proc_t* proc);
void CmdJae  (Proc_t* proc);
void CmdJbe  (Proc_t* proc);
void CmdJe   (Proc_t* proc);
void CmdJne  (Proc_t* proc);
void CmdDraw (Proc_t* proc);

#endif

