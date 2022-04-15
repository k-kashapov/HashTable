#ifndef LOGS_H
#define LOGS_H

#ifdef LOGGING
    #ifndef LOG_NAME
        #define LOG_NAME "GLOBAL"
    #endif
    
    static const char *LogPath = "logs/Log" LOG_NAME ".log";
    static FILE       *LogFile = fopen (LogPath, "at");
    #define LOG_MSG(msg, ...) LogMsg (LogFile, 0, msg, __FUNCTION__, __LINE__, __VA_ARGS__);
#else
    #define LogFile stderr
    #define LOG_MSG(msg, ...) ;
#endif

#define LOG_ERR(msg, ...) LogMsg (LogFile, 1, msg, __FUNCTION__, __LINE__, __VA_ARGS__);

int LogMsg (FILE *logFile, char isErr, const char *format, const char *func, int line, ...);

#undef LOG_NAME
#endif