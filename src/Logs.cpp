#include <stdio.h>
#include <stdarg.h>
#include "Logs.h"

int LogErr (FILE *errFile, const char *format, const char *func, int line, ...)
{
    fprintf (errFile, "ERROR: at \"%s\" (%d):\n",func, line);

    va_list errArgs;
    va_start (errArgs, format);

    vprintf (format, errArgs);

    va_end (errArgs);

    return 0;
}
