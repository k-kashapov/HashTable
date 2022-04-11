#include "List.h"

struct Hash_t
{
    void **Data;
    int  size;
    int  capacity;
};

#ifndef TABLE_LOGS
    #define ErrFile stderr
#else
    static FILE *ErrFile = NULL
#endif

#define TABLE_ERR(msg, ...) LogErr (ErrFile, msg, __FUNCTION__, __LINE__, __VA_ARGS__);

int64_t SumHash (const void *data_ptr, int len);

int CreateTable (Hash_t *target_table);

int TableInsert (Hash_t *target_table, type_t value, int64_t (*UserHash) (const void *key, int len));

void *GetElemByHash (Hash_t *target_table, int64_t hash);

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *));
