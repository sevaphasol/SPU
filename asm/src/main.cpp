#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "INCORRECT INPUT\n");

        return 0;
    }

    FILE* inputFile  = fopen(argv[1], "r");
    FILE* outputFile = fopen(argv[2], "w");

    LenArr_t codeArrStruct = {.len = 0, .codeArr = {}};
    memset((void*) codeArrStruct.codeArr, Poison, ArrSize);

    Labels_t labelsStruct  = {.nLabels = 0, .Addrs = {}};
    memset((void*) labelsStruct.Addrs, Poison, LabelsSize);

    MakeArr(&codeArrStruct, &labelsStruct, inputFile);

    Write  (&codeArrStruct, outputFile);

    return 0;
}
