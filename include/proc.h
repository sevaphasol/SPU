#ifndef PROC_H__
#define PROC_H__

//———————————————————————————————————————————————————————————————————//

#include <SFML/Graphics.hpp>

//-------------------------------------------------------------------//

#include "stack.h"

//———————————————————————————————————————————————————————————————————//

// #define PRINT_READ_CODE

//———————————————————————————————————————————————————————————————————//

const size_t StackSize                 = 8;

//-------------------------------------------------------------------//

const size_t RegsAmount                = 8;
const size_t MaxRegName                = 2;
const char* const RegNames[RegsAmount] = {"AX", "BX", "CX", "DX", "SP", "BP", "SI", "DI"};

//-------------------------------------------------------------------//

const size_t RamSize                   = 1024*1024*1024;
const size_t RamDisplaySize            = 128;

//-------------------------------------------------------------------//

const int    ImcCode                   = 1;
const int    RegCode                   = 2;
const int    RamIndexCode              = 4;

//-------------------------------------------------------------------//

const size_t FrameSize                 = 97*36;
const int    PixelLength               = 5;
const int    PixelWidth                = 10;
const int    GraphicWindowLength       = 97*PixelLength;
const int    GraphicWindowWidth        = 36*PixelWidth;
const int    TerminalDrawCode          = 0;
const int    GraphicWindowDrawCode     = 1;
const char* const GraphicWindowName    = "SPU";
const char* const BadAppleMusicFile    = "sounds/BadApple.wav";

//-------------------------------------------------------------------//

const char* const DefaultInput         = "asm/executable_files/circle2.bin";
const char* const DefaultDump          = "logs/proc_dump.log";

//———————————————————————————————————————————————————————————————————//

enum SpuReturnCode
{
    SPU_SUCCESS                       = 0,
    SPU_STACK_CTOR_ERROR              = 1,
    SPU_REGS_CTOR_ERROR               = 2,
    SPU_RAM_CTOR_ERROR                = 3,
    SPU_GRAPHICS_CTOR_ERROR           = 4,
    SPU_PARSE_ARGV_ERROR              = 5,
    SPU_FILE_OPEN_ERROR               = 6,
    SPU_INVALID_ARGV_ERROR            = 7,
    SPU_NULL_PTR_ERROR                = 8,
    SPU_OPEN_CODE_ERROR               = 9,
    SPU_READ_CODE_ERROR               = 10,
    SPU_INVALID_INSRUCTION_ERROR      = 11,
    SPU_DIVISION_BY_ZERO_ERROR        = 12,
    SPU_INFO_NULL_PTR_ERROR           = 13,
    SPU_CLOSE_INPUT_FILE_ERROR        = 14,
    SPU_CLOSE_DUMP_FILE_ERROR         = 15,
    SPU_INVALID_START_RAM_INDEX_ERROR = 16,
    SPU_CMD_DRAW_ERROR                = 17,
    SPU_ROOT_OF_A_NEGATIVE_ERROR      = 18,
    SPU_DIV_ERROR                     = 19,
    SPU_SQRT_ERROR                    = 20,
    SPU_INVALID_DRAW_PARAM_ERROR      = 21,
    SPU_WINDOW_IS_NOT_OPENED_ERROR    = 22,
    SPU_DRAW_ERROR                    = 23,
    SPU_START_PROGRAMM_ERROR          = 24,
    SPU_EXECUTE_CODE_ERROR            = 25,
};

//-------------------------------------------------------------------//

typedef struct Stream
{
    const char* name;
    FILE*       ptr;
    size_t      size;
    size_t      n_strings;
    char*       data;
} Stream_t;

//-------------------------------------------------------------------//

typedef struct Register
{
    const char* name;
    int         code;
    int         value;
} Reg_t;

//-------------------------------------------------------------------//

typedef struct Registers
{
    size_t len;
    Reg_t  regs[RegsAmount];
} Regs_t;

//-------------------------------------------------------------------//

typedef struct Proc
{
    bool running;
    size_t len;
    size_t elem_size;
    int    ip;
    int*   code;
    Regs_t regs;
} Proc_t;

//-------------------------------------------------------------------//

typedef struct Stk
{
    size_t len;
    StackId_t id;
} Stk_t;

//-------------------------------------------------------------------//

typedef struct Ram
{
    size_t len;
    size_t elem_size;
    int*   ram;
} Ram_t;

//-------------------------------------------------------------------//

typedef struct Graphics
{
    bool              open_window;
    bool              play_sound;
    int               length;
    int               width;
    int               pixel_length;
    int               pixel_width;
    const char*       name;
    sf::RenderWindow* window;
} Graphics_t;

//-------------------------------------------------------------------//

typedef struct SpuInfo
{
    Stream_t   input;
    Stream_t   dump;
    Proc_t     proc;
    Stk_t      stk;
    Ram_t      ram;
    Graphics_t graphics;
} SpuInfo_t;

//———————————————————————————————————————————————————————————————————//

SpuReturnCode SpuInfoCtor(SpuInfo_t* spu_info, sf::RenderWindow* window, int argc, const char* argv[]);
SpuReturnCode StartProgramm(SpuInfo_t* spu_info);
SpuReturnCode SpuInfoDtor(SpuInfo_t* spu_info);

//———————————————————————————————————————————————————————————————————//

#endif // PROC_H__

