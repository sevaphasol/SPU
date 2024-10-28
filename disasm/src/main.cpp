#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "disasm.h"

int main(int argc, const char* argv[])
{
    DisAsmInfo_t dis_asm_info = {DIS_ASM_INFO_INIT};

    if (OpenCode (&dis_asm_info, argc, argv) != DIS_ASM_SUCCESS)
    {
        CloseCode (&dis_asm_info);

        return EXIT_FAILURE;
    }

    if (ReadCode (&dis_asm_info) != DIS_ASM_SUCCESS)
    {
        CloseCode (&dis_asm_info);

        return EXIT_FAILURE;
    }

    if (BuildCode (&dis_asm_info) != DIS_ASM_SUCCESS)
    {
        CloseCode (&dis_asm_info);

        return EXIT_FAILURE;
    }

    if (CloseCode (&dis_asm_info) != DIS_ASM_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
