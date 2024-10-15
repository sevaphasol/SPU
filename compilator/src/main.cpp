#include <stdio.h>

#include "compilator.h"

int main(int argc, const char* argv[])
{
    if (argc != 3)
    {
        printf(stderr, "INCORRECT INPUT\n");

        return 0;
    }

    FILE* inputFile = fopen(argv[1], "r");
    FILE* ouputFile = fopen(argv[2], "w");

    Compile(inputFile, outputFile);

    return 0;
}
