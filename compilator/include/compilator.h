#include <stdio.h>

#ifndef COMPILATOR_H__
#define COMPILATOR_H__

const int ArrSize     = 64;

const int LabelsSize  = 16;

const int MaxLineSize = 64;

typedef struct LenArr
{
    size_t len;
    int    codeArr[ArrSize];
} LenArr_t;

typedef struct Labels
{
    size_t nLabels;
    int    Addrs[LabelsSize];
} Labels_t;

void MakeArr(LenArr_t* codeArrStruct, Labels_t* labelsStruct, FILE* inputFile);

void Write  (LenArr_t* codeArrStruct, FILE* outputFile);

#endif // COMPILATOR_H__
