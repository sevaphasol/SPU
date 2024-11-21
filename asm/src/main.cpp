#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//-------------------------------------------------------------------//

#include "asm.h"

//———————————————————————————————————————————————————————————————————//

int main(int argc, const char* argv[])
{
    AsmInfo_t asm_info = {};

    //-------------------------------------------------------------------//

    if (AsmCtor(&asm_info, argc, argv) != ASM_SUCCESS)
    {
        CloseCode(&asm_info);

        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    if (BuildCode(&asm_info) != ASM_SUCCESS)
    {
        CloseCode(&asm_info);

        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    if (FixUpLabels(&asm_info) != ASM_SUCCESS)
    {
        CloseCode(&asm_info);

        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    if (WriteCode(&asm_info) != ASM_SUCCESS)
    {
        CloseCode(&asm_info);

        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    if (CloseCode(&asm_info) != ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    //-------------------------------------------------------------------//

    return EXIT_SUCCESS;
}
