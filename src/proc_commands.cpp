#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "proc_commands.h"
#include "custom_assert.h"

SpuReturnCode CmdHlt(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    spu_info->proc.running = false;

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdPush(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int arg = *GetArg(spu_info);

    StackPush(spu_info->stk.id, arg);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdPop(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int* arg = GetArg(spu_info);

    int val = StackPop(spu_info->stk.id);

    *arg = val;

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdAdd(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    StackPush(spu_info->stk.id, a + b);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdSub(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    StackPush(spu_info->stk.id, b - a);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdMul(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    StackPush(spu_info->stk.id, a * b);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdDiv(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    VERIFY(a == 0, return SPU_DIVISION_BY_ZERO_ERROR);

    StackPush(spu_info->stk.id, b / a);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdSqrt(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);

    VERIFY(a < 0, return SPU_ROOT_OF_A_NEGATIVE_ERROR);

    StackPush(spu_info->stk.id, (int) sqrt(a));

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdSin(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    StackPush(spu_info->stk.id, (int) sin(StackPop(spu_info->stk.id))); // allways returns 0

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdCos(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    StackPush(spu_info->stk.id, (int) cos(StackPop(spu_info->stk.id))); // allways returns 0

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

SpuReturnCode CmdIn(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = 0;
    scanf("%d", &a);

    StackPush(spu_info->stk.id, a);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdOut(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);

    printf("%d\n", a);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdDump(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    SpecialStackDump(spu_info->stk.id);

    FILE* dump_file = spu_info->dump.ptr;

    //-------------------------------------------------------------------//

    time_t RawTime;
    struct tm* TimeInfo;
    time(&RawTime);
    TimeInfo = localtime(&RawTime);

    fprintf(dump_file, "Local time and date: %s\n", asctime(TimeInfo));

    //-------------------------------------------------------------------//

    fprintf(dump_file, "input file = %s\n", spu_info->input.name);
    fprintf(dump_file, "input file pointer = %p\n\n", spu_info->input.ptr);

    fprintf(dump_file, "running = %s\n\n", spu_info->proc.running ? "true" : "false");

    fprintf(dump_file, "arr_size = %ld\n",  spu_info->proc.len);
    fprintf(dump_file, "regs_size = %ld\n", spu_info->proc.regs.len);
    fprintf(dump_file, "ram_size = %ld\n",  spu_info->ram.len);

    fprintf(dump_file, "\nlen_code = %ld\n",  spu_info->proc.len);
    fprintf(dump_file, "ip       = %d\n\n", spu_info->proc.ip);

    //-------------------------------------------------------------------//

    fprintf(dump_file, "CODE ARRAY\n\n");

    for (int i = 0; i < spu_info->proc.len; i++)
    {
        fprintf(dump_file, "[%d] = %d\n", i, spu_info->proc.code[i]);
    }

    //-------------------------------------------------------------------//

    fprintf(dump_file, "\nREGISTERS\n\n");

    for (int i = 0; i < spu_info->proc.regs.len; i++)
    {
        fprintf(dump_file, "%s [ak %d] = %d\n",
                spu_info->proc.regs.regs[i].name, spu_info->proc.regs.regs[i].code, spu_info->proc.regs.regs[i].value);
    }

    //-------------------------------------------------------------------//

    fprintf(dump_file, "\nRAM\n\n");

    for (int i = 0; i < RamDisplaySize; i++)
    {
        fprintf(dump_file, "[%d] = %d\n", i, spu_info->ram.ram[i]);
    }

    //-------------------------------------------------------------------//

    fflush(dump_file);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJmp(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJa(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b > a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];
    }
    else
    {
        spu_info->proc.ip++;
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJb(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b < a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];
    }
    else
    {
        spu_info->proc.ip++;
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJae(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b >= a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];
    }
    else
    {
        spu_info->proc.ip++;
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJbe(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b <= a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];
    }
    else
    {
        spu_info->proc.ip++;
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJe(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b == a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];
    }
    else
    {
        spu_info->proc.ip++;
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdJne(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int a = StackPop(spu_info->stk.id);
    int b = StackPop(spu_info->stk.id);

    if (b != a)
    {
        spu_info->proc.ip = spu_info->proc.code[spu_info->proc.ip];
    }
    else
    {
        spu_info->proc.ip++;
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdDraw(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int mode            = spu_info->proc.code[spu_info->proc.ip++];
    int start_ram_index = *GetArg(spu_info);
    int length          = *GetArg(spu_info);
    int width           = *GetArg(spu_info);

    if (spu_info->graphics.open_window)
    {
        VERIFY(GraphicWindowDraw(spu_info, start_ram_index, length, width) != SPU_SUCCESS,
               return SPU_CMD_DRAW_ERROR);
    }
    else
    {
        VERIFY(TerminalDraw(spu_info, start_ram_index, length, width) != SPU_SUCCESS,
               return SPU_CMD_DRAW_ERROR);
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode TerminalDraw(SpuInfo_t* spu_info, int start_ram_index, int length, int width)
{
    ASSERT(spu_info);

    VERIFY(start_ram_index + length * width >= RamSize,
           return SPU_INVALID_START_RAM_INDEX_ERROR);

    VERIFY(length < 0 || width < 0,
           return SPU_INVALID_DRAW_PARAM_ERROR);

    //-------------------------------------------------------------------//

    for (int i = 0; i < width; i++)
    {
        for (int j = i * length; j < (i + 1) * length; j++)
        {
            putchar(spu_info->ram.ram[start_ram_index + j]);
        }

        putchar('\n');
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode GraphicWindowDraw(SpuInfo_t* spu_info, int start_ram_index, int length, int width)
{
    ASSERT(spu_info);

    VERIFY(!spu_info->graphics.open_window,
           return SPU_WINDOW_IS_NOT_OPENED_ERROR);

    VERIFY(start_ram_index + length * width >= RamSize,
           return SPU_INVALID_START_RAM_INDEX_ERROR);

    VERIFY(length < 0 || width < 0,
           return SPU_INVALID_DRAW_PARAM_ERROR);

    //-------------------------------------------------------------------//

    sf::RenderTexture texture;
    texture.create(length*spu_info->graphics.pixel_length, width*PixelWidth);

    sf::RectangleShape rectangle(sf::Vector2f((float) PixelLength, (float) (PixelWidth)));

    float x, y = 0;

    for (int i = 0; i < width; i++)
    {
        for (int j = start_ram_index + i * length; j < start_ram_index + (i + 1) * length; j++)
        {
            rectangle.setPosition(x, y);

            if (isspace(spu_info->ram.ram[j]) || spu_info->ram.ram[j] == 0)
            {
                rectangle.setFillColor(sf::Color::Black);
            }
            else
            {
                rectangle.setFillColor(sf::Color::White);
            }

            texture.draw(rectangle);

            x += PixelLength;
        }

        x = 0;
        y += PixelWidth;
    }

    texture.display();

    sf::Sprite sprite(texture.getTexture());

    (*(spu_info->graphics.window)).clear();
    (*(spu_info->graphics.window)).draw(sprite);
    (*(spu_info->graphics.window)).display();

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdCall(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int func_ip = spu_info->proc.code[spu_info->proc.ip];
    int ret_ip  = spu_info->proc.ip + 1;

    StackPush(spu_info->stk.id, ret_ip);

    spu_info->proc.ip = func_ip;

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdRet(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int ret_ip = StackPop(spu_info->stk.id);
    spu_info->proc.ip = ret_ip;

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdClear(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    system("clear");

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CmdSleep(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    if (!spu_info->graphics.open_window)
    {
        usleep(30000);
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

int* GetArg (SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    int arg_type = spu_info->proc.code[spu_info->proc.ip++];

    static int arg_value;

    arg_value = 0;

    int*       arg_addr  = nullptr;

    if (arg_type & ImcCode)
    {
        arg_value +=  spu_info->proc.code[spu_info->proc.ip];
        arg_addr   = &spu_info->proc.code[spu_info->proc.ip];

        spu_info->proc.ip++;
    }

    if (arg_type & RegCode)
    {
        int reg_number = spu_info->proc.code[spu_info->proc.ip] - 1;

        arg_value +=  spu_info->proc.regs.regs[reg_number].value;
        arg_addr   = &spu_info->proc.regs.regs[reg_number].value;

        spu_info->proc.ip++;
    }

    if (arg_type & ImcCode && arg_type & RegCode)
    {
        arg_addr = &arg_value;
    }

    if (arg_type & RamIndexCode)
    {
        int ram_addres = arg_value;

        arg_addr = &spu_info->ram.ram[ram_addres];
    }

    //-------------------------------------------------------------------//

    return arg_addr;
}

//===================================================================//
