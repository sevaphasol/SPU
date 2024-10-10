#ifndef PROC_H__
#define PROC_H__

typedef enum CommadCodes
{
    HLT  = 0,
    PUSH = 1,
    ADD  = 2,
    SUB  = 3,
    MUL  = 4,
    DIV  = 5,
    OUT  = 6,
    DUMP = 7,
} CommandCode;

void Run();

#endif
