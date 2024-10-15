#ifndef PROC_H__
#define PROC_H__

const int ArrSize = 16;

typedef enum CommandCodes
{
    HLT   = 0,
    PUSH  = 1,
    ADD   = 2,
    SUB   = 3,
    MUL   = 4,
    DIV   = 5,
    OUT   = 6,
    DUMP  = 7,
    PUSHR = 8,
    POP   = 9,
    JMP   = 10,
    JA    = 11,
    JB    = 12,
    JAE   = 13,
    JBE   = 14,
    JE    = 15,
    JNE   = 16,
} CommandCode;

typedef enum RegisterCodes
{
    AX = 1,
    BX = 2,
    CX = 3,
    DX = 4,
} RegisterCode;

void Run(int codeAddr[ArrSize]);

#endif
