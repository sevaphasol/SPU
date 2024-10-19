#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

int main(int argc, const char* argv[])
{
    AsmInfo_t asm_info = {ASM_INFO_INIT};

    OpenCode(&asm_info, argc, argv) asserted;

    ReadCode(&asm_info)             asserted;

    BuildCode(&asm_info)            asserted;

    WriteCode(&asm_info)            asserted;

    CloseCode(&asm_info)            asserted;

    return 0;
}
