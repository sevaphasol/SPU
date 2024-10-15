#include <stdio.h>

#include "compilator.h"

void Compile(FILE* inputFile, FILE* outputFile)
{
    bool reading = true;

    while (reading)
    {
        char cmd[50] = "";
        fscanf(inputFile, "%s", cmd);

        if (strcmp(cmd, "hlt") == 0)
        {
            fprintf(inputFile, "0\n");

            continue;
        }

        if (strcmp(cmd, "push") == 0)
        {
            fscanf("%s", cmd);
            fprintf(inputFile, "1 %s\n", cmd);

            continue;
        }

        if (strcmp(cmd, "add") == 0)
        {
            fprintf(inputFile, "2\n");

            continue;
        }

        if (strcmp(cmd, "sub") == 0)
        {
            fprintf(inputFile, "3\n");

            continue;
        }

        if (strcmp(cmd, "mul") == 0)
        {
            fprintf(inputFile, "4\n");

            continue;
        }

        if (strcmp(cmd, "div") == 0)
        {
            fprintf(inputFile, "5\n");

            continue;
        }

        if (strcmp(cmd, "out") == 0)
        {
            fprintf(inputFile, "6\n");

            continue;
        }

        if (strcmp(cmd, "dump") == 0)
        {
            fprintf(inputFile, "7\n");

            continue;
        }

        fprintf(stderr, "Syntax error: %s\n", cmd);
}
