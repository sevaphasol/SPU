#ifndef PROC_COMMANDS_H__
#define PROC_COMMANDS_H__

//———————————————————————————————————————————————————————————————————//

#include "proc.h"

//———————————————————————————————————————————————————————————————————//

const int nCommands = 25;

//———————————————————————————————————————————————————————————————————//

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

//———————————————————————————————————————————————————————————————————//

int*          GetArg            (SpuInfo_t* spu_info);

//-------------------------------------------------------------------//

SpuReturnCode TerminalDraw      (SpuInfo_t* spu_info, int start_ram_index, int length, int width);
SpuReturnCode GraphicWindowDraw (SpuInfo_t* spu_info, int start_ram_index, int length, int width);

//-------------------------------------------------------------------//

SpuReturnCode CmdHlt            (SpuInfo_t* spu_info);
SpuReturnCode CmdPush           (SpuInfo_t* spu_info);
SpuReturnCode CmdPop            (SpuInfo_t* spu_info);
SpuReturnCode CmdAdd            (SpuInfo_t* spu_info);
SpuReturnCode CmdSub            (SpuInfo_t* spu_info);
SpuReturnCode CmdMul            (SpuInfo_t* spu_info);
SpuReturnCode CmdDiv            (SpuInfo_t* spu_info);
SpuReturnCode CmdSqrt           (SpuInfo_t* spu_info);
SpuReturnCode CmdSin            (SpuInfo_t* spu_info);
SpuReturnCode CmdCos            (SpuInfo_t* spu_info);
SpuReturnCode CmdIn             (SpuInfo_t* spu_info);
SpuReturnCode CmdOut            (SpuInfo_t* spu_info);
SpuReturnCode CmdDump           (SpuInfo_t* spu_info);
SpuReturnCode CmdJmp            (SpuInfo_t* spu_info);
SpuReturnCode CmdJa             (SpuInfo_t* spu_info);
SpuReturnCode CmdJb             (SpuInfo_t* spu_info);
SpuReturnCode CmdJae            (SpuInfo_t* spu_info);
SpuReturnCode CmdJbe            (SpuInfo_t* spu_info);
SpuReturnCode CmdJe             (SpuInfo_t* spu_info);
SpuReturnCode CmdJne            (SpuInfo_t* spu_info);
SpuReturnCode CmdDraw           (SpuInfo_t* spu_info);
SpuReturnCode CmdCall           (SpuInfo_t* spu_info);
SpuReturnCode CmdRet            (SpuInfo_t* spu_info);
SpuReturnCode CmdClear          (SpuInfo_t* spu_info);
SpuReturnCode CmdSleep          (SpuInfo_t* spu_info);

//———————————————————————————————————————————————————————————————————//

struct SpuCommand_t
{
    CmdCode code;
    SpuReturnCode (*func)(SpuInfo_t*);
};

const SpuCommand_t CommandsTabel[nCommands]   = {{.code = HLT,   .func = &CmdHlt},
                                                 {.code = PUSH,  .func = &CmdPush},
                                                 {.code = POP,   .func = &CmdPop},
                                                 {.code = ADD,   .func = &CmdAdd},
                                                 {.code = SUB,   .func = &CmdSub},
                                                 {.code = MUL,   .func = &CmdMul},
                                                 {.code = DIV,   .func = &CmdDiv},
                                                 {.code = SQRT,  .func = &CmdSqrt},
                                                 {.code = SIN,   .func = &CmdSin},
                                                 {.code = COS,   .func = &CmdCos},
                                                 {.code = IN,    .func = &CmdIn},
                                                 {.code = OUT,   .func = &CmdOut},
                                                 {.code = DUMP,  .func = &CmdDump},
                                                 {.code = JMP,   .func = &CmdJmp},
                                                 {.code = JA,    .func = &CmdJa},
                                                 {.code = JB,    .func = &CmdJb},
                                                 {.code = JAE,   .func = &CmdJae},
                                                 {.code = JBE,   .func = &CmdJbe},
                                                 {.code = JE,    .func = &CmdJe},
                                                 {.code = JNE,   .func = &CmdJne},
                                                 {.code = DRAW,  .func = &CmdDraw},
                                                 {.code = CALL,  .func = &CmdCall},
                                                 {.code = RET,   .func = &CmdRet},
                                                 {.code = CLEAR, .func = &CmdClear},
                                                 {.code = SLEEP, .func = &CmdSleep}};

//———————————————————————————————————————————————————————————————————//

#endif // PROC_COMMANDS_H__
