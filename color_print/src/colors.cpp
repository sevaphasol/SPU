#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "colors.h"

//———————————————————————————————————————————————————————————————————//

void color_print(const char *color_code, const char *str, ...)
{
	va_list list;
    va_start(list, str);

	set_color(color_code);

    vprintf(str, list);

	reset_color();

    va_end(list);
}

//===================================================================//

void set_color(const char *color_code)
{
	printf("%s", color_code);
}

//===================================================================//

void reset_color()
{
	printf(WHITE_CODE);
}

//———————————————————————————————————————————————————————————————————//
