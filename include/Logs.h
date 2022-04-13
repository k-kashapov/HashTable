#ifndef LOGS_H
#define LOGS_H
int LogMsg (FILE *logFile, char isErr, const char *format, const char *func, int line, ...);
#endif