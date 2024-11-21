#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

#include "proc.h"
#include "stack.h"
#include "custom_assert.h"
#include "proc_commands.h"

//———————————————————————————————————————————————————————————————————//

static SpuReturnCode StackCtor         (Stk_t*      stk);
static SpuReturnCode RegsCtor          (Regs_t*     regs);
static SpuReturnCode RamCtor           (Ram_t*      ram);
static SpuReturnCode GraphicsCtor      (Graphics_t* graphics, sf::RenderWindow* window);

static SpuReturnCode OpenCode          (SpuInfo_t* spu_info, int argc, const char* argv[]);
static SpuReturnCode ParseArgv         (SpuInfo*   spu_info, int argc, const char* argv[]);

static SpuReturnCode CheckWindowEvents (SpuInfo_t* spu_info);
static SpuReturnCode ReadCode          (SpuInfo_t* spu_info);
static SpuReturnCode ExecuteCode       (SpuInfo_t* spu_info);

//———————————————————————————————————————————————————————————————————//

SpuReturnCode SpuInfoCtor (SpuInfo_t* spu_info, sf::RenderWindow* window, int argc, const char* argv[])
{
    ASSERT(spu_info);
    ASSERT(window);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    VERIFY(OpenCode(spu_info, argc, argv) != SPU_SUCCESS,
           return SPU_OPEN_CODE_ERROR);

    VERIFY(ReadCode(spu_info) != SPU_SUCCESS,
           return SPU_READ_CODE_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(StackCtor(&spu_info->stk) != SPU_SUCCESS,
           return SPU_STACK_CTOR_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(RegsCtor(&spu_info->proc.regs),
           return SPU_REGS_CTOR_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(RamCtor(&spu_info->ram),
           return SPU_RAM_CTOR_ERROR);

    //-------------------------------------------------------------------//

    VERIFY(GraphicsCtor(&spu_info->graphics, window),
           return SPU_GRAPHICS_CTOR_ERROR);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode StackCtor(Stk_t* stk)
{
    ASSERT(stk);

    //-------------------------------------------------------------------//

    stk->len = StackSize;
    stk->id = STACK_CTOR(StackSize);
    VERIFY(stk->id == INVALID_STACK_ID, return SPU_STACK_CTOR_ERROR);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode RegsCtor(Regs_t* regs)
{
    ASSERT(regs);

    regs->len = RegsAmount;

    for (int reg_code = 1; reg_code <= regs->len; reg_code++)
    {
        regs->regs[reg_code - 1].name = RegNames[reg_code];
        regs->regs[reg_code - 1].code = reg_code;
    }

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode RamCtor(Ram_t* ram)
{
    ASSERT(ram);

    ram->len = RamSize;
    ram->elem_size = sizeof(int);

    ram->ram = (int*) calloc(ram->len, ram->elem_size);
    VERIFY(!ram->ram, return SPU_RAM_CTOR_ERROR);

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode GraphicsCtor(Graphics_t* graphics, sf::RenderWindow* window)
{
    ASSERT(graphics);
    ASSERT(window);

    //-------------------------------------------------------------------//

    graphics->length       = GraphicWindowLength;
    graphics->width        = GraphicWindowWidth;
    graphics->pixel_length = PixelLength;
    graphics->pixel_width  = PixelWidth;
    graphics->name         = GraphicWindowName;
    graphics->window       = window;

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode OpenCode(SpuInfo_t* spu_info, int argc, const char* argv[])
{
    ASSERT(spu_info);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    VERIFY(ParseArgv(spu_info, argc, argv) != SPU_SUCCESS,
           return SPU_PARSE_ARGV_ERROR);

    //-------------------------------------------------------------------//

    spu_info->input.ptr = fopen(spu_info->input.name,  "rb");
    spu_info->dump.ptr  = fopen(spu_info->dump.name,   "w");

    //-------------------------------------------------------------------//

    VERIFY(!(spu_info->input.ptr), return SPU_FILE_OPEN_ERROR);
    VERIFY(!(spu_info->dump.ptr),  return SPU_FILE_OPEN_ERROR);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode ParseArgv(SpuInfo* spu_info, int argc, const char* argv[])
{
    ASSERT(spu_info);
    ASSERT(argv);

    //-------------------------------------------------------------------//

    int option_index = 1;
    int parsed_files = 0;

    if (argc == 1)
    {
        spu_info->input.name = DefaultInput;
        spu_info->dump.name = DefaultDump;

        return SPU_SUCCESS;
    }

    //-------------------------------------------------------------------//

    while (option_index < argc)
	{
        if (strcmp(argv[option_index], "--window") == 0 || strcmp(argv[option_index], "-w") == 0)
		{
            spu_info->graphics.open_window = true;

			option_index++;

            continue;
        }

        if (strcmp(argv[option_index], "--sound") == 0 || strcmp(argv[option_index], "-s") == 0)
		{
            spu_info->graphics.play_sound = true;

			option_index++;

            continue;
        }

        if (parsed_files == 0)
        {
            spu_info->input.name = argv[option_index];

            parsed_files++;

            option_index++;

            continue;
        }

        fprintf(stderr, "INCORRECT INPUT\n I/O files set as default\n\n");

        spu_info->input.name = DefaultInput;

        break;
    }

    spu_info->dump.name = DefaultDump;

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode ReadCode(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    VERIFY(fread(&spu_info->proc.len, sizeof(size_t), 1, spu_info->input.ptr) != 1,
           return SPU_READ_CODE_ERROR);

    //-------------------------------------------------------------------//

    spu_info->proc.elem_size = sizeof(int);
    spu_info->proc.code      = (int*) calloc(spu_info->proc.len, spu_info->proc.elem_size);

    //-------------------------------------------------------------------//

    VERIFY(fread(spu_info->proc.code,
                 spu_info->proc.elem_size,
                 spu_info->proc.len,
                 spu_info->input.ptr) != spu_info->proc.len,
           return SPU_READ_CODE_ERROR);

    //-------------------------------------------------------------------//

    #ifdef PRINT_READ_CODE

    printf("ReadCode:\n");

    for (int i = 0; i < spu_info->proc.len; i++)
    {
        printf("[%d] = %d\n", i, spu_info->proc.code[i]);
    }

    #endif

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode StartProgramm(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    if (spu_info->graphics.play_sound)
    {
        sf::SoundBuffer buffer;
        sf::Sound sound;

        if (buffer.loadFromFile(BadAppleMusicFile))
        {
            sound.setBuffer(buffer);
            sound.play();
        }
    }

    spu_info->proc.running = true;

    if (spu_info->graphics.open_window)
    {
        while (spu_info->graphics.window->isOpen())
        {
            VERIFY((ExecuteCode (spu_info) != SPU_SUCCESS),
                   return SPU_EXECUTE_CODE_ERROR);
        }
    }
    else
    {
        spu_info->graphics.window->close();

        VERIFY((ExecuteCode (spu_info) != SPU_SUCCESS),
               return SPU_EXECUTE_CODE_ERROR);
    }

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode CheckWindowEvents(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    if (spu_info->graphics.open_window)
    {
        sf::Event event;

        while (spu_info->graphics.window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                spu_info->graphics.window->close();

                spu_info->proc.running = false;

                return SPU_SUCCESS;
            }
        }
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode ExecuteCode(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    //-------------------------------------------------------------------//

    CheckWindowEvents(spu_info);

    while (spu_info->proc.running)
    {
        CheckWindowEvents(spu_info);

        bool executed = false;

        for (int command = 0; command < nCommands; command++)
        {
            if (spu_info->proc.code[spu_info->proc.ip] == CommandsTabel[command].code)
            {
                spu_info->proc.ip++;

                SpuReturnCode func_status = CommandsTabel[command].func(spu_info);
                VERIFY(func_status, return func_status);

                executed = true;

                break;
            }
        }

        VERIFY(!executed, return SPU_INVALID_INSRUCTION_ERROR);
    }

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//===================================================================//

SpuReturnCode SpuInfoDtor(SpuInfo_t* spu_info)
{
    ASSERT(spu_info);

    VERIFY(!spu_info->input.ptr, return SPU_CLOSE_INPUT_FILE_ERROR);

    //-------------------------------------------------------------------//

    fclose(spu_info->input.ptr);

    if (!spu_info->dump.ptr)
    {
        fprintf(stderr, "SPU CLOSE DUMP FILE ERROR in %s:%d:%s\n", __FILE__, __LINE__, __PRETTY_FUNCTION__);

        return SPU_CLOSE_DUMP_FILE_ERROR;
    }

    fclose(spu_info->dump.ptr);

    free(spu_info->proc.code);

    free(spu_info->ram.ram);

    StackDtor(spu_info->stk.id);

    //-------------------------------------------------------------------//

    return SPU_SUCCESS;
}

//———————————————————————————————————————————————————————————————————//
