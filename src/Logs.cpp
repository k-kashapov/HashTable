#include <stdio.h>
#include <stdarg.h>
#include "Logs.h"

int LogMsg (FILE *logFile, char isErr, const char *format, const char *func, int line, ...)
{
    fprintf (logFile, "%s: at \"%s\" (%d):\n", isErr ? "ERROR" : "Msg" , func, line);

    va_list logArgs;
    va_start (logArgs, format);

    vfprintf (logFile, format, logArgs);

    va_end (logArgs);

    return 0;
}
