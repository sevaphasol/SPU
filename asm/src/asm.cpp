#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm.h"

void MakeArr(Code_t* code)
{
    bool reading = true;

    int* codeArr = codeArrStruct->codeArr;

    int* labels    = labelsStruct->Addrs;
    size_t nLabels = labelsStruct->nLabels;

    int ip = 0;

    for (int i =0; i < 16; i++)
    {
        printf("%d\n", codeArr[i]);
    }

    while (reading)
    {
        char cmd[MaxLineSize] = "";
        fscanf(inputFile, "%s", cmd);

        if (strcmp(cmd, "") == 0)
        {
            continue;
        }

        if (strchr(cmd, ':'))
        {
            int label = atoi(cmd);

            if (nLabels >= LabelsSize)
            {
                fprintf(stderr, "LABELS OVERFLOW\n");

                break;
            }

            if (!((0 <= label) && (label <= LabelsSize - 1)))
            {
                fprintf(stderr, "INVALID LABEL\n");

                break;
            }

            if(labels[label] != Poison)
            {
                fprintf(stderr, "REDEFENITION OF LABEL\n");

                break;
            }

            if ((nLabels < LabelsSize) && (0 <= label <= nLabels - 1) && (labels[label] != -1))
            {
                labels[label] = ip;
            }

            continue;
        }

        if (strcmp(cmd, "push") == 0)
        {
            codeArr[ip++] = 1;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "add") == 0)
        {
            codeArr[ip++] = 2;

            continue;
        }

        if (strcmp(cmd, "sub") == 0)
        {
            codeArr[ip++] = 3;

            continue;
        }

        if (strcmp(cmd, "mul") == 0)
        {
            codeArr[ip++] = 4;

            continue;
        }

        if (strcmp(cmd, "div") == 0)
        {
            codeArr[ip++] = 5;

            continue;
        }

        if (strcmp(cmd, "out") == 0)
        {
            codeArr[ip++] = 6;

            continue;
        }

        if (strcmp(cmd, "dump") == 0)
        {
            codeArr[ip++] = 7;

            continue;
        }

        if (strcmp(cmd, "pushr") == 0)
        {
            codeArr[ip++] = 8;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "pop") == 0)
        {
            codeArr[ip++] = 9;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "jmp") == 0)
        {
            codeArr[ip++] = 10;

            fscanf(inputFile, "%s", cmd);

            int arg = atoi(cmd);

            if (strchr(cmd, ':'))
            {
                codeArr[ip++] = labels[arg];
            }
            else
            {
                codeArr[ip++] = arg;
            }

            continue;
        }

        if (strcmp(cmd, "ja") == 0)
        {
            codeArr[ip++] = 11;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "jb") == 0)
        {
            codeArr[ip++] = 12;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "jae") == 0)
        {
            codeArr[ip++] = 13;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "jbe") == 0)
        {
            codeArr[ip++] = 14;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "je") == 0)
        {
            codeArr[ip++] = 15;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "jne") == 0)
        {
            codeArr[ip++] = 16;

            fscanf(inputFile, "%s", cmd);

            codeArr[ip++] = atoi(cmd);

            continue;
        }

        if (strcmp(cmd, "hlt") == 0)
        {
            codeArr[ip++] = 0;

            reading = false;

            continue;
        }

        fprintf(stderr, "Syntax error: %s\n", cmd);

        reading = false;
    }

    codeArrStruct->len = ip;
}

void Write(LenArr_t* codeArrStruct, FILE* outputFile)
{
    int ip = 0;

    while(ip < codeArrStruct->len)
    {
        int arg = codeArrStruct->codeArr[ip];

        switch (arg)
        {
            /*Pass through all instructions, requires argument on the same line*/

            case 1:  case 8:  case 9:  case 10: case 11:
            case 12: case 13: case 14: case 15: case 16:
            {
                fprintf(outputFile, "%d ", arg);

                arg = codeArrStruct->codeArr[++ip];
            }

            default:
                fprintf(outputFile, "%d\n", arg);

                ip++;

                break;
        }
    }
}
