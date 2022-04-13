#include "List.h"

struct Hash_t
{
    void **Data;
    int  size;
    int  capacity;
};

int CeilPowerOfTwo (int value);

int CreateTable (Hash_t *target_table, int InitTableCap);

int TableInsert (Hash_t *target_table, type_t value, int64_t (*UserHash) (const void *key, int len));

void *GetElemByHash (Hash_t *target_table, int64_t hash);

int DestrTable (Hash_t *target_table, int (*elemDtor) (void *));
