#include <stdio.h>

#include "proc.h"
#include "stack.h"

int main()
{
    FILE* code = fopen("programm_code.txt", "r");

    int codeArr[ArrSize] = {};

    int arg  = 0;
    int size = 0;

    for ( ; fscanf(code, "%d", &arg) != EOF; size++)
    {
        codeArr[size] = arg;
    }

    Run(codeArr);

    return 0;
}
