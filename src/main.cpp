#include <stdio.h>
#include <stdlib.h>

#include "stack.h"
#include "proc.h"

int main(const int argc, const char* argv[])
{
    SpuInfo_t spu_info = {SPU_INFO_INIT};

    if (SpuInfoCtor(&spu_info, argc, argv) != SPU_SUCCESS)
    {
        SpuInfoDtor(&spu_info);

        return EXIT_FAILURE;
    }

    if (ExecuteCode (&spu_info) != SPU_SUCCESS)
    {
        SpuInfoDtor(&spu_info);

        return EXIT_FAILURE;
    }

    if (SpuInfoDtor (&spu_info) != SPU_SUCCESS)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
