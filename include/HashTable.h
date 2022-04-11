#include "List.h"

struct TableElem
{
    void *Data;
    int  DataLen;
};

struct Hash_t
{
    TableElem *Data;
    int       size;
    int       capacity;
};

#ifndef TABLE_LOGS
    #define ErrFile stderr
#else
    static FILE *ErrFile = NULL
#endif

#define TABLE_ERR(msg, ...) LogErr (ErrFile, msg, __FUNCTION__, __LINE__, __VA_ARGS__);

int LogErr (FILE *errFile, const char *format, const char *func, int line, ...);

int CreateTable (Hash_t *target_table);

int TableInsert (Hash_t *target_table, const void *value, const void *key, int key_len);

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *));
