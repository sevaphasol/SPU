#include <stdio.h>

#include "proc.h"
#include "stack.h"

int main(const int argc, const char* argv[])
{
    Proc_t proc = {};

    ProcCtor(&proc, argc, argv);

    Run(&proc);

    ProcDtor(&proc);

    return 0;
}
