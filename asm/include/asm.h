#include <stdio.h>

#ifndef ASM_H__
#define ASM_H__

const int CodeArrSize   = 64;

const int RamSize       = 1024;

const int LabelsSize    = 16;

const int MaxLineSize   = 64;

const int Poison        = -1;

typedef struct Labels
{
    size_t nLabels;
    int    Addrs[LabelsSize];
} Labels_t;

typedef struct Proc
{
    FILE* input_file;
    FILE* output_file;

    size_t len_code;
    int code[CodeArrSize];

    size_t len_labels;
    int labels[LabelsSize];

    int ram[RamSize];
} Proc_t;

typedef enum CmdCodes
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
} CmdCode;

void OpenCode (Code_t* code);

void MakeCode (Code_t* code);

void WriteCode(Code_t* code);

#endif // ASM_H__
